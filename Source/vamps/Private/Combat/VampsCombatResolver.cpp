#include "Combat/VampsCombatResolver.h"
#include "Characters/VampsCharacter.h"
#include "Characters/VampsLockOnComponent.h"
#include "Combat/VampsCombatMovementComponent.h"
#include "Combat/VampsCombatLog.h"
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
	PendingContacts.Reset();
	PendingDashes.Reset();
	PendingContinuations.Reset();
	PendingRecoveryEnds.Reset();

	Super::Deinitialize();
}

bool UVampsCombatResolver::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void UVampsCombatResolver::QueueCombatContact(AVampsCharacter& Attacker, AVampsCharacter& Recipient)
{
	const UVampsCombatSettings& AttackSettings = *Attacker.CombatSettings;
	const int32 StepIndex = Attacker.CombatState.ComboStep - 1;

	// The montage sets the number of combo steps. The data asset supplies settings for each step
	// This should probably never happen but I had errors with this at some point
	if (!AttackSettings.ComboSteps.IsValidIndex(StepIndex))
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: contact rejected: attacker='%s' has no settings for combo step %d in '%s'."),
		       __FUNCTION__,
		       *Attacker.GetPathName(),
		       Attacker.CombatState.ComboStep,
		       *AttackSettings.GetPathName());
		return;
	}

	// Capture the action, damage and geometry now (on contact). Either fighter can change state before resolution
	FVampsPendingContact& Contact = PendingContacts.AddDefaulted_GetRef();

	Contact.Attacker = &Attacker;
	Contact.Recipient = &Recipient;
	Contact.ActionNumber = Attacker.CombatState.ActionNumber;
	Contact.AttackerGeneration = Attacker.CombatGeneration;
	Contact.RecipientGeneration = Recipient.CombatGeneration;
	Contact.bCritical =
	    AttackSettings.CriticalChancePercent >= 100.f ||
	    (AttackSettings.CriticalChancePercent > 0.f && FMath::FRand() * 100.f < AttackSettings.CriticalChancePercent);
	Contact.ContactDamage = Contact.bCritical ? AttackSettings.CriticalDamage : AttackSettings.AttackDamage;
	Contact.BlockReduction = Recipient.CombatSettings->BlockDamageReduction;
	Contact.GuardLoad = AttackSettings.GuardLoadPerHit;
	Contact.AttackOrigin = Attacker.GetActorLocation();
	Contact.AttackDirection = Attacker.GetActorForwardVector().GetSafeNormal2D();
	Contact.StepSettings = AttackSettings.ComboSteps[StepIndex];
}

void UVampsCombatResolver::QueueCombatDash(AVampsCharacter& Fighter)
{
	PendingDashes.AddUnique(&Fighter);
}

void UVampsCombatResolver::QueueCombatContinuation(AVampsCharacter& Fighter)
{
	PendingContinuations.AddUnique(&Fighter);
}

void UVampsCombatResolver::QueueCombatRecoveryEnd(AVampsCharacter& Fighter)
{
	PendingRecoveryEnds.Add(&Fighter, Fighter.CombatState.ActionNumber);
}

void UVampsCombatResolver::DiscardPendingCombatFor(AVampsCharacter& Fighter)
{
	const TWeakObjectPtr<AVampsCharacter> FighterRef(&Fighter);
	PendingContinuations.Remove(FighterRef);
	PendingRecoveryEnds.Remove(FighterRef);
	PendingDashes.Remove(FighterRef);
}

int32 UVampsCombatResolver::GetContactRank(const FVampsPendingContact& Contact)
{
	// Favor the recipient's current lock-on target, then follow its target cycle
	const UVampsLockOnComponent* Selection = Contact.Recipient->LockOn;

	if (Selection->GetCurrentTarget() == Contact.Attacker.Get())
	{
		return -1;
	}

	const TArray<TWeakObjectPtr<AVampsCharacter>>& Candidates = Selection->GetCandidates();
	const int32 CandidateIndex = Candidates.Find(Contact.Attacker);
	return CandidateIndex == INDEX_NONE ? Candidates.Num() : CandidateIndex;
}

