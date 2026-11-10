#include "Player/VampsPlayerCameraManager.h"
#include "Characters/VampsCharacter.h"
#include "Characters/VampsLockOnComponent.h"
#include "Combat/VampsCombatLog.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "HAL/IConsoleManager.h"

namespace
{
	float FollowDistance = 600.f;
	float CombatDistance = 425.f;
	float FollowEyeHeight = 70.f;
	float CombatEyeHeight = 30.f;
	float FollowInterestHeight = 30.f;
	float CombatInterestHeight = -20.f;
	float FollowVerticalFov = 38.f;
	float CombatVerticalFov = 32.4f;
	float CombatInteriorDistance = 95.f;
	float CombatYawOffset = 20.f;
	float PositionRadius = 180.f;
	float AngularSpeed = 6.f;
	float TransitionDuration = 0.3f;

#if !UE_BUILD_SHIPPING
	FAutoConsoleVariableRef FollowDistanceSetting(TEXT("vamps.Camera.FollowDistance"),
	                                              FollowDistance,
	                                              TEXT("Follow distance in cm."),
	                                              ECVF_Cheat);
	FAutoConsoleVariableRef CombatDistanceSetting(TEXT("vamps.Camera.CombatDistance"),
	                                              CombatDistance,
	                                              TEXT("Combat distance in cm."),
	                                              ECVF_Cheat);
	FAutoConsoleVariableRef FollowEyeSetting(TEXT("vamps.Camera.FollowEyeHeight"),
	                                         FollowEyeHeight,
	                                         TEXT("Follow eye height above capsule center in cm."),
	                                         ECVF_Cheat);
	FAutoConsoleVariableRef CombatEyeSetting(TEXT("vamps.Camera.CombatEyeHeight"),
	                                         CombatEyeHeight,
	                                         TEXT("Combat eye height above capsule center in cm."),
	                                         ECVF_Cheat);
	FAutoConsoleVariableRef FollowInterestSetting(TEXT("vamps.Camera.FollowInterestHeight"),
	                                              FollowInterestHeight,
	                                              TEXT("Follow interest height in cm."),
	                                              ECVF_Cheat);
	FAutoConsoleVariableRef CombatInterestSetting(TEXT("vamps.Camera.CombatInterestHeight"),
	                                              CombatInterestHeight,
	                                              TEXT("Combat interest height in cm."),
	                                              ECVF_Cheat);
	FAutoConsoleVariableRef FollowFovSetting(TEXT("vamps.Camera.FollowVerticalFOV"),
	                                         FollowVerticalFov,
	                                         TEXT("Follow vertical FOV in degrees."),
	                                         ECVF_Cheat);
	FAutoConsoleVariableRef CombatFovSetting(TEXT("vamps.Camera.CombatVerticalFOV"),
	                                         CombatVerticalFov,
	                                         TEXT("Combat vertical FOV in degrees."),
	                                         ECVF_Cheat);
	FAutoConsoleVariableRef InteriorSetting(TEXT("vamps.Camera.InteriorDistance"),
	                                        CombatInteriorDistance,
	                                        TEXT("Combat pivot displacement toward target in cm."),
	                                        ECVF_Cheat);
	FAutoConsoleVariableRef YawSetting(TEXT("vamps.Camera.CombatYawOffset"),
	                                   CombatYawOffset,
	                                   TEXT("Combat composition yaw offset in degrees."),
	                                   ECVF_Cheat);
	FAutoConsoleVariableRef RadiusSetting(TEXT("vamps.Camera.PositionRadius"),
	                                      PositionRadius,
	                                      TEXT("Spatial filter radius in cm; zero disables it."),
	                                      ECVF_Cheat);
	FAutoConsoleVariableRef AngularSetting(TEXT("vamps.Camera.AngularSpeed"),
	                                       AngularSpeed,
	                                       TEXT("Angular response per second."),
	                                       ECVF_Cheat);
	FAutoConsoleVariableRef TransitionSetting(TEXT("vamps.Camera.TransitionDuration"),
	                                          TransitionDuration,
	                                          TEXT("Framing and target transition duration in seconds."),
	                                          ECVF_Cheat);
#endif
} // namespace

