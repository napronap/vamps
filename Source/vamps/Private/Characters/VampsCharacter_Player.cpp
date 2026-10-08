
#include "Characters/VampsCharacter_Player.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

AVampsCharacter_Player::AVampsCharacter_Player()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}

void AVampsCharacter_Player::DoLook()
{
	// do the looking
}
