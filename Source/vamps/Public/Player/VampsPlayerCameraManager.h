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

	// Observe the controller's current pawn and listen for possession changes.
	virtual void InitializeFor(APlayerController* OwningController) override;

	// Update the camera and sync the local controller to its rotation for camera-relative movement.
	virtual void UpdateCamera(float DeltaTime) override;

	// Discard look input and use the last camera rotation once the custom view is initialized.
	virtual void ProcessViewRotation(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot) override;

	// Unbind the controller and fighter delegates and restore any faded geometry.
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;

protected:

	// Hold the last view while the local controller still possesses the dead fighter it is viewing.
	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;

	// Build the local fighter's view from movement or lock-on, then apply framing, collision, and occluder fading.
	virtual void UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime) override;

private:

	// Default follow framing. Heights offset the camera and its look-at point vertically.
	static constexpr float FollowDistance = 600.f;
	static constexpr float FollowEyeHeight = 70.f;
	static constexpr float FollowInterestHeight = 30.f;
	static constexpr float FollowVerticalFov = 38.f;

	// Close-up framing during an attack or block. Shift the pivot toward the subject and orbit to the side.
	static constexpr float CombatDistance = 425.f;
	static constexpr float CombatEyeHeight = 30.f;
	static constexpr float CombatInterestHeight = -20.f;
	static constexpr float CombatVerticalFov = 32.4f;
	static constexpr float CombatInteriorDistance = 95.f;
	static constexpr float CombatYawOffset = 20.f;

	// Position smoothing radius, lock-on yaw response, and blend time when the framing or target changes.
	static constexpr float PositionRadius = 180.f;
	static constexpr float AngularSpeed = 6.f;
	static constexpr float TransitionDuration = 0.3f;

	// Smoothed world-space camera and look-at positions before the collision adjustment.
	UPROPERTY(VisibleInstanceOnly, Transient, Category = "Camera")
	FVector EyePosition = FVector::ZeroVector;

	UPROPERTY(VisibleInstanceOnly, Transient, Category = "Camera")
	FVector InterestPosition = FVector::ZeroVector;

	// Whether combat framing is active and which fighter's attack montage drives it.
	UPROPERTY(VisibleInstanceOnly, Transient, Category = "Camera")
	bool bCombatFraming = false;

	UPROPERTY(VisibleInstanceOnly, Transient, Category = "Camera")
	TWeakObjectPtr<AVampsCharacter> FramingSource;

	// The possessed fighter and the lock-on target used in the last camera update.
	TWeakObjectPtr<AVampsCharacter> ObservedFighter;
	TWeakObjectPtr<AVampsCharacter> ObservedTarget;

	// Keep the framing tied to one montage playback, even when the same montage plays again.
	int32 FramingMontageID = INDEX_NONE;

	// Initialization flag, smoothed orbit and free-follow headings, and current vertical field of view.
	bool bHasCameraState = false;
	double OrbitalYaw = 0;
	double UnlockedHeadingYaw = 0;
	float VerticalFov = FollowVerticalFov;

	// Elapsed blend time and the view saved when the lock-on target or framing changes.
	float TransitionElapsed = 0;
	FVector TransitionEye = FVector::ZeroVector;
	FVector TransitionInterest = FVector::ZeroVector;
	float TransitionFov = FollowVerticalFov;

	// Original material fade values for occluders that are fading out or being restored.
	TMap<TWeakObjectPtr<UPrimitiveComponent>, float> FadedComponents;

	// Smooth a world-space position toward its goal with a faster response at greater distances.
	static FVector SpatialFollow(const FVector& PreviousPosition, const FVector& DesiredPosition, float DeltaTime);

	// On possession changes, reset the view and move the combat subscriptions to the new fighter.
	UFUNCTION()
	void HandleObservedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	// On attack or confirmed block, record the attacker and the montage playback that drives the framing.
	void HandleCombatFramingRequested(AVampsCharacter* ChainSource);

	// Restore faded geometry and clear the target and framing so the next update initializes a fresh view.
	void ResetCameraState();

	// Fade tagged geometry between the camera and the fighters, then restore it when the view clears.
	void UpdateOccluders(const FVector& PublishedEye,
	                     const AVampsCharacter& ViewingFighter,
	                     const AVampsCharacter* LockedFighter,
	                     float DeltaTime);
};
