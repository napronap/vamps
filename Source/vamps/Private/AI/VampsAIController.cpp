#include "AI/VampsAIController.h"
#include "Characters/VampsCharacter.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"

void AVampsAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AVampsCharacter* ControlledCharacter = CastChecked<AVampsCharacter>(InPawn);
	UCharacterMovementComponent* Movement = ControlledCharacter->GetCharacterMovement();
	bPreviousOrientRotationToMovement = Movement->bOrientRotationToMovement;
	bPreviousUseControllerDesiredRotation = Movement->bUseControllerDesiredRotation;
	Movement->bOrientRotationToMovement = false;
	Movement->bUseControllerDesiredRotation = true;
	check(AttackDistance > 0.f && AttackInterval > 0.f && MixedModeDuration > 0.f);
	SetMode(Mode);
}

void AVampsAIController::OnUnPossess()
{
	GetWorldTimerManager().ClearTimer(DecisionTimer);
	StopActions();
	if (AVampsCharacter* ControlledCharacter = Cast<AVampsCharacter>(GetPawn()))
	{
		UCharacterMovementComponent* Movement = ControlledCharacter->GetCharacterMovement();
		Movement->bOrientRotationToMovement = bPreviousOrientRotationToMovement;
		Movement->bUseControllerDesiredRotation = bPreviousUseControllerDesiredRotation;
	}
	Super::OnUnPossess();
}

void AVampsAIController::EndPlay(const EEndPlayReason::Type Reason)
{
	GetWorldTimerManager().ClearTimer(DecisionTimer);
	StopActions();
	Super::EndPlay(Reason);
}

void AVampsAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (APawn* ControlledPawn = GetPawn())
	{
		ControlledPawn->AddMovementInput(MoveDirection);
	}
}

void AVampsAIController::SetMode(EVampsAIMode NewMode)
{
	check(NewMode <= EVampsAIMode::Mixed);
	GetWorldTimerManager().ClearTimer(DecisionTimer);
	StopActions();
	Mode = NewMode;
	ActiveBehavior = EVampsAIMode::Passive;
	BehaviorStartedAt = GetWorld()->GetTimeSeconds() + 0.75;
	NextAttackAt = BehaviorStartedAt;
	if (Mode != EVampsAIMode::Passive)
	{
		GetWorldTimerManager().SetTimer(DecisionTimer, this, &AVampsAIController::Decide, 0.1f, true, 0.75f);
	}
}

void AVampsAIController::StopActions()
{
	MoveDirection = FVector::ZeroVector;
	if (AVampsCharacter* ControlledCharacter = Cast<AVampsCharacter>(GetPawn()))
	{
		ControlledCharacter->ResetCombatState();
		ControlledCharacter->ConsumeMovementInputVector();
		ControlledCharacter->GetCharacterMovement()->StopMovementImmediately();
	}
}

void AVampsAIController::Decide()
{
	MoveDirection = FVector::ZeroVector;
	AActor* FocusActor = GetFocusActor();
	if (!IsValid(FocusActor))
	{
		StopActions();
		return;
	}

	AVampsCharacter* ControlledCharacter = CastChecked<AVampsCharacter>(GetPawn());
	const double Now = GetWorld()->GetTimeSeconds();
	EVampsAIMode Behavior = Mode;
	if (Mode == EVampsAIMode::Mixed)
	{
		Behavior = EVampsAIMode(1 + (int32((Now - BehaviorStartedAt) / MixedModeDuration) % 3));
	}
	if (Behavior != ActiveBehavior)
	{
		StopActions();
		ActiveBehavior = Behavior;
	}

	const FVector ToTarget = FocusActor->GetActorLocation() - ControlledCharacter->GetActorLocation();
	switch (Behavior)
	{
	case EVampsAIMode::Passive:
		return;
	case EVampsAIMode::Attack:
		if (!ControlledCharacter->CombatState.bCanStartAction)
		{
			return;
		}
		if (ToTarget.SizeSquared2D() > FMath::Square(AttackDistance))
		{
			MoveDirection = ToTarget.GetSafeNormal2D();
			return;
		}
		ControlledCharacter->ConsumeMovementInputVector();
		ControlledCharacter->GetCharacterMovement()->StopMovementImmediately();
		if (Now >= NextAttackAt)
		{
			ControlledCharacter->RequestCombatAction();
			NextAttackAt = Now + AttackInterval;
		}
		return;
	case EVampsAIMode::Guard:
		if (!ControlledCharacter->CombatState.bCanDefend)
		{
			ControlledCharacter->RequestCombatAction();
		}
		return;
	case EVampsAIMode::Sidestep:
		MoveDirection = FVector::CrossProduct(FVector::UpVector, ToTarget.GetSafeNormal2D());
		return;
	default:
		checkNoEntry();
	}
}
