#include "Player/VampsPlayerCameraManager.h"
#include "Characters/VampsCharacter.h"
#include "Characters/VampsLockOnComponent.h"
#include "GameFramework/PlayerController.h"
#include "Combat/VampsCombatLog.h"

void AVampsPlayerCameraManager::ProcessViewRotation(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
	if (!PCOwner)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: camera rotation cancelled; PlayerController owner is missing."),
		       __FUNCTION__,
		       *GetPathName());
		return;
	}
	AVampsCharacter* ControlledFighter = Cast<AVampsCharacter>(PCOwner->GetPawn());
	if (!PCOwner->IsLocalPlayerController() || !ControlledFighter)
	{
		Super::ProcessViewRotation(DeltaTime, OutViewRotation, OutDeltaRot);
		return;
	}
	if (!ControlledFighter->LockOn || !FMath::IsFinite(TrackingSpeed) || !FMath::IsFinite(HorizontalTolerance) ||
	    !FMath::IsFinite(FollowPitch) || TrackingSpeed <= 0.f || HorizontalTolerance < 0.f ||
	    HorizontalTolerance >= 180.f)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: camera rotation cancelled; fighter='%s', LockOn='%s', TrackingSpeed=%f, "
		            "HorizontalTolerance=%f, FollowPitch=%f. Expected positive speed, tolerance in [0,180) and finite "
		            "settings."),
		       __FUNCTION__,
		       *GetPathName(),
		       *ControlledFighter->GetPathName(),
		       *GetPathNameSafe(ControlledFighter->LockOn),
		       TrackingSpeed,
		       HorizontalTolerance,
		       FollowPitch);
		return;
	}

	AVampsCharacter* LockedFighter = ControlledFighter->LockOn->GetCurrentTarget();
	FVector FollowDirection = ControlledFighter->GetActorForwardVector();
	if (IsValid(LockedFighter))
	{
		FollowDirection = LockedFighter->GetActorLocation() - ControlledFighter->GetActorLocation();
	}

	double DesiredYaw = OutViewRotation.Yaw;
	if (FollowDirection.SizeSquared2D() > UE_SMALL_NUMBER)
	{
		DesiredYaw = FollowDirection.Rotation().Yaw;
	}
	double YawCorrection = FMath::FindDeltaAngleDegrees(OutViewRotation.Yaw, DesiredYaw);
	const double AppliedTolerance = IsValid(LockedFighter) ? HorizontalTolerance : 0.f;
	YawCorrection -= FMath::Clamp(YawCorrection, -AppliedTolerance, AppliedTolerance);

	const float RotationAlpha = 1.f - FMath::Exp(-TrackingSpeed * DeltaTime);
	OutViewRotation.Yaw = FRotator::NormalizeAxis(OutViewRotation.Yaw + YawCorrection * RotationAlpha);
	OutViewRotation.Pitch = FRotator::NormalizeAxis(
	    OutViewRotation.Pitch + FMath::FindDeltaAngleDegrees(OutViewRotation.Pitch, FollowPitch) * RotationAlpha);
	OutViewRotation.Roll = 0.f;
	OutDeltaRot = FRotator::ZeroRotator;
	Super::ProcessViewRotation(DeltaTime, OutViewRotation, OutDeltaRot);
}
