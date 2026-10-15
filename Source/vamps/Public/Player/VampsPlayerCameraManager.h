#pragma once

#include "Camera/PlayerCameraManager.h"
#include "VampsPlayerCameraManager.generated.h"

UCLASS()
class VAMPS_API AVampsPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:

	virtual void ProcessViewRotation(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Lock On Camera", meta = (ClampMin = "-80.0", ClampMax = "0.0"))
	float FollowPitch = -15.f;

	UPROPERTY(EditDefaultsOnly, Category = "Lock On Camera", meta = (ClampMin = "0.1"))
	float TrackingSpeed = 6.f;

	UPROPERTY(EditDefaultsOnly, Category = "Lock On Camera", meta = (ClampMin = "0.0", ClampMax = "45.0"))
	float HorizontalTolerance = 6.f;
};
