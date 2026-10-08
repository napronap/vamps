#pragma once

#include "CoreMinimal.h"
#include "VampsCharacter.h"
#include "VampsCharacter_Player.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS(abstract)
class AVampsCharacter_Player : public AVampsCharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

public:
	AVampsCharacter_Player();

	USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION()
	void DoLook();
};
