#pragma once

#include "Components/ActorComponent.h"
#include "VampsLockOnComponent.generated.h"

class AVampsCharacter;

DECLARE_MULTICAST_DELEGATE_OneParam(FVampsTargetChanged, AVampsCharacter*);

UCLASS()
class VAMPS_API UVampsLockOnComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	// called after the owner begins play to register other fighters. In a match, only opposing teams are registered
	void InitializeCandidates();

	AVampsCharacter* GetCurrentTarget() const;
	const TArray<TWeakObjectPtr<AVampsCharacter>>& GetCandidates() const;

	// cycles through candidates in registration order. Clears the target if none is eligible
	void CycleTarget();

	// selects a specific fighter if eligible. Used to target the attacker after a confirmed block
	void SelectTarget(AVampsCharacter* NewTarget);
	FVampsTargetChanged OnTargetChanged;

protected:

	virtual void EndPlay(const EEndPlayReason::Type Reason) override;

private:

	UPROPERTY(VisibleInstanceOnly, Category = "Lock On")
	TWeakObjectPtr<AVampsCharacter> CurrentTarget;

	TArray<TWeakObjectPtr<AVampsCharacter>> Candidates;

	// adds a fighter to the cycle and listens for changes to its eligibility
	void RegisterCandidate(AVampsCharacter& Candidate);

	UFUNCTION()
	void HandleCandidateEndPlay(AActor* EndingActor, EEndPlayReason::Type Reason);

	// runs when fighters enter play, die or respawn, or when the round phase changes
	// InitializeCandidates calls it directly
	// bound to OnCombatAliveChanged and OnRoundPhaseChanged
	// UVampsHUDWidget listens to OnTargetChanged. camera and AI read GetCurrentTarget
	void RefreshTarget();

	void CycleTargetStartingAt(int32 FirstCandidateIndex);

	// Stores the chosen target and notifies listeners when it changes
	void SetCurrentTarget(AVampsCharacter* NewTarget);
	bool IsEligible(const AVampsCharacter* Candidate) const;
};
