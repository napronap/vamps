#include "Combat/VampsCombatResolver.h"
#include "Characters/VampsCharacter.h"
#include "Characters/VampsLockOnComponent.h"
#include "Combat/VampsCombatMovementComponent.h"
#include "Combat/VampsCombatLog.h"
#include "Combat/Attributes/VampsAttributeSet.h"
#include "Engine/World.h"

void UVampsCombatResolver::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	PostActorTickHandle =
	    FWorldDelegates::OnWorldPostActorTick.AddUObject(this, &UVampsCombatResolver::ResolveCombatContacts);
}

void UVampsCombatResolver::Deinitialize()
{
	FWorldDelegates::OnWorldPostActorTick.Remove(PostActorTickHandle);
	PendingContacts.Reset();
	PendingDashes.Reset();
	PendingContinuations.Reset();
	PendingLocomotion.Reset();
	Super::Deinitialize();
}

bool UVampsCombatResolver::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void UVampsCombatResolver::QueueCombatContact(AVampsCharacter* Attacker, AVampsCharacter* Recipient)
{
	if (!IsValid(Attacker) || !IsValid(Recipient) || Attacker == Recipient || !Attacker->HasAuthority() ||
	    !Recipient->HasAuthority() || Attacker->GetWorld() != GetWorld() || Recipient->GetWorld() != GetWorld())
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: contact rejected: attacker='%s', recipient='%s'. Expected distinct valid authoritative "
		            "fighters in resolver world '%s'."),
		       __FUNCTION__,
		       *GetPathNameSafe(Attacker),
		       *GetPathNameSafe(Recipient),
		       *GetPathNameSafe(GetWorld()));
		return;
	}
	if (!Attacker->CombatSettings ||
	    !Attacker->CombatSettings->ComboSteps.IsValidIndex(Attacker->CombatState.ComboStep - 1) || !Recipient->LockOn ||
	    !Cast<UVampsCombatMovementComponent>(Recipient->GetCharacterMovement()))
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: contact rejected: attacker='%s', step=%d, CombatSettings='%s', recipient='%s'. Required step "
		            "settings, recipient LockOn or combat movement are missing."),
		       __FUNCTION__,
		       *Attacker->GetPathName(),
		       Attacker->CombatState.ComboStep,
		       *GetPathNameSafe(Attacker->CombatSettings),
		       *Recipient->GetPathName());
		return;
	}
	const UVampsCombatSettings* AttackSettings = Attacker->CombatSettings;
	const UVampsCombatSettings* DefenseSettings = Recipient->CombatSettings;
	const FVampsCombatStepSettings& StepSettings = AttackSettings->ComboSteps[Attacker->CombatState.ComboStep - 1];
	if (!DefenseSettings || !AttackSettings->HasValidReactionSettings() ||
	    !DefenseSettings->HasValidReactionSettings() || !Recipient->Attributes ||
	    !FMath::IsFinite(Attacker->AttackDamage) || Attacker->AttackDamage < 0.f ||
	    !FMath::IsFinite(StepSettings.ContactReactionDuration) || StepSettings.ContactReactionDuration <= 0.f)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: contact rejected: attacker='%s', settings='%s', damage=%f, "
		            "reaction=%f, recipient='%s', settings='%s'. Required finite nonnegative damage and positive "
		            "reaction duration."),
		       __FUNCTION__,
		       *Attacker->GetPathName(),
		       *GetPathNameSafe(AttackSettings),
		       Attacker->AttackDamage,
		       StepSettings.ContactReactionDuration,
		       *Recipient->GetPathName(),
		       *GetPathNameSafe(DefenseSettings));
		return;
	}
	FVampsPendingContact& Contact = PendingContacts.AddDefaulted_GetRef();
	Contact.Attacker = Attacker;
	Contact.Recipient = Recipient;
	Contact.ActionNumber = Attacker->CombatState.ActionNumber;
	Contact.AttackerGeneration = Attacker->CombatGeneration;
	Contact.RecipientGeneration = Recipient->CombatGeneration;
	Contact.ContactTime = GetWorld()->GetTimeSeconds();
	Contact.bCritical =
	    AttackSettings->CriticalChancePercent >= 100.f ||
	    (AttackSettings->CriticalChancePercent > 0.f && FMath::FRand() * 100.f < AttackSettings->CriticalChancePercent);
	Contact.ContactDamage = Contact.bCritical ? AttackSettings->CriticalDamage : Attacker->AttackDamage;
	Contact.BlockReduction = DefenseSettings->BlockDamageReduction;
	Contact.GuardLoad = AttackSettings->GuardLoadPerHit;
	Contact.AttackOrigin = Attacker->GetActorLocation();
	Contact.AttackDirection = Attacker->GetActorForwardVector().GetSafeNormal2D();
	Contact.StepSettings = StepSettings;
}

void UVampsCombatResolver::QueueCombatDash(AVampsCharacter* Fighter)
{
	if (!IsValid(Fighter) || !Fighter->HasAuthority() || Fighter->GetWorld() != GetWorld())
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: dash rejected for '%s'; expected an authoritative fighter in '%s'."),
		       __FUNCTION__,
		       *GetPathNameSafe(Fighter),
		       *GetPathName());
		return;
	}
	PendingDashes.AddUnique(Fighter);
}

