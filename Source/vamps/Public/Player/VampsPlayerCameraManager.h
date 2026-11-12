#pragma once

#include "Camera/PlayerCameraManager.h"
#include "VampsPlayerCameraManager.generated.h"

class APawn;
class AVampsCharacter;
class UPrimitiveComponent;

UCLASS()
class VAMPS_API AVampsPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:

	virtual void InitializeFor(APlayerController* OwningController) override;
	virtual void UpdateCamera(float DeltaTime) override;
	virtual void ProcessViewRotation(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot) override;
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;

protected:

	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;
	virtual void UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime) override;

private:

	UPROPERTY(VisibleInstanceOnly, Transient, Category = "Camera")
	FVector EyePosition = FVector::ZeroVector;

	UPROPERTY(VisibleInstanceOnly, Transient, Category = "Camera")
	FVector InterestPosition = FVector::ZeroVector;

	UPROPERTY(VisibleInstanceOnly, Transient, Category = "Camera")
	bool bCombatFraming = false;

	UPROPERTY(VisibleInstanceOnly, Transient, Category = "Camera")
	TWeakObjectPtr<AVampsCharacter> FramingSource;

	TWeakObjectPtr<AVampsCharacter> ObservedFighter;
	TWeakObjectPtr<AVampsCharacter> ObservedTarget;
	int32 FramingMontageID = INDEX_NONE;
	bool bHasCameraState = false;
	double OrbitalYaw = 0;
	double UnlockedHeadingYaw = 0;
	float VerticalFov = 38.f;
	float TransitionElapsed = 0;
	FVector TransitionEye = FVector::ZeroVector;
	FVector TransitionInterest = FVector::ZeroVector;
	float TransitionFov = 38.f;
	TMap<TWeakObjectPtr<UPrimitiveComponent>, float> FadedComponents;

	static FVector SpatialFollow(const FVector& PreviousPosition, const FVector& DesiredPosition, float DeltaTime);

	UFUNCTION()
	void HandleObservedPawnChanged(APawn* OldPawn, APawn* NewPawn);
	void HandleCombatFramingRequested(AVampsCharacter* ChainSource);
	void ResetCameraState();
	void UpdateOccluders(const FVector& PublishedEye,
	                     const AVampsCharacter& ViewingFighter,
	                     const AVampsCharacter* LockedFighter,
	                     float DeltaTime);
};