FVector AVampsPlayerCameraManager::SpatialFollow(const FVector& PreviousPosition,
                                                 const FVector& DesiredPosition,
                                                 float DeltaTime)
{
	if (PositionRadius == 0.f)
	{
		return DesiredPosition;
	}
	const float ReferenceAlpha = FMath::Square(FMath::Min(
	    float(FVector::Distance(PreviousPosition, DesiredPosition)) / FMath::Max(PositionRadius, 0.1f), 1.f));
	const float TimeAlpha = 1.f - FMath::Pow(1.f - ReferenceAlpha, DeltaTime * 60.f);
	return FMath::Lerp(PreviousPosition, DesiredPosition, TimeAlpha);
}

void AVampsPlayerCameraManager::InitializeFor(APlayerController* OwningController)
{
	Super::InitializeFor(OwningController);
	OwningController->OnPossessedPawnChanged.AddUniqueDynamic(this,
	                                                          &AVampsPlayerCameraManager::HandleObservedPawnChanged);
	HandleObservedPawnChanged(nullptr, OwningController->GetPawn());
}

void AVampsPlayerCameraManager::HandleObservedPawnChanged(APawn*, APawn* NewPawn)
{
	if (AVampsCharacter* PreviousFighter = ObservedFighter.Get())
	{
		PreviousFighter->OnCombatFramingRequested.RemoveAll(this);
	}
	ResetCameraState();
	AVampsCharacter* ViewingFighter = Cast<AVampsCharacter>(NewPawn);
	ObservedFighter = ViewingFighter;
	if (ViewingFighter)
	{
		ObservedGeneration = ViewingFighter->CombatGeneration;
		ViewingFighter->OnCombatFramingRequested.AddUObject(this,
		                                                    &AVampsPlayerCameraManager::HandleCombatFramingRequested);
	}
}

void AVampsPlayerCameraManager::HandleCombatFramingRequested(AVampsCharacter* ChainSource)
{
	if (!PCOwner || !PCOwner->IsLocalPlayerController())
	{
		return;
	}
	if (!IsValid(ChainSource) || ChainSource->GetWorld() != GetWorld())
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': framing requires a valid chain source in this world; source='%s'."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(ChainSource));
		return;
	}
	if (ObservedGeneration != ObservedFighter->CombatGeneration)
	{
		ResetCameraState();
		ObservedGeneration = ObservedFighter->CombatGeneration;
	}
	FramingSource = ChainSource;
	FramingGeneration = ChainSource->CombatGeneration;
	FramingMontageID = ChainSource->CombatState.MontageInstanceID;
}

void AVampsPlayerCameraManager::UpdateCamera(float DeltaTime)
{
	Super::UpdateCamera(DeltaTime);
	if (bHasCameraState && PCOwner && PCOwner->IsLocalPlayerController() && GetViewTarget() == ObservedFighter.Get())
	{
		PCOwner->SetControlRotation(GetCameraRotation());
	}
}

void AVampsPlayerCameraManager::ProcessViewRotation(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
	OutDeltaRot = FRotator::ZeroRotator;
	if (bHasCameraState)
	{
		OutViewRotation = GetCameraRotation();
	}
	Super::ProcessViewRotation(DeltaTime, OutViewRotation, OutDeltaRot);
}

void AVampsPlayerCameraManager::ResetCameraState()
{
	for (const auto& FadeEntry : FadedComponents)
	{
		if (UPrimitiveComponent* Occluder = FadeEntry.Key.Get())
		{
			Occluder->SetCustomPrimitiveDataFloat(0, FadeEntry.Value);
		}
	}
	FadedComponents.Reset();
	ObservedTarget.Reset();
	FramingSource.Reset();
	bHasCameraState = false;
	bCombatFraming = false;
}

