#include "Combat/VampsCombatMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Characters/VampsCharacter.h"
#include "Combat/Data/VampsCombatSettings.h"
#include "Combat/VampsCombatLog.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

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
	if (!bBlocked && Fighter->CombatSettings->HitReactionMontage)
	{
		UAnimMontage* ReactionMontage = Fighter->CombatSettings->HitReactionMontage;
		if (ReactionMontage == Fighter->CombatSettings->AttackMontage || !ReactionMontage->bEnableAutoBlendOut)
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: %s: HitReactionMontage '%s' must differ from AttackMontage and enable Auto Blend Out. "
			            "Displacement cancelled."),
			       __FUNCTION__,
			       *Fighter->GetPathName(),
			       *ReactionMontage->GetPathName());
			return;
		}
		UAnimInstance* Animation = Fighter->GetMesh()->GetAnimInstance();
		if (!Animation)
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: %s: cannot play reaction '%s'; mesh '%s' has no AnimInstance."),
			       __FUNCTION__,
			       *Fighter->GetPathName(),
			       *ReactionMontage->GetPathName(),
			       *Fighter->GetMesh()->GetPathName());
			return;
		}
		const float ReactionDuration = Animation->Montage_Play(ReactionMontage);
		const FAnimMontageInstance* ReactionPlayback = Animation->GetActiveInstanceForMontage(ReactionMontage);
		if (!(ReactionDuration > 0.f) || !ReactionPlayback)
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: %s: reaction '%s' failed to start (duration=%f, valid instance=%d). Verify skeleton and "
			            "slot. Displacement cancelled."),
			       __FUNCTION__,
			       *Fighter->GetPathName(),
			       *ReactionMontage->GetPathName(),
			       ReactionDuration,
			       ReactionPlayback != nullptr);
			Animation->Montage_Stop(0.f, ReactionMontage);
			return;
		}
		ReactionMontageInstanceID = ReactionPlayback->GetInstanceID();
	}
	CombatPushDirection = AttackDirection.GetSafeNormal2D();
	PendingKnockbackDistance = StepSettings.KnockbackDistance;
	bCombatMotionBlocked = false;
	if (bBlocked)
	{
		BeginCombatKnockback();
		return;
	}

	FVector AlignmentGoal = AttackOrigin + CombatPushDirection * StepSettings.TargetAlignmentDistance;
	AlignmentGoal.Z = UpdatedComponent->GetComponentLocation().Z;
	CombatFacingStart = UpdatedComponent->GetComponentQuat();
	CombatFacingGoal = (-CombatPushDirection).ToOrientationQuat();
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
	Super::TickCharacterPose(DeltaTime);
	const AVampsCharacter* Fighter = Cast<AVampsCharacter>(CharacterOwner);
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
	if (CombatMotion != EVampsCombatMotion::None ||
	    (RootPlayback && (RootPlayback->GetInstanceID() == ReactionMontageInstanceID ||
	                      (Fighter->CombatState.bRecoveryVisualReleased &&
	                       RootPlayback->Montage == Fighter->CombatSettings->AttackMontage))))
	{
		RootMotionParams.Clear();
	}
}

void UVampsCombatMovementComponent::ApplyRootMotionToVelocity(float DeltaTime)
{
	Super::ApplyRootMotionToVelocity(DeltaTime);
	if (CombatMotion == EVampsCombatMotion::None || DeltaTime < MIN_TICK_TIME)
	{
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
	if (CombatMotion == EVampsCombatMotion::None)
	{
		Super::PhysicsRotation(DeltaTime);
		return;
	}
	if (CombatMotion == EVampsCombatMotion::Alignment)
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
	ReactionMontageInstanceID = INDEX_NONE;
	RootMotionParams.Clear();
	StopMovementImmediately();
}
