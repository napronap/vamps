#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "VampsCombatMovementComponent.generated.h"

class UAnimMontage;
struct FVampsCombatStepSettings;

UENUM(BlueprintType)
enum class EVampsCombatMotion : uint8
{
	None,
	Alignment,
	Knockback
};

UCLASS()
class VAMPS_API UVampsCombatMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
	EVampsCombatMotion CombatMotion = EVampsCombatMotion::None;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
	bool bCombatMotionBlocked = false;

	void ReceiveCombatDisplacement(const FVector& AttackOrigin,
	                               const FVector& AttackDirection,
	                               const FVampsCombatStepSettings& StepSettings,
	                               bool bBlocked);
	void ResetCombatMotion();

protected:

	virtual void TickCharacterPose(float DeltaTime) override;
	virtual void ApplyRootMotionToVelocity(float DeltaTime) override;
	virtual void PhysicsRotation(float DeltaTime) override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual void HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta) override;
	virtual float SlideAlongSurface(
	    const FVector& Delta, float Time, const FVector& Normal, FHitResult& Hit, bool bHandleImpact) override;

private:

	FVector CombatMotionStart = FVector::ZeroVector;
	FVector CombatMotionGoal = FVector::ZeroVector;
	FVector CombatPushDirection = FVector::ZeroVector;
	FQuat CombatFacingStart = FQuat::Identity;
	FQuat CombatFacingGoal = FQuat::Identity;
	double CombatMotionStartedAt = 0;
	float CombatMotionDuration = 0.f;
	float PendingKnockbackDistance = 0.f;
	int32 ReactionMontageInstanceID = INDEX_NONE;

	void BeginCombatMotion(EVampsCombatMotion NewMotion, const FVector& Destination, float Duration);
	void BeginCombatKnockback();
	void FinishCombatMotion();
};
