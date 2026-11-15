#include "Player/VampsPlayerCameraManager.h"
#include "Characters/VampsCharacter.h"
#include "Characters/VampsLockOnComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameModes/VampsMatchGameMode.h"
FVector AVampsPlayerCameraManager::SpatialFollow(const FVector& PreviousPosition,
                                                 const FVector& DesiredPosition,
                                                 float DeltaTime)
{
	const float Distance = FVector::Distance(PreviousPosition, DesiredPosition);
	const float ReferenceAlpha = FMath::Square(FMath::Min(Distance / PositionRadius, 1.f));
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
		PreviousFighter->OnCombatReset.RemoveAll(this);
	}
	ResetCameraState();
	AVampsCharacter* ViewingFighter = Cast<AVampsCharacter>(NewPawn);
	ObservedFighter = ViewingFighter;
	if (ViewingFighter)
	{
		ViewingFighter->OnCombatFramingRequested.AddUObject(this,
		                                                    &AVampsPlayerCameraManager::HandleCombatFramingRequested);
		ViewingFighter->OnCombatReset.AddUObject(this, &AVampsPlayerCameraManager::ResetCameraState);
	}
}

void AVampsPlayerCameraManager::HandleCombatFramingRequested(AVampsCharacter* ChainSource)
{
	if (!PCOwner || !PCOwner->IsLocalPlayerController())
	{
		return;
	}
	if (!ChainSource)
	{
		return;
	}
	FramingSource = ChainSource;
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
	FramingMontageID = INDEX_NONE;
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
	if (bHasCameraState && PreviouslyViewedFighter && !PreviouslyViewedFighter->CombatState.bIsAlive && PCOwner &&
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

	if (!ViewingFighter->CombatState.bIsAlive)
	{
		FramingSource.Reset();
	}
	AVampsCharacter* ChainSource = FramingSource.Get();
	if (ChainSource)
	{
		const FVampsCombatState& ChainState = ChainSource->CombatState;
		if (!ChainState.bIsAlive || FramingMontageID == INDEX_NONE || ChainState.MontageInstanceID != FramingMontageID)
		{
			FramingSource.Reset();
		}
	}
	const bool bNextCombatFraming = FramingSource.IsValid();
	AVampsCharacter* LockedFighter = ViewingFighter->LockOn->GetCurrentTarget();
	const bool bTargetChanged =
	    !ObservedTarget.HasSameIndexAndSerialNumber(TWeakObjectPtr<AVampsCharacter>(LockedFighter));
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
	if (const AVampsMatchGameMode* MatchMode = GetWorld()->GetAuthGameMode<AVampsMatchGameMode>())
	{
		for (const FVampsMatchParticipant& Participant : MatchMode->Participants)
		{
			CollisionParams.AddIgnoredActor(Participant.Fighter.Get());
		}
	}
	else
	{
		for (const TWeakObjectPtr<AVampsCharacter>& Candidate : ViewingFighter->LockOn->GetCandidates())
		{
			CollisionParams.AddIgnoredActor(Candidate.Get());
		}
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
	QueryParams.AddIgnoredActor(LockedFighter);
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
		if (bFadeComplete && !bOccluded)
		{
			FadeIterator.RemoveCurrent();
		}
	}
}
