#include "AI/DuelAIController.h"
#include "Characters/VampsDuelCharacter.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"

ADuelAIController::ADuelAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ADuelAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ResetBehavior();
}

void ADuelAIController::OnUnPossess()
{
	GetWorldTimerManager().ClearTimer(DecisionTimer);
	StopActions();
	Super::OnUnPossess();
}

void ADuelAIController::EndPlay(const EEndPlayReason::Type Reason)
{
	GetWorldTimerManager().ClearTimer(DecisionTimer);
	StopActions();
	Super::EndPlay(Reason);
}

void ADuelAIController::ResetBehavior()
{
	StopActions();
	StartedAt = GetWorld()->GetTimeSeconds() + 0.75;
	NextAttackAt = StartedAt;
	GetWorldTimerManager().SetTimer(DecisionTimer, this, &ADuelAIController::Decide, 0.1f, true, 0.75f);
}

void ADuelAIController::CycleMode()
{
	Mode = EDuelAIMode((int32(Mode) + 1) % 5);
	ResetBehavior();
}

void ADuelAIController::StopActions()
{
	MoveDirection = FVector::ZeroVector;
	SetActorTickEnabled(false);
	if (AVampsDuelCharacter* Fighter = Cast<AVampsDuelCharacter>(GetPawn()))
	{
		Fighter->RequestGuard(false);
		Fighter->CancelAttack();
		Fighter->ConsumeMovementInputVector();
		Fighter->GetCharacterMovement()->StopMovementImmediately();
	}
}

void ADuelAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	// CharacterMovement consumes input each frame; decisions run in Decide.
	if (AVampsDuelCharacter* Fighter = Cast<AVampsDuelCharacter>(GetPawn()))
	{
		Fighter->RequestMove(MoveDirection);
	}
}

void ADuelAIController::Decide()
{
	MoveDirection = FVector::ZeroVector;
	AVampsDuelCharacter* Fighter = Cast<AVampsDuelCharacter>(GetPawn());
	AVampsDuelCharacter* Target = Fighter ? Fighter->GetTarget() : nullptr;
	if (!Target)
	{
		StopActions();
		return;
	}

	EDuelAIMode Behavior = Mode;
	if (Mode == EDuelAIMode::Mixed)
	{
		const double Elapsed = GetWorld()->GetTimeSeconds() - StartedAt;
		Behavior = EDuelAIMode(1 + (int32(Elapsed / 4.) % 3));
	}

	const FVector ToTarget = Target->GetActorLocation() - Fighter->GetActorLocation();
	switch (Behavior)
	{
	case EDuelAIMode::Attack:
		ApproachAndAttack(Fighter, ToTarget);
		break;
	case EDuelAIMode::Guard:
		HoldGuard(Fighter);
		break;
	case EDuelAIMode::Sidestep:
		Sidestep(Fighter, ToTarget);
		break;
	default:
		StopActions();
		break;
	}
	SetActorTickEnabled(!MoveDirection.IsNearlyZero());
}

void ADuelAIController::ApproachAndAttack(AVampsDuelCharacter* Fighter, const FVector& ToTarget)
{
	Fighter->RequestGuard(false);
	if (Fighter->IsAttacking())
	{
		return;
	}
	if (ToTarget.SizeSquared2D() > FMath::Square(AttackDistance))
	{
		MoveDirection = ToTarget.GetSafeNormal2D();
		return;
	}

	Fighter->ConsumeMovementInputVector();
	Fighter->GetCharacterMovement()->StopMovementImmediately();
	const double Now = GetWorld()->GetTimeSeconds();
	if (Now >= NextAttackAt)
	{
		Fighter->RequestAttack();
		NextAttackAt = Now + FMath::Max(0.1f, AttackInterval);
	}
}

void ADuelAIController::HoldGuard(AVampsDuelCharacter* Fighter)
{
	Fighter->ConsumeMovementInputVector();
	Fighter->GetCharacterMovement()->StopMovementImmediately();
	if (Fighter->IsAttacking())
	{
		Fighter->CancelAttack();
	}
	if (!Fighter->IsGuarding())
	{
		Fighter->RequestGuard(true);
	}
}

void ADuelAIController::Sidestep(AVampsDuelCharacter* Fighter, const FVector& ToTarget)
{
	Fighter->RequestGuard(false);
	if (Fighter->IsAttacking())
	{
		Fighter->CancelAttack();
	}
	MoveDirection = FVector::CrossProduct(FVector::UpVector, ToTarget.GetSafeNormal2D());
}
