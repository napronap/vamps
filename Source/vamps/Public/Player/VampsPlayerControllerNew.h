#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VampsPlayerControllerNew.generated.h"

class UInputMappingContext;

UCLASS()
class AVampsPlayerControllerNew : public APlayerController
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere)
	UInputMappingContext* DefaultInputMappingContext;

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
};