void UVampsCombatResolver::QueueCombatContinuation(AVampsCharacter* Fighter)
{
	if (!IsValid(Fighter) || !Fighter->HasAuthority() || Fighter->GetWorld() != GetWorld())
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: continuation rejected for '%s'; expected a valid authoritative fighter in world '%s'."),
		       __FUNCTION__,
		       *GetPathNameSafe(Fighter),
		       *GetPathNameSafe(GetWorld()));
		return;
	}
	PendingContinuations.AddUnique(Fighter);
}

void UVampsCombatResolver::QueueCombatLocomotion(AVampsCharacter* Fighter)
{
	if (!IsValid(Fighter) || !Fighter->HasAuthority() || Fighter->GetWorld() != GetWorld())
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: locomotion request rejected for '%s'; expected a valid authoritative fighter in world '%s'."),
		       __FUNCTION__,
		       *GetPathNameSafe(Fighter),
		       *GetPathNameSafe(GetWorld()));
		return;
	}
	PendingLocomotion.AddUnique(Fighter);
}

void UVampsCombatResolver::DiscardContactsFor(const AVampsCharacter* Fighter)
{
	PendingDashes.RemoveAll(
	    [Fighter](const TWeakObjectPtr<AVampsCharacter>& Candidate)
	    {
		    return Candidate.Get() == Fighter;
	    });
	PendingContacts.RemoveAll(
	    [Fighter](const FVampsPendingContact& Contact)
	    {
		    return Contact.Attacker.Get() == Fighter || Contact.Recipient.Get() == Fighter;
	    });
	PendingContinuations.RemoveAll(
	    [Fighter](const TWeakObjectPtr<AVampsCharacter>& Candidate)
	    {
		    return Candidate.Get() == Fighter;
	    });
	PendingLocomotion.RemoveAll(
	    [Fighter](const TWeakObjectPtr<AVampsCharacter>& Candidate)
	    {
		    return Candidate.Get() == Fighter;
	    });
}

void UVampsCombatResolver::ResolveCombatContacts(UWorld* TickedWorld, ELevelTick TickType, float DeltaSeconds)
{
	if (TickedWorld != GetWorld())
	{
		return;
	}

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

	for (int32 GroupStart = 0; GroupStart < Contacts.Num();)
	{
		AVampsCharacter* Recipient = Contacts[GroupStart].Recipient.Get();
		int32 GroupEnd = GroupStart + 1;
		while (GroupEnd < Contacts.Num() && Contacts[GroupEnd].Recipient.Get() == Recipient)
		{
			++GroupEnd;
		}
		FVampsPendingContact* RequestedDefense = nullptr;
		if (Recipient->CombatState.bCanDefend && Recipient->CombatState.bDefenseRequestArmed &&
		    GetWorld()->GetTimeSeconds() < Recipient->CombatState.DefenseRequestEndsAt)
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
			if (Recipient->ReceiveCombatHit(Contact))
			{
				const int32 Severity = Contact.bGuardBreak ? 2 : (Contact.bBlocked ? 0 : 1);
				const int32 PreviousSeverity =
				    ReactionContact ? (ReactionContact->bGuardBreak ? 2 : (ReactionContact->bBlocked ? 0 : 1)) : -1;
				if (Severity > PreviousSeverity)
				{
					ReactionContact = &Contact;
				}
			}
		}
		if (ReactionContact && Recipient->CombatState.bIsAlive)
		{
			Recipient->ApplyCombatReaction(*ReactionContact);
			UVampsCombatMovementComponent* CombatMovement =
			    Cast<UVampsCombatMovementComponent>(Recipient->GetCharacterMovement());
			if (CombatMovement)
			{
				CombatMovement->ReceiveCombatDisplacement(ReactionContact->AttackOrigin,
				                                          ReactionContact->AttackDirection,
				                                          ReactionContact->StepSettings,
				                                          ReactionContact->bBlocked);
			}
			else
			{
				UE_LOG(LogVampsCombat,
				       Error,
				       TEXT("%hs: '%s': cannot apply reaction displacement without VampsCombatMovementComponent."),
				       __FUNCTION__,
				       *Recipient->GetPathName());
			}
		}
		GroupStart = GroupEnd;
	}

	const TArray<TWeakObjectPtr<AVampsCharacter>> Continuations = MoveTemp(PendingContinuations);
	for (const TWeakObjectPtr<AVampsCharacter>& Fighter : Continuations)
	{
		if (Fighter.IsValid() && !Fighter->IsActorBeingDestroyed())
		{
			Fighter->ResolveCombatContinuation();
		}
	}
	const TArray<TWeakObjectPtr<AVampsCharacter>> Dashes = MoveTemp(PendingDashes);
	for (const TWeakObjectPtr<AVampsCharacter>& Fighter : Dashes)
	{
		if (Fighter.IsValid() && !Fighter->IsActorBeingDestroyed())
		{
			Fighter->ResolveCombatDash();
		}
	}
	const TArray<TWeakObjectPtr<AVampsCharacter>> LocomotionRequests = MoveTemp(PendingLocomotion);
	for (const TWeakObjectPtr<AVampsCharacter>& Fighter : LocomotionRequests)
	{
		if (Fighter.IsValid() && !Fighter->IsActorBeingDestroyed())
		{
			Fighter->ResolveCombatLocomotion();
		}
	}
}
