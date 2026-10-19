#include "Characters/VampsCharacter_Player.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

AVampsCharacter_Player::AVampsCharacter_Player()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->TargetOffset = FVector(0.f, 0.f, 60.f);
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->SetComponentTickEnabled(false);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	FollowCamera->SetAutoActivate(false);
}
