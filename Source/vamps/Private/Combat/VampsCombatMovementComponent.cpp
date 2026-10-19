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
	if (CombatMotion == EVampsCombatMotion::Advance || CombatMotion == EVampsCombatMotion::Dash)
	{
		CombatMotion = EVampsCombatMotion::None;
		Velocity.X = 0.f;
		Velocity.Y = 0.f;
	}
	UAnimInstance* Animation = CharacterOwner ? CharacterOwner->GetMesh()->GetAnimInstance() : nullptr;
	const FAnimMontageInstance* Playback =
	    Animation ? Animation->GetMontageInstanceForID(DashPlaybackInstanceID) : nullptr;
	if (Playback)
	{
		Animation->Montage_Stop(0.08f, Playback->Montage);
	}
	DashPlaybackInstanceID = INDEX_NONE;
}

void UVampsCombatMovementComponent::BeginOffensiveAdvance(const FVampsCombatStepSettings& StepSettings)
{
	StopVoluntaryMotion();
	const AVampsCharacter* Fighter = Cast<AVampsCharacter>(CharacterOwner);
	const AVampsCharacter* TargetFighter = Fighter && Fighter->LockOn ? Fighter->LockOn->GetCurrentTarget() : nullptr;
	if (!IsValid(TargetFighter) || !TargetFighter->CombatState.bIsAlive || !IsMovingOnGround())
	{
		return;
	}
	const FVector ToTarget = TargetFighter->GetActorLocation() - UpdatedComponent->GetComponentLocation();
	const float AdvanceLength = FMath::Min(
	    StepSettings.AdvanceDistance, FMath::Max(0.f, float(ToTarget.Size2D()) - StepSettings.AdvanceStopDistance));
	if (ToTarget.GetSafeNormal2D().IsNearlyZero())
	{
		return;
	}
	CombatFacingStart = UpdatedComponent->GetComponentQuat();
	CombatFacingGoal = ToTarget.GetSafeNormal2D().ToOrientationQuat();
	BeginCombatMotion(EVampsCombatMotion::Advance,
	                  UpdatedComponent->GetComponentLocation() + ToTarget.GetSafeNormal2D() * AdvanceLength,
	                  StepSettings.AdvanceDuration);
}

UAnimMontage* UVampsCombatMovementComponent::FindCombatDashMontage(const FVector& DashDirection) const
{
	const AVampsCharacter* Fighter = Cast<AVampsCharacter>(CharacterOwner);
	const UVampsCombatSettings* Settings = Fighter ? Fighter->CombatSettings.Get() : nullptr;
	UAnimInstance* Animation = Fighter ? Fighter->GetMesh()->GetAnimInstance() : nullptr;
	if (!Settings || !Settings->HasValidMobilitySettings() || !Animation || !IsMovingOnGround() ||
	    DashDirection.ContainsNaN() || DashDirection.GetSafeNormal2D().IsNearlyZero())
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': dash requires valid settings, animation, ground and direction=%s."),
		       __FUNCTION__,
		       *GetPathName(),
		       *DashDirection.ToString());
		return nullptr;
	}
	const float Angle = FMath::UnwindDegrees(DashDirection.Rotation().Yaw - Fighter->GetActorRotation().Yaw);
	const int32 DirectionIndex = (FMath::RoundToInt(Angle / 45.f) + 8) % 8;
	UAnimMontage* Montage =
	    Settings->DashMontages.IsValidIndex(DirectionIndex) ? Settings->DashMontages[DirectionIndex].Get() : nullptr;
	if (Settings->DashMontages.Num() != 8 || !Montage || Montage == Settings->AttackMontage ||
	    Montage == Settings->GuardReactionMontage || Montage == Settings->HitReactionMontage ||
	    Montage->GetPlayLength() <= 0.f || !FMath::IsFinite(Montage->RateScale) || Montage->RateScale <= 0.f ||
	    !Montage->bEnableAutoBlendOut || Montage->TimeStretchCurve.IsValid() || Montage->SlotAnimTracks.Num() != 1 ||
	    !FMath::IsNearlyEqual(Montage->SlotAnimTracks[0].AnimTrack.GetLength(), Montage->GetPlayLength(), 0.001f))
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': invalid dash montage[%d]='%s' in '%s'. Define 8 independent constant-speed "
		            "montages, one slot, Auto Blend Out and matching segment/montage lengths. Recalculate length in "
		            "the editor."),
		       __FUNCTION__,
		       *GetPathName(),
		       DirectionIndex,
		       *GetPathNameSafe(Montage),
		       *Settings->GetPathName());
		return nullptr;
	}
	return Montage;
}

bool UVampsCombatMovementComponent::TryStartCombatDash(const FVector& DashDirection, UAnimMontage* DashMontage)
{
	AVampsCharacter* Fighter = Cast<AVampsCharacter>(CharacterOwner);
	UAnimInstance* Animation = Fighter ? Fighter->GetMesh()->GetAnimInstance() : nullptr;
	if (!Fighter || !Fighter->CombatSettings || !Animation || !DashMontage)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': missing dash owner, settings, animation or montage '%s'."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(DashMontage));
		return false;
	}
	StopVoluntaryMotion();
	const float Played = Animation->Montage_Play(
	    DashMontage, DashMontage->GetPlayLength() / (Fighter->CombatSettings->DashDuration * DashMontage->RateScale));
	const FAnimMontageInstance* Playback = Animation->GetActiveInstanceForMontage(DashMontage);
	if (Played <= 0.f || !Playback)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': dash montage '%s' did not start; verify skeleton and slot."),
		       __FUNCTION__,
		       *Fighter->GetPathName(),
		       *DashMontage->GetPathName());
		return false;
	}
	DashPlaybackInstanceID = Playback->GetInstanceID();
	PendingKnockbackDistance = 0.f;
	DashFacing = UpdatedComponent->GetComponentQuat();
	BeginCombatMotion(EVampsCombatMotion::Dash,
	                  UpdatedComponent->GetComponentLocation() +
	                      DashDirection.GetSafeNormal2D() * Fighter->CombatSettings->DashDistance,
	                  Fighter->CombatSettings->DashDuration);
	return true;
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
	if (CombatMotion == EVampsCombatMotion::Dash)
	{
		StopVoluntaryMotion();
		return;
	}
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
	    (RootPlayback && (RootPlayback->Montage == Fighter->CombatSettings->AttackMontage ||
	                      Fighter->CombatSettings->DashMontages.Contains(RootPlayback->Montage))))
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
		if (Fighter && (!Fighter->CombatState.bCanMove || (Fighter->CombatState.MontageInstanceID != INDEX_NONE &&
		                                                   !Fighter->CombatState.bRecoveryVisualReleased)))
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
	if (CombatMotion == EVampsCombatMotion::Dash)
	{
		MoveUpdatedComponent(FVector::ZeroVector, DashFacing, true);
		return;
	}
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
		if (Fighter && Fighter->CombatState.MontageInstanceID != INDEX_NONE &&
		    !Fighter->CombatState.bRecoveryVisualReleased)
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
