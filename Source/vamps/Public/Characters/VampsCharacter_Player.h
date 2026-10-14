#pragma once

#include "Characters/VampsCharacter.h"
#include "VampsCharacter_Player.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS(Abstract)
class VAMPS_API AVampsCharacter_Player : public AVampsCharacter
{
	GENERATED_BODY()

public:

	AVampsCharacter_Player();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;
};
