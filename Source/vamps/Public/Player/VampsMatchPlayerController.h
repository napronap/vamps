#pragma once

#include "Player/VampsPlayerController.h"
#include "VampsMatchPlayerController.generated.h"

class ACameraActor;

UCLASS()
class VAMPS_API AVampsMatchPlayerController : public AVampsPlayerController
{
	GENERATED_BODY()

public:

	virtual void PlayerTick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
	void BeginRespawnCamera();
	void FinishRespawnCamera();

private:

	UPROPERTY(Transient)
	TObjectPtr<ACameraActor> RespawnCamera;
};
