#include "Player/VampsCombatTestPlayerController.h"
#include "Characters/VampsCharacter.h"
#include "Characters/VampsLockOnComponent.h"
#include "EnhancedInputComponent.h"
#include "Engine/World.h"
#include "GameModes/VampsCombatTestGameMode.h"
#include "InputAction.h"
#include "InputCoreTypes.h"
#include "Combat/VampsCombatLog.h"

void AVampsCombatTestPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent);
	if (!Input || !CycleAIModeAction || !ResetCombatAction)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: debug input bindings require EnhancedInputComponent, CycleAIModeAction and "
		            "ResetCombatAction; component='%s', cycle='%s', reset='%s'."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(InputComponent),
		       *GetPathNameSafe(CycleAIModeAction),
		       *GetPathNameSafe(ResetCombatAction));
		return;
	}
	if (CycleAIModeAction->ValueType != EInputActionValueType::Boolean ||
	    ResetCombatAction->ValueType != EInputActionValueType::Boolean)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: debug input actions must be Boolean; '%s' has type %d, '%s' has type %d."),
		       __FUNCTION__,
		       *GetPathName(),
		       *CycleAIModeAction->GetPathName(),
		       int32(CycleAIModeAction->ValueType),
		       *ResetCombatAction->GetPathName(),
		       int32(ResetCombatAction->ValueType));
		return;
	}
	Input->BindAction(CycleAIModeAction, ETriggerEvent::Started, this, &AVampsCombatTestPlayerController::CycleAIMode);
	Input->BindAction(ResetCombatAction, ETriggerEvent::Started, this, &AVampsCombatTestPlayerController::ResetCombat);
#if !UE_BUILD_SHIPPING
	Input->BindDebugKey(
	    FInputChord(EKeys::F7), IE_Pressed, this, &AVampsCombatTestPlayerController::ToggleLockOnDebug, false);
#endif
}

#if !UE_BUILD_SHIPPING
void AVampsCombatTestPlayerController::ToggleLockOnDebug(FKey, FInputActionValue)
{
	AVampsCharacter* ControlledFighter = Cast<AVampsCharacter>(GetPawn());
	if (!ControlledFighter || !ControlledFighter->LockOn)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': debug lock-on toggle requires a VampsCharacter pawn with LockOn; pawn='%s'."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(GetPawn()));
		return;
	}
	ControlledFighter->LockOn->ToggleDebugLockOn();
}
#endif

void AVampsCombatTestPlayerController::CycleAIMode()
{
	AVampsCombatTestGameMode* CombatGameMode = Cast<AVampsCombatTestGameMode>(GetWorld()->GetAuthGameMode());
	if (!CombatGameMode)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: cannot cycle AI mode; GameMode '%s' is not VampsCombatTestGameMode."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(GetWorld()->GetAuthGameMode()));
		return;
	}
	CombatGameMode->CycleAIMode();
}

void AVampsCombatTestPlayerController::ResetCombat()
{
	AVampsCombatTestGameMode* CombatGameMode = Cast<AVampsCombatTestGameMode>(GetWorld()->GetAuthGameMode());
	if (!CombatGameMode)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: cannot reset combat; GameMode '%s' is not VampsCombatTestGameMode."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(GetWorld()->GetAuthGameMode()));
		return;
	}
	CombatGameMode->ResetCombat();
}