void AVampsPlayerCameraManager::EndPlay(const EEndPlayReason::Type Reason)
{
	if (PCOwner)
	{
		PCOwner->OnPossessedPawnChanged.RemoveDynamic(this, &AVampsPlayerCameraManager::HandleObservedPawnChanged);
	}
	HandleObservedPawnChanged(nullptr, nullptr);
	Super::EndPlay(Reason);
}

void AVampsPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	const AVampsCharacter* PreviouslyViewedFighter = ObservedFighter.Get();
	if (bHasCameraState && PreviouslyViewedFighter && !PreviouslyViewedFighter->CombatState.bIsAlive &&
	    ObservedGeneration == PreviouslyViewedFighter->CombatGeneration && PCOwner &&
	    PCOwner->IsLocalPlayerController() && PCOwner->GetPawn() == PreviouslyViewedFighter &&
	    OutVT.Target == PreviouslyViewedFighter)
	{
		return;
	}
	Super::UpdateViewTarget(OutVT, DeltaTime);
}

void AVampsPlayerCameraManager::UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime)
{
	constexpr float UnlockedLookAheadTime = 0.5f;
	constexpr float UnlockedAngularSpeed = 1.5f;
	constexpr float UnlockedHeadingResponse = 0.4f;
	constexpr float UnlockedHeadingTurnRate = 60.f;

	AVampsCharacter* ViewingFighter = ObservedFighter.Get();
	if (!PCOwner || !PCOwner->IsLocalPlayerController() || !ViewingFighter || OutVT.Target != ViewingFighter)
	{
		ResetCameraState();
		Super::UpdateViewTargetInternal(OutVT, DeltaTime);
		return;
	}
	const float CameraValues[] = {FollowDistance,
	                              CombatDistance,
	                              FollowEyeHeight,
	                              CombatEyeHeight,
	                              FollowInterestHeight,
	                              CombatInterestHeight,
	                              FollowVerticalFov,
	                              CombatVerticalFov,
	                              CombatInteriorDistance,
	                              CombatYawOffset,
	                              PositionRadius,
	                              AngularSpeed,
	                              TransitionDuration,
	                              DeltaTime};
	for (float CameraValue : CameraValues)
	{
		if (!FMath::IsFinite(CameraValue))
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: '%s': camera update rejected; camera settings and DeltaTime must be finite."),
			       __FUNCTION__,
			       *GetPathName());
			return;
		}
	}
	if (!ViewingFighter->LockOn || FollowDistance <= 12.f || CombatDistance <= 12.f || FollowVerticalFov <= 0.f ||
	    FollowVerticalFov >= 170.f || CombatVerticalFov <= 0.f || CombatVerticalFov >= 170.f || PositionRadius < 0.f ||
	    CombatInteriorDistance < 0.f || AngularSpeed <= 0.f || TransitionDuration < 0.f || DeltaTime < 0.f)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': missing LockOn or invalid camera settings. Distances must exceed 12 cm, FOV must be in "
		            "(0,170), speed positive and radius/interior/duration/time nonnegative."),
		       __FUNCTION__,
		       *ViewingFighter->GetPathName());
		return;
	}
	if (ObservedGeneration != ViewingFighter->CombatGeneration)
	{
		ResetCameraState();
		ObservedGeneration = ViewingFighter->CombatGeneration;
	}
	if (!ViewingFighter->CombatState.bIsAlive)
	{
		FramingSource.Reset();
	}
	AVampsCharacter* ChainSource = FramingSource.Get();
	if (ChainSource)
	{
		const FVampsCombatState& ChainState = ChainSource->CombatState;
		const bool bSameMontage = ChainState.MontageInstanceID == FramingMontageID && FramingMontageID != INDEX_NONE;
		if (!ChainState.bIsAlive || ChainSource->CombatGeneration != FramingGeneration ||
		    ChainState.Phase == EVampsActionPhase::NARecovery || ChainState.Phase == EVampsActionPhase::Idle ||
		    !bSameMontage)
		{
			FramingSource.Reset();
		}
	}
	const bool bNextCombatFraming = FramingSource.IsValid();
	AVampsCharacter* LockedFighter = ViewingFighter->LockOn->GetCurrentTarget();
	if (!IsValid(LockedFighter) || !LockedFighter->CombatState.bIsAlive)
	{
		LockedFighter = nullptr;
	}
	const bool bTargetChanged =
	    LockedFighter ? ObservedTarget.Get() != LockedFighter : !ObservedTarget.IsExplicitlyNull();
	if (!LockedFighter && (!bHasCameraState || bTargetChanged))
	{
		UnlockedHeadingYaw =
		    bHasCameraState
		        ? FRotator::NormalizeAxis(GetCameraRotation().Yaw - (bCombatFraming ? CombatYawOffset : 0.f))
		        : PCOwner->GetControlRotation().Yaw;
	}
	const FVector PlayerAnchor = ViewingFighter->GetActorLocation();
	const FVector TravelVelocity = ViewingFighter->GetVelocity();
	if (!LockedFighter && TravelVelocity.SizeSquared2D() > 1.f)
	{
		const double HeadingError = FMath::FindDeltaAngleDegrees(UnlockedHeadingYaw, TravelVelocity.Rotation().Yaw);
		const double HeadingAlpha = 1.0 - FMath::Exp(-UnlockedHeadingResponse * DeltaTime);
		const double HeadingLimit = UnlockedHeadingTurnRate * DeltaTime;
		UnlockedHeadingYaw = FRotator::NormalizeAxis(
		    UnlockedHeadingYaw + FMath::Clamp(HeadingError * HeadingAlpha, -HeadingLimit, HeadingLimit));
	}
	const FVector TravelLookAhead = (FVector(TravelVelocity.X, TravelVelocity.Y, 0.f) * UnlockedLookAheadTime)
	                                    .GetClampedToMaxSize(FollowDistance * 0.5f);
	const FVector TargetAnchor =
	    LockedFighter
	        ? LockedFighter->GetActorLocation()
	        : PlayerAnchor + FRotator(0.f, UnlockedHeadingYaw, 0.f).Vector() * FollowDistance + TravelLookAhead;
	const FVector SubjectDirection = TargetAnchor - PlayerAnchor;
	const bool bHasSubjectDirection = SubjectDirection.SizeSquared2D() > UE_SMALL_NUMBER;
	const double ReferenceYaw =
	    bHasSubjectDirection ? SubjectDirection.Rotation().Yaw : OrbitalYaw - (bCombatFraming ? CombatYawOffset : 0.f);
	const double DesiredYaw = ReferenceYaw + (bNextCombatFraming ? CombatYawOffset : 0.f);
	const float DesiredFov = bNextCombatFraming ? CombatVerticalFov : FollowVerticalFov;
	const float DesiredDistance = bNextCombatFraming ? CombatDistance : FollowDistance;
	const float EyeHeight = bNextCombatFraming ? CombatEyeHeight : FollowEyeHeight;
	const float InterestHeight = bNextCombatFraming ? CombatInterestHeight : FollowInterestHeight;
	FVector PivotAnchor = PlayerAnchor;
	if (bHasSubjectDirection && bNextCombatFraming)
	{
		const double InteriorWeight = FMath::Clamp(CombatInteriorDistance / SubjectDirection.Size2D(), 0.0, 0.95);
		PivotAnchor.X += SubjectDirection.X * InteriorWeight;
		PivotAnchor.Y += SubjectDirection.Y * InteriorWeight;
	}
	if (bHasCameraState && (bTargetChanged || bCombatFraming != bNextCombatFraming))
	{
		TransitionEye = GetCameraLocation();
		TransitionInterest = InterestPosition;
		TransitionFov = VerticalFov;
		TransitionElapsed = 0.f;
	}
	ObservedTarget = LockedFighter;
	bCombatFraming = bNextCombatFraming;
	if (!bHasCameraState)
	{
		OrbitalYaw = DesiredYaw;
		VerticalFov = DesiredFov;
		TransitionElapsed = TransitionDuration;
	}
	float RemainingTime = DeltaTime;
	do
	{
		const float CameraStep = FMath::Min(RemainingTime, 1.f / 120.f);
		const float AngularAlpha =
		    1.f - FMath::Exp(-(LockedFighter ? AngularSpeed : UnlockedAngularSpeed) * CameraStep);
		OrbitalYaw =
		    FRotator::NormalizeAxis(OrbitalYaw + FMath::FindDeltaAngleDegrees(OrbitalYaw, DesiredYaw) * AngularAlpha);
		const FVector DesiredEye =
		    PivotAnchor - FRotator(0.f, OrbitalYaw, 0.f).Vector() * DesiredDistance + FVector(0.f, 0.f, EyeHeight);
		const FVector DesiredInterest = PivotAnchor + FVector(0.f, 0.f, InterestHeight);
		if (!bHasCameraState)
		{
			EyePosition = DesiredEye;
			InterestPosition = DesiredInterest;
		}
		else if (TransitionElapsed < TransitionDuration)
		{
			TransitionElapsed = FMath::Min(TransitionElapsed + CameraStep, TransitionDuration);
			const float BlendAlpha = 0.5f * (1.f - FMath::Cos(PI * TransitionElapsed / TransitionDuration));
			EyePosition = FMath::Lerp(TransitionEye, DesiredEye, BlendAlpha);
			InterestPosition = FMath::Lerp(TransitionInterest, DesiredInterest, BlendAlpha);
			VerticalFov = FMath::Lerp(TransitionFov, DesiredFov, BlendAlpha);
		}
		else
		{
			EyePosition = SpatialFollow(EyePosition, DesiredEye, CameraStep);
			InterestPosition = SpatialFollow(InterestPosition, DesiredInterest, CameraStep);
			VerticalFov = FMath::Lerp(VerticalFov, DesiredFov, AngularAlpha);
		}
		RemainingTime -= CameraStep;
	} while (RemainingTime > UE_SMALL_NUMBER);
	bHasCameraState = true;
	FVector PublishedEye = EyePosition;
	FCollisionQueryParams CollisionParams(SCENE_QUERY_STAT(VampsCameraCollision), false, ViewingFighter);
	for (const TWeakObjectPtr<AVampsCharacter>& Candidate : ViewingFighter->LockOn->GetCandidates())
	{
		CollisionParams.AddIgnoredActor(Candidate.Get());
	}
	FHitResult CameraHit;
	if (GetWorld()->SweepSingleByChannel(CameraHit,
	                                     PlayerAnchor,
	                                     PublishedEye,
	                                     FQuat::Identity,
	                                     ECC_Camera,
	                                     FCollisionShape::MakeSphere(12.f),
	                                     CollisionParams))
	{
		PublishedEye = CameraHit.Location;
	}
	OutVT.POV.Location = PublishedEye;
	OutVT.POV.Rotation = (InterestPosition - PublishedEye).Rotation();
	OutVT.POV.AspectRatio = 16.f / 9.f;
	OutVT.POV.AspectRatioAxisConstraint = AspectRatio_MaintainYFOV;
	OutVT.POV.bConstrainAspectRatio = false;
	OutVT.POV.FOV = FMath::RadiansToDegrees(
	    2.f * FMath::Atan(FMath::Tan(FMath::DegreesToRadians(VerticalFov * 0.5f)) * OutVT.POV.AspectRatio));
	UpdateOccluders(PublishedEye, *ViewingFighter, LockedFighter, DeltaTime);
}

