#include "Characters/VampsLockOnComponent.h"
#include "Characters/VampsCharacter.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameModes/VampsMatchGameMode.h"

void UVampsLockOnComponent::InitializeCandidates()
{
	AVampsCharacter& OwnerFighter = *static_cast<AVampsCharacter*>(GetOwner());
	AVampsMatchGameMode* MatchMode = GetWorld()->GetAuthGameMode<AVampsMatchGameMode>();

	for (AVampsCharacter* Fighter : TActorRange<AVampsCharacter>(GetWorld()))
	{
		if (Fighter != &OwnerFighter && Fighter->HasActorBegunPlay() &&
		    (!MatchMode || MatchMode->AreEnemies(&OwnerFighter, Fighter)))
		{
			RegisterCandidate(*Fighter);
			Fighter->LockOn->RegisterCandidate(OwnerFighter);
			Fighter->LockOn->RefreshTarget();
		}
	}

	if (MatchMode)
	{
		MatchMode->OnRoundPhaseChanged.AddUObject(this, &UVampsLockOnComponent::RefreshTarget);
	}

	RefreshTarget();
}

void UVampsLockOnComponent::EndPlay(const EEndPlayReason::Type Reason)
{
	if (AVampsMatchGameMode* MatchMode = GetWorld()->GetAuthGameMode<AVampsMatchGameMode>())
	{
		MatchMode->OnRoundPhaseChanged.RemoveAll(this);
	}
	for (const TWeakObjectPtr<AVampsCharacter>& Candidate : Candidates)
	{
		if (AVampsCharacter* Fighter = Candidate.Get())
		{
			Fighter->OnCombatAliveChanged.RemoveAll(this);
			Fighter->OnEndPlay.RemoveDynamic(this, &UVampsLockOnComponent::HandleCandidateEndPlay);
		}
	}

	Candidates.Reset();
	SetCurrentTarget(nullptr);

	Super::EndPlay(Reason);
}

void UVampsLockOnComponent::RefreshTarget()
{
	if (!IsEligible(CurrentTarget.Get()))
	{
		CycleTarget();
	}
}

AVampsCharacter* UVampsLockOnComponent::GetCurrentTarget() const
{
	return CurrentTarget.Get();
}

const TArray<TWeakObjectPtr<AVampsCharacter>>& UVampsLockOnComponent::GetCandidates() const
{
	return Candidates;
}

void UVampsLockOnComponent::CycleTarget()
{
	const int32 CurrentIndex = Candidates.IndexOfByPredicate(
	    [this](const TWeakObjectPtr<AVampsCharacter>& Candidate)
	    {
		    return Candidate.HasSameIndexAndSerialNumber(CurrentTarget);
	    });

	CycleTargetStartingAt(CurrentIndex + 1);
}

void UVampsLockOnComponent::CycleTargetStartingAt(int32 FirstCandidateIndex)
{
	for (int32 Offset = 0; Offset < Candidates.Num(); ++Offset)
	{
		AVampsCharacter* Candidate = Candidates[(FirstCandidateIndex + Offset) % Candidates.Num()].Get();

		if (IsEligible(Candidate))
		{
			SetCurrentTarget(Candidate);
			return;
		}
	}

	SetCurrentTarget(nullptr);
}

void UVampsLockOnComponent::SelectTarget(AVampsCharacter* NewTarget)
{
	if (IsEligible(NewTarget))
	{
		SetCurrentTarget(NewTarget);
	}
}

void UVampsLockOnComponent::SetCurrentTarget(AVampsCharacter* NewTarget)
{
	const TWeakObjectPtr<AVampsCharacter> NextTarget(NewTarget);

	if (!CurrentTarget.HasSameIndexAndSerialNumber(NextTarget))
	{
		CurrentTarget = NextTarget;
		OnTargetChanged.Broadcast(NewTarget);
	}
}

void UVampsLockOnComponent::RegisterCandidate(AVampsCharacter& Candidate)
{
	if (!Candidates.Contains(&Candidate))
	{
		Candidates.Add(&Candidate);
		Candidate.OnCombatAliveChanged.AddUObject(this, &UVampsLockOnComponent::RefreshTarget);
		Candidate.OnEndPlay.AddDynamic(this, &UVampsLockOnComponent::HandleCandidateEndPlay);
	}
}

void UVampsLockOnComponent::HandleCandidateEndPlay(AActor* EndingActor, EEndPlayReason::Type)
{
	AVampsCharacter& Fighter = *static_cast<AVampsCharacter*>(EndingActor);
	const TWeakObjectPtr<AVampsCharacter> EndingFighter(&Fighter);
	const int32 EndingIndex = Candidates.IndexOfByKey(EndingFighter);
	if (EndingIndex == INDEX_NONE)
	{
		return;
	}

	Candidates.RemoveAt(EndingIndex);
	Fighter.OnCombatAliveChanged.RemoveAll(this);
	Fighter.OnEndPlay.RemoveDynamic(this, &UVampsLockOnComponent::HandleCandidateEndPlay);

	if (CurrentTarget.HasSameIndexAndSerialNumber(EndingFighter))
	{
		CycleTargetStartingAt(EndingIndex);
	}
}

bool UVampsLockOnComponent::IsEligible(const AVampsCharacter* Candidate) const
{
	if (!Candidate || !Candidate->CombatState.bIsAlive)
	{
		return false;
	}

	const AVampsMatchGameMode* MatchMode = GetWorld()->GetAuthGameMode<AVampsMatchGameMode>();
	return !MatchMode || MatchMode->RoundPhase == EVampsMatchPhase::Active;
}
