#include "AI/VampsAIController.h"
#include "Characters/VampsCharacter.h"
#include "Characters/VampsLockOnComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameModes/VampsMatchGameMode.h"

void AVampsAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UCharacterMovementComponent* Movement = GetCharacter()->GetCharacterMovement();

	Movement->bOrientRotationToMovement = false;
	Movement->bUseControllerDesiredRotation = true;

	SetMode(Mode);
}

void AVampsAIController::OnUnPossess()
{
	GetWorldTimerManager().ClearTimer(DecisionTimer);
	MoveDirection = FVector::ZeroVector;
	GetPawn()->ConsumeMovementInputVector();

	Super::OnUnPossess();
}

void AVampsAIController::EndPlay(const EEndPlayReason::Type Reason)
{
	GetWorldTimerManager().ClearTimer(DecisionTimer);
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
	GetWorldTimerManager().ClearTimer(DecisionTimer);
	MoveDirection = FVector::ZeroVector;

	if (NewMode > EVampsAIMode::Mixed || !FMath::IsFinite(AttackDistance) || !FMath::IsFinite(AttackInterval) ||
	    !FMath::IsFinite(MixedModeDuration) || AttackDistance <= 0.f || AttackInterval <= 0.f ||
	    MixedModeDuration <= 0.f)
	{
		return;
	}

	Mode = NewMode;
	ActiveBehavior = EVampsAIMode::Passive;
	BehaviorStartedAt = GetWorld()->GetTimeSeconds() + 0.75;
	NextAttackAt = BehaviorStartedAt;

	if (AVampsCharacter* ControlledFighter = GetPawn<AVampsCharacter>())
	{
		ResetFighterActions(*ControlledFighter);

		if (Mode != EVampsAIMode::Passive)
		{
			GetWorldTimerManager().SetTimer(DecisionTimer, this, &AVampsAIController::Decide, 0.1f, true, 0.75f);
		}
	}
}

void AVampsAIController::ResetFighterActions(AVampsCharacter& ControlledFighter)
{
	ControlledFighter.ResetCombatState();
	ControlledFighter.ConsumeMovementInputVector();
}

void AVampsAIController::Decide()
{
	MoveDirection = FVector::ZeroVector;
	AVampsCharacter& ControlledFighter = *GetPawn<AVampsCharacter>();
	const FVampsCombatState& FighterState = ControlledFighter.CombatState;

	if (!FighterState.bIsAlive)
	{
		return;
	}

	const bool bInMatch = GetWorld()->GetAuthGameMode<AVampsMatchGameMode>() != nullptr;
	AActor* TargetActor = bInMatch ? ControlledFighter.LockOn->GetCurrentTarget() : GetFocusActor();

	if (bInMatch)
	{
		SetFocus(TargetActor);
	}

	if (!TargetActor)
	{
		return;
	}

	const double Now = GetWorld()->GetTimeSeconds();
	EVampsAIMode Behavior = Mode;

	if (Mode == EVampsAIMode::Mixed)
	{
		Behavior = EVampsAIMode(1 + (int32((Now - BehaviorStartedAt) / MixedModeDuration) % 3));
	}

	if (Behavior != ActiveBehavior)
	{
		ResetFighterActions(ControlledFighter);
		ActiveBehavior = Behavior;
	}

	const FVector ToTarget = TargetActor->GetActorLocation() - ControlledFighter.GetActorLocation();

	switch (Behavior)
	{
	case EVampsAIMode::Passive:
	case EVampsAIMode::Mixed:
		return;
	case EVampsAIMode::Attack:
		if (FighterState.bActionConnected && !FighterState.bContinuationRequested &&
		    ControlledFighter.CanRequestCombatContinuation())
		{
			ControlledFighter.RequestCombatAction();
			return;
		}

		if (!FighterState.bCanStartAction)
		{
			return;
		}

		if (ToTarget.SizeSquared2D() > FMath::Square(AttackDistance))
		{
			MoveDirection = ToTarget.GetSafeNormal2D();
			return;
		}

		ControlledFighter.ConsumeMovementInputVector();
		ControlledFighter.GetCharacterMovement()->StopMovementImmediately();

		if (Now >= NextAttackAt)
		{
			ControlledFighter.RequestCombatAction();
			NextAttackAt = Now + AttackInterval;
		}

		return;
	case EVampsAIMode::Guard:
		if (FighterState.bCanRequestCombat && !FighterState.bDefenseRequestArmed)
		{
			ControlledFighter.RequestCombatAction();
		}

		return;
	case EVampsAIMode::Sidestep:
		MoveDirection = FVector::CrossProduct(FVector::UpVector, ToTarget.GetSafeNormal2D());

		return;
	}
}
