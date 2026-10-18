#include "AI/VampsAIController.h"
#include "Animation/AnimMontage.h"
#include "Characters/VampsCharacter.h"
#include "Combat/Data/VampsCombatSettings.h"
#include "Combat/VampsCombatLog.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"

void AVampsAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AVampsCharacter* ControlledCharacter = Cast<AVampsCharacter>(InPawn);
	if (!ControlledCharacter || !ControlledCharacter->GetCharacterMovement())
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: cannot initialize AI; pawn '%s' must be a VampsCharacter with a movement component."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(InPawn));
		return;
	}
	UCharacterMovementComponent* Movement = ControlledCharacter->GetCharacterMovement();
	bPreviousOrientRotationToMovement = Movement->bOrientRotationToMovement;
	bPreviousUseControllerDesiredRotation = Movement->bUseControllerDesiredRotation;
	Movement->bOrientRotationToMovement = false;
	Movement->bUseControllerDesiredRotation = true;
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
	if (NewMode > EVampsAIMode::Mixed || !FMath::IsFinite(AttackDistance) || !FMath::IsFinite(AttackInterval) ||
	    !FMath::IsFinite(MixedModeDuration) || AttackDistance <= 0.f || AttackInterval <= 0.f ||
	    MixedModeDuration <= 0.f)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: AI mode change rejected; mode=%d, AttackDistance=%f, AttackInterval=%f, "
		            "MixedModeDuration=%f. Expected mode 0..4 and finite positive settings."),
		       __FUNCTION__,
		       *GetPathName(),
		       int32(NewMode),
		       AttackDistance,
		       AttackInterval,
		       MixedModeDuration);
		GetWorldTimerManager().ClearTimer(DecisionTimer);
		MoveDirection = FVector::ZeroVector;
		return;
	}
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

	AVampsCharacter* ControlledCharacter = Cast<AVampsCharacter>(GetPawn());
	if (!IsValid(ControlledCharacter))
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: AI decision cancelled; pawn '%s' is not a valid VampsCharacter."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(GetPawn()));
		GetWorldTimerManager().ClearTimer(DecisionTimer);
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
		StopActions();
		ActiveBehavior = Behavior;
	}

	const FVector ToTarget = FocusActor->GetActorLocation() - ControlledCharacter->GetActorLocation();
	switch (Behavior)
	{
	case EVampsAIMode::Passive:
		return;
	case EVampsAIMode::Attack:
		if (!ControlledCharacter->CombatSettings || !ControlledCharacter->CombatSettings->AttackMontage)
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: %s: AI attack cancelled; '%s' requires CombatSettings and AttackMontage."),
			       __FUNCTION__,
			       *GetPathName(),
			       *ControlledCharacter->GetPathName());
			GetWorldTimerManager().ClearTimer(DecisionTimer);
			return;
		}
		if (ControlledCharacter->CombatState.bComboWindowOpen && ControlledCharacter->CombatState.bActionConnected &&
		    !ControlledCharacter->CombatState.bOffenseRestricted &&
		    !ControlledCharacter->CombatState.bContinuationRequested &&
		    ControlledCharacter->CombatState.ComboStep <
		        ControlledCharacter->CombatSettings->AttackMontage->GetNumSections())
		{
			ControlledCharacter->RequestCombatAction();
			return;
		}
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
		if (ControlledCharacter->CombatState.bCanRequestCombat &&
		    !ControlledCharacter->CombatState.bDefenseRequestArmed)
		{
			ControlledCharacter->RequestCombatAction();
		}
		return;
	case EVampsAIMode::Sidestep:
		MoveDirection = FVector::CrossProduct(FVector::UpVector, ToTarget.GetSafeNormal2D());
		return;
	default:
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: AI decision cancelled; unsupported behavior %d."),
		       __FUNCTION__,
		       *GetPathName(),
		       int32(Behavior));
		GetWorldTimerManager().ClearTimer(DecisionTimer);
		return;
	}
}
