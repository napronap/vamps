#pragma once

#include "Player/VampsPlayerController.h"
#include "VampsMatchPlayerController.generated.h"

class UInputAction;

UCLASS()
class VAMPS_API AVampsMatchPlayerController : public AVampsPlayerController
{
	GENERATED_BODY()

public:

	FSimpleMulticastDelegate OnPauseRequested;

protected:

	virtual void SetupInputComponent() override;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> PauseAction;

	void RequestPause();
};
