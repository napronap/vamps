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
	if (!Attacker.HasAuthority() || !Recipient.HasAuthority() || Attacker.GetWorld() != GetWorld() ||
	    Recipient.GetWorld() != GetWorld())
	{
		UE_LOG(
		    LogVampsCombat,
		    Error,
		    TEXT(
		        "%hs: contact rejected: attacker='%s', recipient='%s'. Expected authoritative fighters in world '%s'."),
		    __FUNCTION__,
		    *Attacker.GetPathName(),
		    *Recipient.GetPathName(),
		    *GetPathNameSafe(GetWorld()));
		return;
	}
	const UVampsCombatSettings* AttackSettings = Attacker.CombatSettings;
	const int32 StepIndex = Attacker.CombatState.ComboStep - 1;
	if (!AttackSettings || !AttackSettings->ComboSteps.IsValidIndex(StepIndex))
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: contact rejected: attacker='%s' has no settings for combo step %d in '%s'."),
		       __FUNCTION__,
		       *Attacker.GetPathName(),
		       Attacker.CombatState.ComboStep,
		       *GetPathNameSafe(AttackSettings));
		return;
	}
	const UVampsCombatSettings* DefenseSettings = Recipient.CombatSettings;
	if (!Recipient.AttackHandle.IsValid() || !DefenseSettings || !Recipient.LockOn || !Recipient.Attributes)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: contact rejected: recipient '%s' has incomplete combat initialization with settings '%s'."),
		       __FUNCTION__,
		       *Recipient.GetPathName(),
		       *GetPathNameSafe(DefenseSettings));
		return;
	}
	FVampsPendingContact& Contact = PendingContacts.AddDefaulted_GetRef();
	Contact.Attacker = &Attacker;
	Contact.Recipient = &Recipient;
	Contact.ActionNumber = Attacker.CombatState.ActionNumber;
	Contact.AttackerGeneration = Attacker.CombatGeneration;
	Contact.RecipientGeneration = Recipient.CombatGeneration;
	Contact.bCritical =
	    AttackSettings->CriticalChancePercent >= 100.f ||
	    (AttackSettings->CriticalChancePercent > 0.f && FMath::FRand() * 100.f < AttackSettings->CriticalChancePercent);
	Contact.ContactDamage = Contact.bCritical ? AttackSettings->CriticalDamage : AttackSettings->AttackDamage;
	Contact.BlockReduction = DefenseSettings->BlockDamageReduction;
	Contact.GuardLoad = AttackSettings->GuardLoadPerHit;
	Contact.AttackOrigin = Attacker.GetActorLocation();
	Contact.AttackDirection = Attacker.GetActorForwardVector().GetSafeNormal2D();
	Contact.StepSettings = AttackSettings->ComboSteps[StepIndex];
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
	PendingContacts.RemoveAll(
	    [&FighterRef](const FVampsPendingContact& Contact)
	    {
		    return Contact.Attacker.HasSameIndexAndSerialNumber(FighterRef) ||
		           Contact.Recipient.HasSameIndexAndSerialNumber(FighterRef);
	    });
	PendingContinuations.Remove(FighterRef);
	PendingRecoveryEnds.Remove(FighterRef);
	PendingDashes.Remove(FighterRef);
}

