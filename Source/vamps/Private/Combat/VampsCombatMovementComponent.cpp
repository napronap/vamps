#include "Combat/VampsCombatMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Characters/VampsCharacter.h"
#include "Characters/VampsLockOnComponent.h"
#include "Combat/Data/VampsCombatSettings.h"
#include "Combat/VampsCombatLog.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

float UVampsCombatMovementComponent::GetMaxSpeed() const
{
	const AVampsCharacter* Fighter = Cast<AVampsCharacter>(CharacterOwner);
	return Fighter && Fighter->CombatSettings && Fighter->CombatState.bIsSprinting
	           ? Fighter->CombatSettings->SprintSpeed
	           : Super::GetMaxSpeed();
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
	const FVector ToTarget = TargetFighter->GetActorLocation() - UpdatedComponent->GetComponentLocation();
	const float AdvanceLength =
	    FMath::Min(Fighter->CombatSettings->ApproachDistance,
	               FMath::Max(0.f, float(ToTarget.Size2D()) - Fighter->CombatSettings->ApproachStopDistance));
	if (ToTarget.GetSafeNormal2D().IsNearlyZero())
	{
		return;
	}
	CombatFacingStart = UpdatedComponent->GetComponentQuat();
	CombatFacingGoal = ToTarget.GetSafeNormal2D().ToOrientationQuat();
	BeginCombatMotion(EVampsCombatMotion::Advance,
	                  UpdatedComponent->GetComponentLocation() + ToTarget.GetSafeNormal2D() * AdvanceLength,
	                  Fighter->CombatSettings->ApproachDuration);
}

void UVampsCombatMovementComponent::ReceiveCombatDisplacement(const FVector& AttackOrigin,
                                                              const FVector& AttackDirection,
                                                              const FVampsCombatStepSettings& StepSettings,
                                                              bool bBlocked)
{
	AVampsCharacter* Fighter = Cast<AVampsCharacter>(CharacterOwner);
	if (!IsValid(Fighter) || !UpdatedComponent || !Fighter->HasAuthority())
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: displacement requires a valid authoritative VampsCharacter and UpdatedComponent; "
		            "owner='%s', component='%s'."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(CharacterOwner),
		       *GetPathNameSafe(UpdatedComponent));
		return;
	}
	if (!FMath::IsFinite(StepSettings.TargetAlignmentDistance) || !FMath::IsFinite(StepSettings.KnockbackDistance) ||
	    StepSettings.TargetAlignmentDistance <= 0.f || StepSettings.KnockbackDistance < 0.f ||
	    AttackOrigin.ContainsNaN() || AttackDirection.ContainsNaN() || AttackDirection.GetSafeNormal2D().IsNearlyZero())
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: invalid displacement: alignment=%f, knockback=%f, origin=%s, direction=%s. Expected "
		            "positive alignment, nonnegative knockback and finite horizontal direction."),
		       __FUNCTION__,
		       *Fighter->GetPathName(),
		       StepSettings.TargetAlignmentDistance,
		       StepSettings.KnockbackDistance,
		       *AttackOrigin.ToString(),
		       *AttackDirection.ToString());
		return;
	}
	if (!Fighter->CombatSettings)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: cannot resolve combat displacement without CombatSettings."),
		       __FUNCTION__,
		       *Fighter->GetPathName());
		return;
	}
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
	CombatMotionStartedAt = GetWorld()->GetTimeSeconds();
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
	const AVampsCharacter* Fighter = Cast<AVampsCharacter>(CharacterOwner);
	const bool bControlledPose = CombatMotion != EVampsCombatMotion::None ||
	                             (Fighter && (Fighter->CombatState.Reaction != EVampsCombatReaction::None ||
	                                          Fighter->CombatState.MontageInstanceID != INDEX_NONE));
	Super::TickCharacterPose(DeltaTime);
	UAnimInstance* Animation = Fighter ? Fighter->GetMesh()->GetAnimInstance() : nullptr;
	if (!Fighter || !Fighter->CombatSettings || !Animation)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: root motion update requires VampsCharacter, CombatSettings and AnimInstance; owner='%s', "
		            "animation='%s'. Movement tick disabled."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(CharacterOwner),
		       *GetPathNameSafe(Animation));
		RootMotionParams.Clear();
		SetComponentTickEnabled(false);
		return;
	}
	const FAnimMontageInstance* RootPlayback = Animation->GetRootMotionMontageInstance();
	if (bControlledPose || CombatMotion != EVampsCombatMotion::None ||
	    Fighter->CombatState.Reaction != EVampsCombatReaction::None ||
	    (RootPlayback && RootPlayback->Montage == Fighter->CombatSettings->AttackMontage))
	{
		RootMotionParams.Clear();
	}
}

