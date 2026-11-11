#include "Player/VampsCombatTestPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Engine/World.h"
#include "GameModes/VampsCombatTestGameMode.h"
#include "InputAction.h"
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
}

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