void UVampsCombatResolver::ResolvePendingContacts()
{
	// A death callback clears queued work without removing exchanges already captured for this frame.
	TArray<FVampsPendingContact> Contacts = MoveTemp(PendingContacts);
	Contacts.RemoveAll(
	    [](const FVampsPendingContact& Contact)
	    {
		    return !Contact.Attacker.IsValid() || !Contact.Recipient.IsValid() ||
		           Contact.Attacker->IsActorBeingDestroyed() || Contact.Recipient->IsActorBeingDestroyed() ||
		           !Contact.Attacker->CombatState.bIsAlive || !Contact.Recipient->CombatState.bIsAlive ||
		           Contact.Attacker->CombatGeneration != Contact.AttackerGeneration ||
		           Contact.Recipient->CombatGeneration != Contact.RecipientGeneration;
	    });
	const auto ContactRank = [](const FVampsPendingContact& Contact)
	{
		const UVampsLockOnComponent* Selection = Contact.Recipient->LockOn;
		if (Selection->GetCurrentTarget() == Contact.Attacker.Get())
		{
			return -1;
		}
		const int32 CandidateIndex = Selection->GetCandidates().IndexOfByPredicate(
		    [&Contact](const TWeakObjectPtr<AVampsCharacter>& Candidate)
		    {
			    return Candidate == Contact.Attacker;
		    });
		return CandidateIndex == INDEX_NONE ? MAX_int32 : CandidateIndex;
	};
	Contacts.StableSort(
	    [&ContactRank](const FVampsPendingContact& Left, const FVampsPendingContact& Right)
	    {
		    if (Left.Recipient != Right.Recipient)
		    {
			    return Left.Recipient->GetPathName() < Right.Recipient->GetPathName();
		    }
		    const int32 LeftRank = ContactRank(Left);
		    const int32 RightRank = ContactRank(Right);
		    return LeftRank != RightRank ? LeftRank < RightRank
		                                 : Left.Attacker->GetPathName() < Right.Attacker->GetPathName();
	    });

	// Classify defense and guard breaks before any contact changes health or reactions.
	for (int32 GroupStart = 0; GroupStart < Contacts.Num();)
	{
		AVampsCharacter* Recipient = Contacts[GroupStart].Recipient.Get();
		int32 GroupEnd = GroupStart + 1;
		while (GroupEnd < Contacts.Num() && Contacts[GroupEnd].Recipient.Get() == Recipient)
		{
			++GroupEnd;
		}
		FVampsPendingContact* RequestedDefense = nullptr;
		if (Recipient->CombatState.bCanDefend && Recipient->CombatState.bDefenseRequestArmed)
		{
			for (int32 ContactIndex = GroupStart; ContactIndex < GroupEnd; ++ContactIndex)
			{
				FVampsPendingContact& Candidate = Contacts[ContactIndex];
				if (!RequestedDefense || (Recipient->MatchesDefendedContact(*RequestedDefense) &&
				                          !Recipient->MatchesDefendedContact(Candidate)))
				{
					RequestedDefense = &Candidate;
				}
			}
		}
		float ProjectedLoad = Recipient->Attributes->GetGuardLoad();
		bool bGuardBroken = false;
		for (int32 ContactIndex = GroupStart; ContactIndex < GroupEnd; ++ContactIndex)
		{
			FVampsPendingContact& Contact = Contacts[ContactIndex];
			const bool bMatchesRequest = RequestedDefense && Contact.Attacker == RequestedDefense->Attacker &&
			                             Contact.ActionNumber == RequestedDefense->ActionNumber;
			Contact.bBlocked = Recipient->CombatState.bCanDefend && !bGuardBroken &&
			                   (RequestedDefense ? bMatchesRequest : Recipient->MatchesDefendedContact(Contact));
			if (Contact.bBlocked)
			{
				ProjectedLoad += Contact.GuardLoad;
				Contact.bGuardBreak = ProjectedLoad >= Recipient->Attributes->GetMaxGuardLoad();
				bGuardBroken |= Contact.bGuardBreak;
			}
		}
		if (RequestedDefense && RequestedDefense->bBlocked)
		{
			Recipient->RememberDefendedContact(*RequestedDefense);
		}
		GroupStart = GroupEnd;
	}

	for (const FVampsPendingContact& Contact : Contacts)
	{
		if (Contact.Attacker->CombatState.ActionNumber == Contact.ActionNumber &&
		    Contact.Attacker->CombatState.MontageInstanceID != INDEX_NONE)
		{
			Contact.Attacker->RegisterCombatContact(Contact.bBlocked);
		}
	}
	for (int32 GroupStart = 0; GroupStart < Contacts.Num();)
	{
		AVampsCharacter* Recipient = Contacts[GroupStart].Recipient.Get();
		int32 GroupEnd = GroupStart;
		const FVampsPendingContact* ReactionContact = nullptr;
		while (GroupEnd < Contacts.Num() && Contacts[GroupEnd].Recipient.Get() == Recipient)
		{
			const FVampsPendingContact& Contact = Contacts[GroupEnd++];
			AVampsCharacter* Attacker = Contact.Attacker.Get();
			if (!Recipient || !Recipient->CombatState.bIsAlive ||
			    Recipient->CombatGeneration != Contact.RecipientGeneration || !Attacker ||
			    Attacker->CombatGeneration != Contact.AttackerGeneration ||
			    !Recipient->ReceiveCombatHit(Contact, *Attacker))
			{
				continue;
			}
			const int32 Severity = Contact.bGuardBreak ? 2 : (Contact.bBlocked ? 0 : 1);
			const int32 PreviousSeverity =
			    ReactionContact ? (ReactionContact->bGuardBreak ? 2 : (ReactionContact->bBlocked ? 0 : 1)) : -1;
			if (Severity > PreviousSeverity)
			{
				ReactionContact = &Contact;
			}
		}
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
	if (TickedWorld != GetWorld())
	{
		return;
	}
	// An impact can interrupt a continuation or cancel a dash before it costs energy.
	ResolvePendingContacts();
	const TArray<TWeakObjectPtr<AVampsCharacter>> Continuations = MoveTemp(PendingContinuations);
	for (const TWeakObjectPtr<AVampsCharacter>& Fighter : Continuations)
	{
		if (Fighter.IsValid() && !Fighter->IsActorBeingDestroyed())
		{
			Fighter->ResolveCombatContinuation();
		}
	}
	const TMap<TWeakObjectPtr<AVampsCharacter>, int32> RecoveryEnds = MoveTemp(PendingRecoveryEnds);
	for (const auto& RecoveryEnd : RecoveryEnds)
	{
		if (RecoveryEnd.Key.IsValid() && !RecoveryEnd.Key->IsActorBeingDestroyed())
		{
			RecoveryEnd.Key->ResolveCombatRecoveryEnd(RecoveryEnd.Value);
		}
	}
	const TArray<TWeakObjectPtr<AVampsCharacter>> Dashes = MoveTemp(PendingDashes);
	for (const TWeakObjectPtr<AVampsCharacter>& Fighter : Dashes)
	{
		if (Fighter.IsValid() && !Fighter->IsActorBeingDestroyed() && Fighter->CombatState.bDashRequested)
		{
			Fighter->ResolveCombatDash();
		}
	}
	if (AVampsMatchGameMode* MatchMode = Cast<AVampsMatchGameMode>(GetWorld()->GetAuthGameMode()))
	{
		MatchMode->CompleteCombatFrame();
	}
}
