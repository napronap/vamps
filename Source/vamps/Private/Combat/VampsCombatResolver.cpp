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

bool UVampsCombatResolver::FAttackPlayback::IsActive() const
{
	const AVampsCharacter* ActiveFighter = Fighter.Get();
	if (!ActiveFighter || !ActiveFighter->CombatState.bIsAlive)
	{
		return false;
	}
	return ActiveFighter->CombatState.MontageInstanceID == MontageInstanceID;
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

void UVampsCombatResolver::ResolvePendingContacts()
{
	// Detach this frame's contacts before any callbacks can queue more.
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
		if (InterruptedAttackers.Contains(Playback.Fighter))
		{
			continue;
		}

		const AVampsCharacter* Attacker = Playback.Fighter.Get();
		if (!Attacker || !Attacker->CombatState.bIsAlive)
		{
			continue;
		}

		for (const FVampsPendingContact& Contact : Contacts)
		{
			if (Contact.Attacker != Playback.Fighter || Contact.AttackerMontageInstanceID != Playback.MontageInstanceID)
			{
				continue;
			}
			if (Attacker->CombatGeneration != Contact.AttackerGeneration)
			{
				continue;
			}

			const AVampsCharacter* Recipient = Contact.Recipient.Get();
			if (!Recipient || !Recipient->CombatState.bIsAlive)
			{
				continue;
			}
			if (Recipient->CombatGeneration != Contact.RecipientGeneration)
			{
				continue;
			}

			// The earlier attack takes priority if its trace was armed when this hit was captured.
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

	// Classify and credit the accepted hits before damage can change either fighter's state.
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
	if (!Recipient.CombatState.bCanDefend)
	{
		return;
	}

	// One request blocks the first hit in attack order and is consumed even during Guard.
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

		// Stop the whole group if a damage callback invalidated its recipient.
		if (!Recipient || !Recipient->CombatState.bIsAlive)
		{
			return;
		}
		if (Recipient->CombatGeneration != Contact.RecipientGeneration)
		{
			return;
		}

		// A dead attacker can still land a hit that was captured in this batch.
		const AVampsCharacter* Attacker = Contact.Attacker.Get();
		if (!Attacker || Attacker->CombatGeneration != Contact.AttackerGeneration)
		{
			continue;
		}

		Recipient->ReactToCombatResolution({EVampsCombatResolution::ApplyDamage, Contact});

		// Guard break beats an open hit, which beats a block. The first contact wins ties.
		if (!ReactionContact)
		{
			ReactionContact = &Contact;
			continue;
		}
		if (ReactionContact->bGuardBreak)
		{
			continue;
		}
		if (Contact.bGuardBreak || (ReactionContact->bBlocked && !Contact.bBlocked))
		{
			ReactionContact = &Contact;
		}
	}

	if (!ReactionContact)
	{
		return;
	}

	AVampsCharacter* Recipient = ReactionContact->Recipient.Get();
	if (!Recipient || !Recipient->CombatState.bIsAlive)
	{
		return;
	}
	if (Recipient->CombatGeneration != ReactionContact->RecipientGeneration)
	{
		return;
	}

	Recipient->ReactToCombatResolution({EVampsCombatResolution::ApplyReaction, *ReactionContact});
}

void UVampsCombatResolver::ResolveCombatFrame(UWorld* TickedWorld, ELevelTick TickType, float DeltaSeconds)
{
	// The world tick delegate is shared across worlds. Each resolver should consume its own queues
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

	// The match settles deaths, scores and round progression once this frame's combat is resolved
	if (AVampsMatchGameMode* MatchMode = Cast<AVampsMatchGameMode>(GetWorld()->GetAuthGameMode()))
	{
		MatchMode->CompleteCombatFrame();
	}

	AttackPlaybacks.RemoveAll(
	    [](const FAttackPlayback& Playback)
	    {
		    return !Playback.IsActive();
	    });
}
