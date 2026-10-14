#include "Player/VampsCombatTestPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Engine/World.h"
#include "GameModes/VampsCombatTestGameMode.h"
#include "InputAction.h"

void AVampsCombatTestPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	checkf(CycleAIModeAction && ResetCombatAction, TEXT("Both combat test input actions must be assigned."));
	check(CycleAIModeAction->ValueType == EInputActionValueType::Boolean);
	check(ResetCombatAction->ValueType == EInputActionValueType::Boolean);
	UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(InputComponent);
	Input->BindAction(CycleAIModeAction, ETriggerEvent::Started, this, &AVampsCombatTestPlayerController::CycleAIMode);
	Input->BindAction(ResetCombatAction, ETriggerEvent::Started, this, &AVampsCombatTestPlayerController::ResetCombat);
}

void AVampsCombatTestPlayerController::CycleAIMode()
{
	CastChecked<AVampsCombatTestGameMode>(GetWorld()->GetAuthGameMode())->CycleAIMode();
}

void AVampsCombatTestPlayerController::ResetCombat()
{
	CastChecked<AVampsCombatTestGameMode>(GetWorld()->GetAuthGameMode())->ResetCombat();
}