void UVampsCombatResolver::ResolvePendingContacts()
{
	// Consume the contacts queued by weapon ticks and leave the queue empty for the next frame
	TArray<FVampsPendingContact> Contacts = MoveTemp(PendingContacts);

	// Drop contacts for dead or invalid fighters. A generation change marks a reset or EndPlay
	Contacts.RemoveAll(
	    [](const FVampsPendingContact& Contact)
	    {
		    return !Contact.Attacker.IsValid() || !Contact.Recipient.IsValid() ||
		           !Contact.Attacker->CombatState.bIsAlive || !Contact.Recipient->CombatState.bIsAlive ||
		           Contact.Attacker->CombatGeneration != Contact.AttackerGeneration ||
		           Contact.Recipient->CombatGeneration != Contact.RecipientGeneration;
	    });

	// Group hits by recipient, then sort each group by lock-on priority and use attacker name for ties
	Contacts.StableSort(
	    [](const FVampsPendingContact& Left, const FVampsPendingContact& Right)
	    {
		    if (Left.Recipient != Right.Recipient)
		    {
			    return Left.Recipient->GetUniqueID() < Right.Recipient->GetUniqueID();
		    }
		    const int32 LeftRank = GetContactRank(Left);
		    const int32 RightRank = GetContactRank(Right);
		    return LeftRank != RightRank ? LeftRank < RightRank : Left.Attacker->GetName() < Right.Attacker->GetName();
	    });

	// Classify defense for the whole batch before any hit changes health or reactions
	for (int32 GroupStart = 0; GroupStart < Contacts.Num();)
	{
		AVampsCharacter* Recipient = Contacts[GroupStart].Recipient.Get();

		int32 GroupEnd = GroupStart + 1;

		while (GroupEnd < Contacts.Num() && Contacts[GroupEnd].Recipient.Get() == Recipient)
		{
			++GroupEnd;
		}

		if (!Recipient->CombatState.bCanDefend)
		{
			GroupStart = GroupEnd;
			continue;
		}

		// A fresh request favors the first incoming action the fighter is not already guarding
		const FVampsPendingContact* RequestedDefense = nullptr;

		if (Recipient->CombatState.bDefenseRequestArmed)
		{
			RequestedDefense = &Contacts[GroupStart];
			for (int32 ContactIndex = GroupStart; ContactIndex < GroupEnd; ++ContactIndex)
			{
				const FVampsPendingContact& Candidate = Contacts[ContactIndex];
				if (!Recipient->MatchesDefendedContact(Candidate))
				{
					RequestedDefense = &Candidate;
					break;
				}
			}
		}

		// The hit that reaches capacity still blocks and breaks guard. Later hits stay open
		float GuardLoadToDecrease = Recipient->Attributes->GetGuardLoad();

		for (int32 ContactIndex = GroupStart; ContactIndex < GroupEnd; ++ContactIndex)
		{
			FVampsPendingContact& Contact = Contacts[ContactIndex];

			// Block the selected action, or keep blocking the action remembered from a previous contact
			Contact.bBlocked = RequestedDefense ? Contact.Attacker == RequestedDefense->Attacker && Contact.ActionNumber == RequestedDefense->ActionNumber
			                                    : Recipient->MatchesDefendedContact(Contact);
			if (!Contact.bBlocked)
			{
				continue;
			}

			GuardLoadToDecrease += Contact.GuardLoad;

			if (GuardLoadToDecrease >= Recipient->Attributes->GetMaxGuardLoad())
			{
				Contact.bGuardBreak = true;
				break;
			}
		}

		// The first contact of the requested action blocks, so consume the request and remember the action
		if (RequestedDefense)
		{
			Recipient->RememberDefendedContact(*RequestedDefense);
		}

		GroupStart = GroupEnd;
	}

	// Credit the contact to the combo step only if its attack is still registered 
	// (fighters can continue combos only if they hit a target)
	for (const FVampsPendingContact& Contact : Contacts)
	{
		if (Contact.Attacker->CombatState.ActionNumber == Contact.ActionNumber &&
		    Contact.Attacker->CombatState.MontageInstanceID != INDEX_NONE)
		{
			Contact.Attacker->RegisterCombatContact(Contact.bBlocked);
		}
	}

	// Apply each recipient's damage and select one reaction for the group
	for (int32 GroupStart = 0; GroupStart < Contacts.Num();)
	{
		AVampsCharacter* Recipient = Contacts[GroupStart].Recipient.Get();
		int32 GroupEnd = GroupStart;
		const FVampsPendingContact* ReactionContact = nullptr;

		while (GroupEnd < Contacts.Num() && Contacts[GroupEnd].Recipient.Get() == Recipient)
		{
			const FVampsPendingContact& Contact = Contacts[GroupEnd++];
			AVampsCharacter* Attacker = Contact.Attacker.Get();

			// Earlier hits can kill this recipient and run callbacks that change either fighter
			// An attacker who died in this batch can still land their captured hit
			if (!Attacker || !Recipient || !Recipient->CombatState.bIsAlive ||
			    Recipient->CombatGeneration != Contact.RecipientGeneration ||
			    Attacker->CombatGeneration != Contact.AttackerGeneration)
			{
				continue;
			}

			Recipient->ReceiveCombatHit(Contact, *Attacker);

			// Guard breaks outrank open hits, which outrank blocks. Keep the first hit at equal severity
			const int32 Severity = Contact.bGuardBreak ? 2 : (Contact.bBlocked ? 0 : 1);

			const int32 PreviousSeverity =
			    ReactionContact ? (ReactionContact->bGuardBreak ? 2 : (ReactionContact->bBlocked ? 0 : 1)) : -1;

			if (Severity > PreviousSeverity)
			{
				ReactionContact = &Contact;
			}
		}

		// Apply reaction and displacement together if the recipient survived without a reset
		if (ReactionContact && ReactionContact->Recipient.IsValid() && Recipient->CombatState.bIsAlive &&
		    Recipient->CombatGeneration == ReactionContact->RecipientGeneration)
		{
			Recipient->ApplyCombatReaction(*ReactionContact);

			auto* CombatMovement = static_cast<UVampsCombatMovementComponent*>(Recipient->GetCharacterMovement());

			CombatMovement->ReceiveCombatDisplacement(ReactionContact->AttackOrigin,
			                                          ReactionContact->AttackDirection,
			                                          ReactionContact->StepSettings,
			                                          ReactionContact->bBlocked);
		}
		GroupStart = GroupEnd;
	}
}

