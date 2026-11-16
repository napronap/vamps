#include "Combat/VampsCombatMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Characters/VampsCharacter.h"
#include "Characters/VampsLockOnComponent.h"
#include "Combat/Data/VampsCombatSettings.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

void UVampsCombatMovementComponent::TickComponent(float DeltaTime,
                                                  ELevelTick TickType,
                                                  FActorComponentTickFunction* ThisTickFunction)
{
	const AVampsCharacter* Fighter = Cast<AVampsCharacter>(CharacterOwner);
	if (Fighter && !Fighter->CombatState.bInHitstop)
	{
		CombatMotionElapsed += DeltaTime;
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

float UVampsCombatMovementComponent::GetMaxSpeed() const
{
	const AVampsCharacter* Fighter = Cast<AVampsCharacter>(CharacterOwner);
	return Fighter && Fighter->CombatState.bIsSprinting ? Fighter->CombatSettings->SprintSpeed : Super::GetMaxSpeed();
}

void UVampsCombatMovementComponent::StopVoluntaryMotion()
{
	if (CombatMotion == EVampsCombatMotion::Advance)
	{
		CombatMotion = EVampsCombatMotion::None;
		Velocity.X = 0.f;
		Velocity.Y = 0.f;
	}
}

void UVampsCombatMovementComponent::BeginOffensiveAdvance()
{
	StopVoluntaryMotion();
	const AVampsCharacter* Fighter = Cast<AVampsCharacter>(CharacterOwner);
	const AVampsCharacter* TargetFighter = Fighter && Fighter->LockOn ? Fighter->LockOn->GetCurrentTarget() : nullptr;

	if (!IsValid(TargetFighter) || !TargetFighter->CombatState.bIsAlive || !IsMovingOnGround())
	{
		return;
	}

	const FVector StartLocation = UpdatedComponent->GetComponentLocation();
	const FVector ToTarget = TargetFighter->GetActorLocation() - StartLocation;
	const FVector AdvanceDirection = ToTarget.GetSafeNormal2D();

	if (AdvanceDirection.IsNearlyZero())
	{
		return;
	}

	const float DistanceToTarget = float(ToTarget.Size2D());
	const float StopDistance = Fighter->CombatSettings->ApproachStopDistance;
	const float MaxAdvanceDistance = Fighter->CombatSettings->ApproachDistance;
	const float DistanceBeforeStop = FMath::Max(0.f, DistanceToTarget - StopDistance);
	const float AdvanceDistance = FMath::Min(MaxAdvanceDistance, DistanceBeforeStop);
	const FVector AdvanceGoal = StartLocation + AdvanceDirection * AdvanceDistance;

	CombatFacingStart = UpdatedComponent->GetComponentQuat();
	CombatFacingGoal = AdvanceDirection.ToOrientationQuat();

	BeginCombatMotion(EVampsCombatMotion::Advance, AdvanceGoal, Fighter->CombatSettings->ApproachDuration);
}

void UVampsCombatMovementComponent::ReceiveCombatDisplacement(const FVector& AttackOrigin,
                                                              const FVector& AttackDirection,
                                                              const FVampsCombatStepSettings& StepSettings,
                                                              bool bBlocked)
{
	CombatPushDirection = AttackDirection.GetSafeNormal2D();

	StopVoluntaryMotion();

	PendingKnockbackDistance = StepSettings.KnockbackDistance;
	bCombatMotionBlocked = false;
	CombatFacingStart = UpdatedComponent->GetComponentQuat();
	CombatFacingGoal = (-CombatPushDirection).ToOrientationQuat();

	if (bBlocked)
	{
		BeginCombatKnockback();
		return;
	}

	FVector AlignmentGoal = AttackOrigin + CombatPushDirection * StepSettings.TargetAlignmentDistance;
	AlignmentGoal.Z = UpdatedComponent->GetComponentLocation().Z;

	BeginCombatMotion(EVampsCombatMotion::Alignment, AlignmentGoal, 0.08f);
}

void UVampsCombatMovementComponent::BeginCombatMotion(EVampsCombatMotion NewMotion,
                                                      const FVector& Destination,
                                                      float Duration)
{
	CombatMotion = NewMotion;
	CombatMotionStart = UpdatedComponent->GetComponentLocation();
	CombatMotionGoal = Destination;
	CombatMotionElapsed = 0.f;
	CombatMotionDuration = Duration;
	bCombatMotionBlocked = false;
	Velocity.X = 0.f;
	Velocity.Y = 0.f;
}

void UVampsCombatMovementComponent::FinishCombatMotion()
{
	if (CombatMotion == EVampsCombatMotion::Alignment)
	{
		BeginCombatKnockback();
		return;
	}
	CombatMotion = EVampsCombatMotion::None;
	Velocity.X = 0.f;
	Velocity.Y = 0.f;
}

void UVampsCombatMovementComponent::BeginCombatKnockback()
{
	if (PendingKnockbackDistance == 0.f)
	{
		CombatMotion = EVampsCombatMotion::None;
		Velocity.X = 0.f;
		Velocity.Y = 0.f;
		return;
	}
	const FVector KnockbackGoal =
	    UpdatedComponent->GetComponentLocation() + CombatPushDirection * PendingKnockbackDistance;
	PendingKnockbackDistance = 0.f;
	BeginCombatMotion(EVampsCombatMotion::Knockback, KnockbackGoal, 0.12f);
}

void UVampsCombatMovementComponent::TickCharacterPose(float DeltaTime)
{
	const AVampsCharacter& Fighter = *static_cast<const AVampsCharacter*>(CharacterOwner.Get());
	const auto HasCombatMotionOrReaction = [this, &Fighter]()
	{
		return CombatMotion != EVampsCombatMotion::None || Fighter.CombatState.Reaction != EVampsCombatReaction::None;
	};

	// The pose tick can end the attack after extracting its last root motion frame
	const bool bSuppressRootMotionBeforeTick = HasCombatMotionOrReaction() || Fighter.CombatState.MontageInstanceID != INDEX_NONE;

	Super::TickCharacterPose(DeltaTime);

	const FAnimMontageInstance* RootPlayback = Fighter.GetRootMotionAnimMontageInstance();
	const bool bAttackRootMotion = RootPlayback && RootPlayback->Montage == Fighter.CombatSettings->AttackMontage;
	if (bSuppressRootMotionBeforeTick || HasCombatMotionOrReaction() || bAttackRootMotion)
	{
		RootMotionParams.Clear();
	}
}

void UVampsCombatMovementComponent::ApplyRootMotionToVelocity(float DeltaTime)
{
	Super::ApplyRootMotionToVelocity(DeltaTime);

	// only calculate a combat velocity when a move is active and the time step is large enough to divide by
	if (CombatMotion == EVampsCombatMotion::None || DeltaTime < MIN_TICK_TIME)
	{
		const AVampsCharacter* Fighter = Cast<AVampsCharacter>(CharacterOwner);

		// clear horizontal drift when the character cannot move or is still in an attack montage
		if (Fighter && (!Fighter->CombatState.bCanMove || Fighter->CombatState.MontageInstanceID != INDEX_NONE))
		{
			Velocity.X = 0.f;
			Velocity.Y = 0.f;
		}
		return;
	}

	const float Progress = FMath::Clamp(CombatMotionElapsed / CombatMotionDuration, 0.f, 1.f);

	const FVector DesiredPosition = FMath::Lerp(CombatMotionStart, CombatMotionGoal, Progress);
	const FVector DesiredVelocity = (DesiredPosition - UpdatedComponent->GetComponentLocation()) / DeltaTime;

	Velocity.X = DesiredVelocity.X;
	Velocity.Y = DesiredVelocity.Y;
}

void UVampsCombatMovementComponent::PhysicsRotation(float DeltaTime)
{
	// during the advance, turn toward the direction captured at the start of the attack.
	if (CombatMotion == EVampsCombatMotion::Advance)
	{
		const FRotator Facing = FMath::RInterpConstantTo(
		    UpdatedComponent->GetComponentRotation(), CombatFacingGoal.Rotator(), DeltaTime, 500.f);

		MoveUpdatedComponent(FVector::ZeroVector, Facing, true);

		return;
	}
	// use normal locomotion rotation unless the dash or attack has already chosen a facing
	if (CombatMotion == EVampsCombatMotion::None)
	{
		const AVampsCharacter* Fighter = Cast<AVampsCharacter>(CharacterOwner);
		if (Fighter && (Fighter->CombatState.bIsDashing || Fighter->CombatState.MontageInstanceID != INDEX_NONE))
		{
			return;
		}

		Super::PhysicsRotation(DeltaTime);

		return;
	}

	// during a hit displacement, turn the recipient against the push direction over the move's duration.
	if (CombatMotion == EVampsCombatMotion::Alignment || CombatMotion == EVampsCombatMotion::Knockback)
	{
		const float Progress = FMath::Clamp(CombatMotionElapsed / CombatMotionDuration, 0.f, 1.f);

		MoveUpdatedComponent(FVector::ZeroVector, FQuat::Slerp(CombatFacingStart, CombatFacingGoal, Progress), true);
	}
}

void UVampsCombatMovementComponent::OnMovementUpdated(float DeltaSeconds,
                                                      const FVector& OldLocation,
                                                      const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

	// finish the current move after a blocking hit or when its allotted time expires
	if (CombatMotion != EVampsCombatMotion::None &&
	    (bCombatMotionBlocked || CombatMotionElapsed >= CombatMotionDuration))
	{
		FinishCombatMotion();
	}
}

void UVampsCombatMovementComponent::HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta)
{
	if (CombatMotion != EVampsCombatMotion::None && Hit.IsValidBlockingHit())
	{
		bCombatMotionBlocked = true;
	}

	Super::HandleImpact(Hit, TimeSlice, MoveDelta);
}

float UVampsCombatMovementComponent::SlideAlongSurface(
    const FVector& Delta, float Time, const FVector& Normal, FHitResult& Hit, bool bHandleImpact)
{
	if (CombatMotion != EVampsCombatMotion::None)
	{
		return 0.f;
	}

	return Super::SlideAlongSurface(Delta, Time, Normal, Hit, bHandleImpact);
}

void UVampsCombatMovementComponent::ResetCombatMotion()
{
	StopVoluntaryMotion();
	CombatMotion = EVampsCombatMotion::None;
	PendingKnockbackDistance = 0.f;
	bCombatMotionBlocked = false;

	if (UAnimInstance* Animation = CharacterOwner->GetMesh()->GetAnimInstance())
	{
		Animation->StopAllMontages(0.f);
	}

	RootMotionParams.Clear();

	StopMovementImmediately();
}
