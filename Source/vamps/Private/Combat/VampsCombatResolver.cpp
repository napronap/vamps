#include "Combat/VampsCombatResolver.h"
#include "Characters/VampsCharacter.h"
#include "Characters/VampsLockOnComponent.h"
#include "Combat/VampsCombatMovementComponent.h"
#include "Combat/VampsCombatLog.h"
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
	float SectionDuration;
	if (!Attacker->TryCalculateCombatSectionDuration(SectionDuration))
	{
		return;
	}
	const FVampsCombatStepSettings& StepSettings =
	    Attacker->CombatSettings->ComboSteps[Attacker->CombatState.ComboStep - 1];
	const float RestrictionDuration = SectionDuration + StepSettings.AdditionalOffensiveRestrictionDuration;
	if (!FMath::IsFinite(StepSettings.AdditionalOffensiveRestrictionDuration) ||
	    StepSettings.AdditionalOffensiveRestrictionDuration < 0.f || !FMath::IsFinite(RestrictionDuration))
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: contact rejected for '%s', CombatSettings '%s', step %d: "
		            "AdditionalOffensiveRestrictionDuration=%f must be finite and nonnegative; section=%f, total=%f."),
		       __FUNCTION__,
		       *Attacker->GetPathName(),
		       *Attacker->CombatSettings->GetPathName(),
		       Attacker->CombatState.ComboStep,
		       StepSettings.AdditionalOffensiveRestrictionDuration,
		       SectionDuration,
		       RestrictionDuration);
		return;
	}
	FVampsPendingContact& Contact = PendingContacts.AddDefaulted_GetRef();
	Contact.Attacker = Attacker;
	Contact.Recipient = Recipient;
	Contact.ActionNumber = Attacker->CombatState.ActionNumber;
	Contact.OffensiveRestrictionDuration = RestrictionDuration;
	Contact.AttackOrigin = Attacker->GetActorLocation();
	Contact.AttackDirection = Attacker->GetActorForwardVector().GetSafeNormal2D();
	Contact.StepSettings = StepSettings;
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
		           !Contact.Attacker->CombatState.bIsAlive || !Contact.Recipient->CombatState.bIsAlive;
	    });

	TMap<AVampsCharacter*, TArray<AVampsCharacter*>> BlockedAttackers;
	for (FVampsPendingContact& Contact : Contacts)
	{
		Contact.bBlocked = Contact.Recipient->CombatState.bCanDefend;
		if (Contact.bBlocked)
		{
			BlockedAttackers.FindOrAdd(Contact.Recipient.Get()).AddUnique(Contact.Attacker.Get());
		}
	}

	TSet<AVampsCharacter*> InvalidRecipients;
	for (const auto& BlockedEntry : BlockedAttackers)
	{
		AVampsCharacter* Defender = BlockedEntry.Key;
		AVampsCharacter* SelectedAttacker = Defender->LockOn->GetCurrentTarget();
		if (!BlockedEntry.Value.Contains(SelectedAttacker))
		{
			SelectedAttacker = nullptr;
			for (const TWeakObjectPtr<AVampsCharacter>& Candidate : Defender->LockOn->GetCandidates())
			{
				if (BlockedEntry.Value.Contains(Candidate.Get()))
				{
					SelectedAttacker = Candidate.Get();
					break;
				}
			}
		}
		if (!SelectedAttacker)
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: contacts for '%s' rejected; no blocked attacker is registered in its LockOn candidates. "
			            "Defense has not been consumed."),
			       __FUNCTION__,
			       *Defender->GetPathName());
			InvalidRecipients.Add(Defender);
			continue;
		}
		Defender->ConsumeDefenseRequest();
		Defender->LockOn->SelectTarget(SelectedAttacker);
	}
	Contacts.RemoveAll(
	    [&InvalidRecipients](const FVampsPendingContact& Contact)
	    {
		    return InvalidRecipients.Contains(Contact.Recipient.Get());
	    });

	for (const FVampsPendingContact& Contact : Contacts)
	{
		if (Contact.bBlocked)
		{
			Contact.Recipient->ApplyOffensiveRestriction(Contact.OffensiveRestrictionDuration);
		}
		if (Contact.Attacker->CombatState.ActionNumber == Contact.ActionNumber)
		{
			Contact.Attacker->RegisterCombatContact(Contact.bBlocked);
		}
		Contact.Recipient->ReceiveCombatHit(Contact.Attacker.Get(), Contact.bBlocked);
	}

	TMap<AVampsCharacter*, const FVampsPendingContact*> MotionContacts;
	for (const FVampsPendingContact& Contact : Contacts)
	{
		AVampsCharacter* Recipient = Contact.Recipient.Get();
		const FVampsPendingContact*& ChosenContact = MotionContacts.FindOrAdd(Recipient);
		const auto ContactRank = [Recipient](const FVampsPendingContact& Candidate)
		{
			if (Candidate.Attacker.Get() == Recipient->LockOn->GetCurrentTarget())
			{
				return -1;
			}
			return Recipient->LockOn->GetCandidates().IndexOfByPredicate(
			    [&Candidate](const TWeakObjectPtr<AVampsCharacter>& Fighter)
			    {
				    return Fighter == Candidate.Attacker;
			    });
		};
		if (!ChosenContact || (ChosenContact->bBlocked && !Contact.bBlocked) ||
		    (ChosenContact->bBlocked == Contact.bBlocked && ContactRank(Contact) < ContactRank(*ChosenContact)))
		{
			ChosenContact = &Contact;
		}
	}
	for (const auto& MotionEntry : MotionContacts)
	{
		AVampsCharacter* Recipient = MotionEntry.Key;
		if (Recipient->CombatState.bIsAlive)
		{
			const FVampsPendingContact& Contact = *MotionEntry.Value;
			UVampsCombatMovementComponent* CombatMovement =
			    Cast<UVampsCombatMovementComponent>(Recipient->GetCharacterMovement());
			if (!CombatMovement)
			{
				UE_LOG(LogVampsCombat,
				       Error,
				       TEXT("%hs: displacement rejected for '%s'; movement component '%s' is not "
				            "VampsCombatMovementComponent."),
				       __FUNCTION__,
				       *Recipient->GetPathName(),
				       *GetPathNameSafe(Recipient->GetCharacterMovement()));
				continue;
			}
			CombatMovement->ReceiveCombatDisplacement(
			    Contact.AttackOrigin, Contact.AttackDirection, Contact.StepSettings, Contact.bBlocked);
		}
	}

	const TArray<TWeakObjectPtr<AVampsCharacter>> Continuations = MoveTemp(PendingContinuations);
	for (const TWeakObjectPtr<AVampsCharacter>& Fighter : Continuations)
	{
		if (Fighter.IsValid() && !Fighter->IsActorBeingDestroyed())
		{
			Fighter->ResolveCombatContinuation();
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
