#include "Combat/VampsCombatResolver.h"
#include "Characters/VampsCharacter.h"
#include "Combat/Attributes/VampsAttributeSet.h"
#include "Engine/World.h"
#include "GameModes/VampsMatchGameMode.h"

void UVampsCombatResolver::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	PostActorTickHandle =
	    FWorldDelegates::OnWorldPostActorTick.AddUObject(this, &UVampsCombatResolver::ResolveCombatFrame);
}

void UVampsCombatResolver::Deinitialize()
{
	FWorldDelegates::OnWorldPostActorTick.Remove(PostActorTickHandle);
	AttackPlaybacks.Reset();
	PendingContacts.Reset();
	AfterCombatResolutionCallbacks.Reset();

	Super::Deinitialize();
}

bool UVampsCombatResolver::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

bool UVampsCombatResolver::IsAttackPlaybackActive(const FAttackPlayback& Playback)
{
	const AVampsCharacter* PlaybackFighter = Playback.Fighter.Get();

	// A registered playback can stay active only while its fighter exists and is alive
	if (!PlaybackFighter || !PlaybackFighter->CombatState.bIsAlive)
	{
		return false;
	}

	return PlaybackFighter->CombatState.MontageInstanceID == Playback.MontageInstanceID;
}

void UVampsCombatResolver::StartAttack(AVampsCharacter& Fighter)
{
	AttackPlaybacks.Add({&Fighter, Fighter.CombatState.MontageInstanceID});
}

void UVampsCombatResolver::QueueCombatContact(FVampsPendingContact Contact)
{
	PendingContacts.Add(MoveTemp(Contact));
}

void UVampsCombatResolver::RunAfterCombatResolution(FSimpleDelegate Callback)
{
	AfterCombatResolutionCallbacks.Add(MoveTemp(Callback));
}

void UVampsCombatResolver::DiscardPendingCombatFor(AVampsCharacter& Fighter)
{
	const TWeakObjectPtr<AVampsCharacter> FighterReference(&Fighter);
	PendingContacts.RemoveAll(
	    [&FighterReference](const FVampsPendingContact& Contact)
	    {
		    return Contact.Attacker.HasSameIndexAndSerialNumber(FighterReference) ||
		           Contact.Recipient.HasSameIndexAndSerialNumber(FighterReference);
	    });
	AttackPlaybacks.RemoveAll(
	    [&FighterReference](const FAttackPlayback& Playback)
	    {
		    return Playback.Fighter.HasSameIndexAndSerialNumber(FighterReference);
	    });
	AfterCombatResolutionCallbacks.RemoveAll(
	    [&Fighter](const FSimpleDelegate& Callback)
	    {
		    return Callback.IsBoundToObject(&Fighter);
	    });
}

void UVampsCombatResolver::ResolvePendingContacts()
{
	// Detach this frame's contacts before any callbacks can queue more
	const TArray<FVampsPendingContact> Contacts = MoveTemp(PendingContacts);

	if (Contacts.IsEmpty())
	{
		return;
	}

	TMap<TWeakObjectPtr<AVampsCharacter>, TArray<FVampsPendingContact>> ContactsByRecipient;
	TMultiMap<TWeakObjectPtr<AVampsCharacter>, int32> EarlierAttackPlaybacks;
	TSet<TWeakObjectPtr<AVampsCharacter>> InterruptedAttackers;

	for (const FAttackPlayback& Playback : AttackPlaybacks)
	{
		// An earlier accepted hit suppresses this fighter's remaining outgoing contacts
		if (InterruptedAttackers.Contains(Playback.Fighter))
		{
			continue;
		}

		const AVampsCharacter* Attacker = Playback.Fighter.Get();

		// A fighter must still exist and be alive for its contacts to be accepted into the batch
		if (!Attacker || !Attacker->CombatState.bIsAlive)
		{
			continue;
		}

		for (const FVampsPendingContact& Contact : Contacts)
		{
			// Resolve only contacts captured for this fighter and this exact attack montage instance
			if (Contact.Attacker != Playback.Fighter || Contact.AttackerMontageInstanceID != Playback.MontageInstanceID)
			{
				continue;
			}

			const AVampsCharacter* Recipient = Contact.Recipient.Get();

			// Admit the hit only if the recipient still exists and is alive
			if (!Recipient || !Recipient->CombatState.bIsAlive)
			{
				continue;
			}

			// An earlier, uninterrupted attack protects the recipient if its damage window was open at capture
			if (EarlierAttackPlaybacks.FindPair(Contact.Recipient, Contact.RecipientAttackMontageInstanceID))
			{
				continue;
			}
			ContactsByRecipient.FindOrAdd(Contact.Recipient).Add(Contact);
			InterruptedAttackers.Add(Contact.Recipient);
			EarlierAttackPlaybacks.Remove(Contact.Recipient);
		}

		EarlierAttackPlaybacks.Add(Playback.Fighter, Playback.MontageInstanceID);
	}

	// Classify and credit the accepted hits before damage can change either fighter's state
	for (auto& RecipientContacts : ContactsByRecipient)
	{
		TArray<FVampsPendingContact>& ContactGroup = RecipientContacts.Value;
		ClassifyDefense(ContactGroup);

		for (const FVampsPendingContact& Contact : ContactGroup)
		{
			Contact.Attacker->ReactToCombatResolution({EVampsCombatResolution::CreditContact, Contact});
		}
	}

	for (const auto& RecipientContacts : ContactsByRecipient)
	{
		ApplyContactGroup(RecipientContacts.Value);
	}
}