void UVampsCombatResolver::ResolveCombatFrame(UWorld* TickedWorld, ELevelTick TickType, float DeltaSeconds)
{
	// The world tick delegate is shared across worlds. Each resolver should consume its own queues
	if (TickedWorld != GetWorld())
	{
		return;
	}

	// An impact can interrupt a continuation or cancel a dash before it costs energy
	ResolvePendingContacts();

	// Let accepted continuations start the next step before checking the queued recovery ends
	const TArray<TWeakObjectPtr<AVampsCharacter>> Continuations = MoveTemp(PendingContinuations);
	for (const TWeakObjectPtr<AVampsCharacter>& FighterRef : Continuations)
	{
		if (AVampsCharacter* Fighter = FighterRef.Get())
		{
			Fighter->ResolveCombatContinuation();
		}
	}

	// The character uses the captured action number to ignore recovery ends from a previous step
	const TMap<TWeakObjectPtr<AVampsCharacter>, int32> RecoveryEnds = MoveTemp(PendingRecoveryEnds);
	for (const auto& RecoveryEnd : RecoveryEnds)
	{
		if (AVampsCharacter* Fighter = RecoveryEnd.Key.Get())
		{
			Fighter->ResolveCombatRecoveryEnd(RecoveryEnd.Value);
		}
	}

	// A reaction or a new attack step can clear the dash request before we get here
	const TArray<TWeakObjectPtr<AVampsCharacter>> Dashes = MoveTemp(PendingDashes);
	for (const TWeakObjectPtr<AVampsCharacter>& FighterRef : Dashes)
	{
		if (AVampsCharacter* Fighter = FighterRef.Get(); Fighter && Fighter->CombatState.bDashRequested)
		{
			Fighter->ResolveCombatDash();
		}
	}

	// The match settles deaths, scores and round progression once this frame's combat is resolved
	if (AVampsMatchGameMode* MatchMode = Cast<AVampsMatchGameMode>(GetWorld()->GetAuthGameMode()))
	{
		MatchMode->CompleteCombatFrame();
	}
}
