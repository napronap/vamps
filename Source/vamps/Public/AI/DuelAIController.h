#pragma once

#include "AIController.h"
#include "TimerManager.h"
#include "DuelAIController.generated.h"

class AVampsDuelCharacter;

UENUM(BlueprintType)
enum class EDuelAIMode : uint8
{
	Passive,
	Attack,
	Guard,
	Sidestep,
	Mixed
};

UCLASS()
class VAMPS_API ADuelAIController : public AAIController
{
	GENERATED_BODY()

public:

	ADuelAIController();
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Duel")
	EDuelAIMode Mode = EDuelAIMode::Passive;

	UPROPERTY(EditDefaultsOnly, Category = "Duel", meta = (ClampMin = "70"))
	float AttackDistance = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Duel", meta = (ClampMin = "0.1"))
	float AttackInterval = 1.f;

	void CycleMode();
	void ResetBehavior();

private:

	FTimerHandle DecisionTimer;
	FVector MoveDirection = FVector::ZeroVector;
	double StartedAt = 0;
	double NextAttackAt = 0;

	void Decide();
	void ApproachAndAttack(AVampsDuelCharacter* Fighter, const FVector& ToTarget);
	void HoldGuard(AVampsDuelCharacter* Fighter);
	void Sidestep(AVampsDuelCharacter* Fighter, const FVector& ToTarget);
	void StopActions();
};
