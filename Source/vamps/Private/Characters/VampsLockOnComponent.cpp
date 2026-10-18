#include "Characters/VampsLockOnComponent.h"
#include "Characters/VampsCharacter.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Combat/VampsCombatLog.h"

UVampsLockOnComponent::UVampsLockOnComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UVampsLockOnComponent::BeginPlay()
{
	Super::BeginPlay();
	if (!Cast<AVampsCharacter>(GetOwner()))
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: LockOn initialization cancelled; owner '%s' must be a VampsCharacter."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(GetOwner()));
		SetComponentTickEnabled(false);
		return;
	}
	ActorSpawnedHandle = GetWorld()->AddOnActorSpawnedHandler(
	    FOnActorSpawned::FDelegate::CreateUObject(this, &UVampsLockOnComponent::RegisterCandidate));
	for (TActorIterator<AVampsCharacter> FighterIterator(GetWorld()); FighterIterator; ++FighterIterator)
	{
		RegisterCandidate(*FighterIterator);
	}
	CycleTarget();
}

void UVampsLockOnComponent::EndPlay(const EEndPlayReason::Type Reason)
{
	GetWorld()->RemoveOnActorSpawnedHandler(ActorSpawnedHandle);
	Candidates.Reset();
	CurrentTarget.Reset();
	Super::EndPlay(Reason);
}

void UVampsLockOnComponent::TickComponent(float DeltaSeconds,
                                          ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaSeconds, TickType, ThisTickFunction);
	if (!IsEligible(CurrentTarget.Get()))
	{
		CycleTarget();
	}
	Candidates.RemoveAll(
	    [](const TWeakObjectPtr<AVampsCharacter>& Candidate)
	    {
		    return !Candidate.IsValid();
	    });
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
	for (int32 Offset = 1; Offset <= Candidates.Num(); ++Offset)
	{
		AVampsCharacter* Candidate = Candidates[(CurrentIndex + Offset) % Candidates.Num()].Get();
		if (IsEligible(Candidate))
		{
			CurrentTarget = Candidate;
			return;
		}
	}
	CurrentTarget.Reset();
}

void UVampsLockOnComponent::SelectTarget(AVampsCharacter* NewTarget)
{
	if (IsEligible(NewTarget))
	{
		Candidates.AddUnique(NewTarget);
		CurrentTarget = NewTarget;
	}
}

void UVampsLockOnComponent::RegisterCandidate(AActor* SpawnedActor)
{
	AVampsCharacter* Candidate = Cast<AVampsCharacter>(SpawnedActor);
	if (Candidate && Candidate != GetOwner())
	{
		Candidates.AddUnique(Candidate);
	}
}

bool UVampsLockOnComponent::IsEligible(const AVampsCharacter* Candidate) const
{
	return IsValid(Candidate) && Candidate != GetOwner() && Candidate->HasActorBegunPlay() &&
	       !Candidate->IsActorBeingDestroyed() && Candidate->CombatState.bIsAlive;
}