void UVampsCombatResolver::ClassifyDefense(TArray<FVampsPendingContact>& Contacts)
{
	FVampsPendingContact& Contact = Contacts[0];
	AVampsCharacter& Recipient = *Contact.Recipient.Get();

	// The first hit stays unblocked unless the recipient can use an armed defense request
	if (!Recipient.CombatState.bCanDefend)
	{
		return;
	}

	// One request blocks the first hit in attack order and is consumed even during Guard
	Contact.bBlocked = true;
	const float ProjectedGuardLoad = Recipient.Attributes->GetGuardLoad() + Contact.GuardLoad;
	Contact.bGuardBreak = ProjectedGuardLoad >= Recipient.Attributes->GetMaxGuardLoad();
	Recipient.ReactToCombatResolution({EVampsCombatResolution::ConfirmDefense, Contact});
}

void UVampsCombatResolver::ApplyContactGroup(const TArray<FVampsPendingContact>& Contacts)
{
	const FVampsPendingContact* ReactionContact = nullptr;

	for (const FVampsPendingContact& Contact : Contacts)
	{
		AVampsCharacter* Recipient = Contact.Recipient.Get();

		// Stop this group if earlier damage killed or invalidated the recipient
		if (!Recipient || !Recipient->CombatState.bIsAlive)
		{
			return;
		}

		// The attacker must still exist. A fighter who died during the batch can land an accepted hit
		if (!Contact.Attacker.IsValid())
		{
			continue;
		}

		Recipient->ReactToCombatResolution({EVampsCombatResolution::ApplyDamage, Contact});

		// Use the first contact that reached the damage step as the initial reaction candidate
		if (!ReactionContact)
		{
			ReactionContact = &Contact;
			continue;
		}

		// A guard break already selected has the highest priority, including ties with later guard breaks
		if (ReactionContact->bGuardBreak)
		{
			continue;
		}

		// Prefer a guard break over either outcome, or an unblocked hit over a normal block
		if (Contact.bGuardBreak || (ReactionContact->bBlocked && !Contact.bBlocked))
		{
			ReactionContact = &Contact;
		}
	}

	// A reaction requires at least one contact to reach the damage step
	if (!ReactionContact)
	{
		return;
	}

	AVampsCharacter* Recipient = ReactionContact->Recipient.Get();

	// The last hit may have killed or invalidated the recipient before the reaction is applied
	if (!Recipient || !Recipient->CombatState.bIsAlive)
	{
		return;
	}

	Recipient->ReactToCombatResolution({EVampsCombatResolution::ApplyReaction, *ReactionContact});
}

void UVampsCombatResolver::ResolveCombatFrame(UWorld* TickedWorld, ELevelTick TickType, float DeltaSeconds)
{
	// The post-actor tick delegate is shared across worlds. Process only this resolver's world
	if (TickedWorld != GetWorld())
	{
		return;
	}

	ResolvePendingContacts();

	const TArray<FSimpleDelegate> Callbacks = MoveTemp(AfterCombatResolutionCallbacks);
	for (const FSimpleDelegate& Callback : Callbacks)
	{
		Callback.ExecuteIfBound();
	}

	// When the world runs a match, settle deaths, scores, and round progression after combat
	if (AVampsMatchGameMode* MatchMode = Cast<AVampsMatchGameMode>(GetWorld()->GetAuthGameMode()))
	{
		MatchMode->CompleteCombatFrame();
	}

	AttackPlaybacks.RemoveAll(
	    [](const FAttackPlayback& Playback)
	    {
		    return !IsAttackPlaybackActive(Playback);
	    });
}
