#pragma once

#include "Components/ActorComponent.h"
#include "VampsLockOnComponent.generated.h"

class AVampsCharacter;

UCLASS()
class VAMPS_API UVampsLockOnComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UVampsLockOnComponent();

	AVampsCharacter* GetCurrentTarget() const;
	const TArray<TWeakObjectPtr<AVampsCharacter>>& GetCandidates() const;
	void CycleTarget();
	void SelectTarget(AVampsCharacter* NewTarget);

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
	virtual void TickComponent(float DeltaSeconds,
	                           ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:

	UPROPERTY(VisibleInstanceOnly, Category = "Lock On")
	TWeakObjectPtr<AVampsCharacter> CurrentTarget;

	TArray<TWeakObjectPtr<AVampsCharacter>> Candidates;
	FDelegateHandle ActorSpawnedHandle;

	void RegisterCandidate(AActor* SpawnedActor);
	bool IsEligible(const AVampsCharacter* Candidate) const;
};
