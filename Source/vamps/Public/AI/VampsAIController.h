#pragma once

#include "AIController.h"
#include "TimerManager.h"
#include "VampsAIController.generated.h"

UENUM(BlueprintType)
enum class EVampsAIMode : uint8
{
	Passive,
	Attack,
	Guard,
	Sidestep,
	Mixed
};

UCLASS()
class VAMPS_API AVampsAIController : public AAIController
{
	GENERATED_BODY()

public:

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	EVampsAIMode Mode = EVampsAIMode::Passive;

	UPROPERTY(EditDefaultsOnly, Category = "AI", meta = (ClampMin = "1"))
	float AttackDistance = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI", meta = (ClampMin = "0.1"))
	float AttackInterval = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI", meta = (ClampMin = "0.1"))
	float MixedModeDuration = 4.f;

	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetMode(EVampsAIMode NewMode);

private:

	FTimerHandle DecisionTimer;
	FVector MoveDirection = FVector::ZeroVector;
	EVampsAIMode ActiveBehavior = EVampsAIMode::Passive;
	double BehaviorStartedAt = 0;
	double NextAttackAt = 0;
	bool bPreviousOrientRotationToMovement = false;
	bool bPreviousUseControllerDesiredRotation = false;

	void Decide();
	void StopActions();
};