void AVampsPlayerCameraManager::UpdateOccluders(const FVector& PublishedEye,
                                                const AVampsCharacter& ViewingFighter,
                                                const AVampsCharacter* LockedFighter,
                                                float DeltaTime)
{
	static const FName OccluderTag(TEXT("VampsCameraFade"));
	const FCollisionObjectQueryParams ObjectParams(FCollisionObjectQueryParams::AllObjects);
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(VampsCameraFade), false, &ViewingFighter);
	QueryParams.bFindInitialOverlaps = true;
	if (LockedFighter)
	{
		QueryParams.AddIgnoredActor(LockedFighter);
	}
	TArray<FHitResult> ViewHits;
	TSet<TWeakObjectPtr<UPrimitiveComponent>> CurrentOccluders;
	for (const AVampsCharacter* Subject : {&ViewingFighter, LockedFighter})
	{
		if (!Subject)
		{
			continue;
		}
		const UCapsuleComponent* SubjectCapsule = Subject->GetCapsuleComponent();
		ViewHits.Reset();
		GetWorld()->SweepMultiByObjectType(ViewHits,
		                                   PublishedEye,
		                                   SubjectCapsule->GetComponentLocation(),
		                                   FQuat::Identity,
		                                   ObjectParams,
		                                   SubjectCapsule->GetCollisionShape(-8.f),
		                                   QueryParams);
		for (const FHitResult& ViewHit : ViewHits)
		{
			UPrimitiveComponent* Occluder = ViewHit.GetComponent();
			if (!Occluder || !Occluder->ComponentHasTag(OccluderTag) || CurrentOccluders.Contains(Occluder))
			{
				continue;
			}
			if (Occluder->GetCollisionResponseToChannel(ECC_Camera) != ECR_Ignore ||
			    Occluder->GetCustomPrimitiveDataIndexForScalarParameter(OccluderTag) != 0)
			{
				UE_LOG(
				    LogVampsCombat,
				    Error,
				    TEXT("%hs: '%s': prepared occluder must ignore Camera and expose material scalar VampsCameraFade "
				         "at custom primitive data index 0."),
				    __FUNCTION__,
				    *Occluder->GetPathName());
				continue;
			}
			CurrentOccluders.Add(Occluder);
			if (!FadedComponents.Contains(Occluder))
			{
				const TArray<float>& OriginalData = Occluder->GetCustomPrimitiveData().Data;
				FadedComponents.Add(Occluder, OriginalData.IsValidIndex(0) ? OriginalData[0] : 0.f);
			}
		}
	}
	for (auto FadeIterator = FadedComponents.CreateIterator(); FadeIterator; ++FadeIterator)
	{
		UPrimitiveComponent* Occluder = FadeIterator.Key().Get();
		if (!Occluder)
		{
			FadeIterator.RemoveCurrent();
			continue;
		}
		const bool bOccluded = CurrentOccluders.Contains(Occluder);
		const float FadeTarget = bOccluded ? 1.f : FadeIterator.Value();
		const float OccluderBlendAlpha = 1.f - FMath::Exp(-(bOccluded ? 25.f : 10.f) * DeltaTime);
		const TArray<float>& CurrentData = Occluder->GetCustomPrimitiveData().Data;
		const float CurrentFade = CurrentData.IsValidIndex(0) ? CurrentData[0] : 0.f;
		const float NextFade = FMath::Lerp(CurrentFade, FadeTarget, OccluderBlendAlpha);
		const bool bFadeComplete = FMath::IsNearlyEqual(NextFade, FadeTarget, 0.001f);
		Occluder->SetCustomPrimitiveDataFloat(0, bFadeComplete ? FadeTarget : NextFade);
		if (bFadeComplete && CurrentFade != FadeTarget)
		{
			UE_LOG(LogVampsCombat,
			       VeryVerbose,
			       TEXT("Camera fade reached %.2f for '%s'."),
			       FadeTarget,
			       *Occluder->GetPathName());
		}
		if (bFadeComplete && !bOccluded)
		{
			FadeIterator.RemoveCurrent();
		}
	}
}
