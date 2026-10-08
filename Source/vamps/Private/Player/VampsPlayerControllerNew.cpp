#include "Player/VampsPlayerControllerNew.h"
#include "EnhancedInputSubsystems.h"

void AVampsPlayerControllerNew::BeginPlay()
{
	Super::BeginPlay();
}

void AVampsPlayerControllerNew::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultInputMappingContext, 0);
		}
	}
}
