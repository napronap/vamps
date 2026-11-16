#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "VampsCombatMovementComponent.generated.h"

struct FVampsCombatStepSettings;

UENUM(BlueprintType)
enum class EVampsCombatMotion : uint8
{
	None,
	Alignment,
	Knockback,
	Advance
};

// Extends the fighter's normal character movement with sprint speed and combat displacement
// The component handles attack advances, hit alignment and knockback 
// dashes use the root motion in their montages
UCLASS()
class VAMPS_API UVampsCombatMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:

	virtual void TickComponent(float DeltaTime,
	                           ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	// Uses the configured sprint speed while sprinting and the movement mode's speed otherwise
	virtual float GetMaxSpeed() const override;

	// Jumps forward towards the locked target when the first attack step begins
	void BeginOffensiveAdvance();

	// Cancels the attack advance and clears its horizontal velocity
	void StopVoluntaryMotion();

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	EVampsCombatMotion CombatMotion = EVampsCombatMotion::None;

	// When hitting an opponent, uses the resolved hit to align and push the recipient
	// targets should always be aligned in front to make it easier to continue combo chains
	// A block goes straight to knockback
	void ReceiveCombatDisplacement(const FVector& AttackOrigin,
	                               const FVector& AttackDirection,
	                               const FVampsCombatStepSettings& StepSettings,
	                               bool bBlocked);

	void ResetCombatMotion();

protected:

	// discards root motion when combat controls the displacement
	virtual void TickCharacterPose(float DeltaTime) override;

	// Converts the current combat move's progress into horizontal velocity for Unreal to apply
	virtual void ApplyRootMotionToVelocity(float DeltaTime) override;

	// Turns toward the target during advance and against the push during hit displacement
	virtual void PhysicsRotation(float DeltaTime) override;

	// Finishes a combat move after it hits an obstacle or its duration runs out so that the character does not remain stuck
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

	// Records a blocking collision so the combat move ends after the movement update
	virtual void HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta) override;

	// Suppresses sliding along a surface during an advance, alignment or knockback
	virtual float SlideAlongSurface(
	    const FVector& Delta, float Time, const FVector& Normal, FHitResult& Hit, bool bHandleImpact) override;

private:

	FVector CombatMotionStart = FVector::ZeroVector;
	FVector CombatMotionGoal = FVector::ZeroVector;
	FVector CombatPushDirection = FVector::ZeroVector;
	FQuat CombatFacingStart = FQuat::Identity;
	FQuat CombatFacingGoal = FQuat::Identity;
	float CombatMotionElapsed = 0.f;
	float CombatMotionDuration = 0.f;
	float PendingKnockbackDistance = 0.f;
	bool bCombatMotionBlocked = false;

	// Records the start, destination and duration for a new combat move
	void BeginCombatMotion(EVampsCombatMotion NewMotion, const FVector& Destination, float Duration);

	// Starts the pending push from the position the recipient has reached
	void BeginCombatKnockback();

	// Starts knockback after alignment or stops the current move and clears horizontal velocity
	void FinishCombatMotion();
};