void UVampsCombatMovementComponent::ApplyRootMotionToVelocity(float DeltaTime)
{
	Super::ApplyRootMotionToVelocity(DeltaTime);
	if (CombatMotion == EVampsCombatMotion::None || DeltaTime < MIN_TICK_TIME)
	{
		const AVampsCharacter* Fighter = Cast<AVampsCharacter>(CharacterOwner);
		if (Fighter && (!Fighter->CombatState.bCanMove || Fighter->CombatState.MontageInstanceID != INDEX_NONE))
		{
			Velocity.X = 0.f;
			Velocity.Y = 0.f;
		}
		return;
	}
	const float Progress =
	    FMath::Clamp(float((GetWorld()->GetTimeSeconds() - CombatMotionStartedAt) / CombatMotionDuration), 0.f, 1.f);
	const FVector DesiredPosition = FMath::Lerp(CombatMotionStart, CombatMotionGoal, Progress);
	const FVector DesiredVelocity = (DesiredPosition - UpdatedComponent->GetComponentLocation()) / DeltaTime;
	Velocity.X = DesiredVelocity.X;
	Velocity.Y = DesiredVelocity.Y;
}

void UVampsCombatMovementComponent::PhysicsRotation(float DeltaTime)
{
	if (CombatMotion == EVampsCombatMotion::Advance)
	{
		const FRotator Facing = FMath::RInterpConstantTo(
		    UpdatedComponent->GetComponentRotation(), CombatFacingGoal.Rotator(), DeltaTime, 500.f);
		MoveUpdatedComponent(FVector::ZeroVector, Facing, true);
		return;
	}
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
	if (CombatMotion == EVampsCombatMotion::Alignment || CombatMotion == EVampsCombatMotion::Knockback)
	{
		const float Progress = FMath::Clamp(
		    float((GetWorld()->GetTimeSeconds() - CombatMotionStartedAt) / CombatMotionDuration), 0.f, 1.f);
		MoveUpdatedComponent(FVector::ZeroVector, FQuat::Slerp(CombatFacingStart, CombatFacingGoal, Progress), true);
	}
}

void UVampsCombatMovementComponent::OnMovementUpdated(float DeltaSeconds,
                                                      const FVector& OldLocation,
                                                      const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
	if (CombatMotion != EVampsCombatMotion::None &&
	    (bCombatMotionBlocked || GetWorld()->GetTimeSeconds() >= CombatMotionStartedAt + CombatMotionDuration))
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
	UAnimInstance* Animation = CharacterOwner ? CharacterOwner->GetMesh()->GetAnimInstance() : nullptr;
	if (Animation)
	{
		Animation->StopAllMontages(0.f);
	}
	else if (CharacterOwner && CharacterOwner->HasActorBegunPlay() && !CharacterOwner->IsActorBeingDestroyed())
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: cannot stop combat montages; mesh has no AnimInstance."),
		       __FUNCTION__,
		       *CharacterOwner->GetPathName());
	}
	RootMotionParams.Clear();
	StopMovementImmediately();
}
