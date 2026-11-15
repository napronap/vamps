#include "Player/VampsMatchPlayerController.h"

#include "Combat/VampsCombatLog.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"

void AVampsMatchPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent);
	if (!Input || !PauseAction || PauseAction->ValueType != EInputActionValueType::Boolean)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s' requires Enhanced Input and a Boolean PauseAction. Action='%s'."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(PauseAction));
		return;
	}
	Input->BindAction(PauseAction, ETriggerEvent::Started, this, &AVampsMatchPlayerController::RequestPause);
}

void AVampsMatchPlayerController::RequestPause()
{
	OnPauseRequested.Broadcast();
}
