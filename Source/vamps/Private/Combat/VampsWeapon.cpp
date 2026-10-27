#include "Combat/VampsWeapon.h"
#include "Characters/VampsCharacter.h"
#include "Combat/VampsCombatResolver.h"
#include "Combat/VampsCombatLog.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

AVampsWeapon::AVampsWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.TickGroup = TG_PostPhysics;
	WeaponRoot = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));
	SetRootComponent(WeaponRoot);
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(WeaponRoot);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetGenerateOverlapEvents(false);
	WeaponMesh->SetCanEverAffectNavigation(false);
	TraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("TraceStart"));
	TraceStart->SetupAttachment(WeaponMesh);
	TraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("TraceEnd"));
	TraceEnd->SetupAttachment(WeaponMesh);
	TraceEnd->SetRelativeLocation(FVector(100.f, 0.f, 0.f));
}

void AVampsWeapon::BeginPlay()
{
	Super::BeginPlay();
	if (!ValidateTraceConfiguration())
	{
		Destroy();
		return;
	}
	const AVampsCharacter* Fighter = Cast<AVampsCharacter>(GetOwner());
	AddTickPrerequisiteComponent(Fighter->GetMesh());
}

bool AVampsWeapon::ValidateTraceConfiguration() const
{
	if (!IsValid(Cast<AVampsCharacter>(GetOwner())))
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: weapon owner '%s' must be a valid VampsCharacter."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(GetOwner()));
		return false;
	}
	if (!WeaponMesh || !WeaponMesh->GetStaticMesh() || !TraceStart || !TraceEnd)
	{
		UE_LOG(
		    LogVampsCombat,
		    Error,
		    TEXT("%hs: %s: weapon requires a static mesh, TraceStart and TraceEnd; mesh='%s', start='%s', end='%s'."),
		    __FUNCTION__,
		    *GetPathName(),
		    *GetPathNameSafe(WeaponMesh),
		    *GetPathNameSafe(TraceStart),
		    *GetPathNameSafe(TraceEnd));
		return false;
	}
	if (!FMath::IsFinite(TraceRadius) || TraceRadius <= 0.f || TraceStart->GetComponentLocation().ContainsNaN() ||
	    TraceEnd->GetComponentLocation().ContainsNaN() ||
	    TraceStart->GetComponentLocation().Equals(TraceEnd->GetComponentLocation()))
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: trace requires finite distinct endpoints and a positive finite radius; start=%s, end=%s, "
		            "radius=%f."),
		       __FUNCTION__,
		       *GetPathName(),
		       *TraceStart->GetComponentLocation().ToString(),
		       *TraceEnd->GetComponentLocation().ToString(),
		       TraceRadius);
		return false;
	}
	return true;
}

void AVampsWeapon::ResetSectionHits()
{
	SetTracingEnabled(false);
	ResetImpactHits();
}

void AVampsWeapon::ResetImpactHits()
{
	HitActors.Reset();
	bHasPreviousTrace = false;
}

bool AVampsWeapon::SetTracingEnabled(bool bEnabled)
{
	bHasPreviousTrace = false;
	if (bEnabled && !ValidateTraceConfiguration())
	{
		SetActorTickEnabled(false);
		return false;
	}
	SetActorTickEnabled(bEnabled && HasAuthority());
	return true;
}

void AVampsWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	AVampsCharacter* Fighter = Cast<AVampsCharacter>(GetOwner());
	if (!IsValid(Fighter))
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: tracing stopped because weapon owner '%s' is not a valid VampsCharacter."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(GetOwner()));
		SetTracingEnabled(false);
		return;
	}
	if (!HasAuthority() || !Fighter->CombatState.bCanDealDamage)
	{
		SetTracingEnabled(false);
		return;
	}

	FCollisionQueryParams Query(SCENE_QUERY_STAT(WeaponHit), false, Fighter);
	for (AVampsWeapon* EquippedWeapon : Fighter->Weapons)
	{
		Query.AddIgnoredActor(EquippedWeapon);
	}
	const FTransform CurrentTransform = GetActorTransform();
	const FVector LocalStart = CurrentTransform.InverseTransformPosition(TraceStart->GetComponentLocation());
	const FVector LocalEnd = CurrentTransform.InverseTransformPosition(TraceEnd->GetComponentLocation());
	const float MaximumScale = CurrentTransform.GetScale3D().GetAbsMax();
	const float Radius = TraceRadius * MaximumScale;
	if (!FMath::IsFinite(Radius) || Radius <= 0.f)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: tracing stopped; scaled radius must be finite and positive, got %f (TraceRadius=%f, "
		            "scale=%s)."),
		       __FUNCTION__,
		       *GetPathName(),
		       Radius,
		       TraceRadius,
		       *CurrentTransform.GetScale3D().ToString());
		SetTracingEnabled(false);
		return;
	}
	SweepWeaponSegment(CurrentTransform.TransformPosition(LocalStart),
	                   CurrentTransform.TransformPosition(LocalEnd),
	                   Radius,
	                   Fighter,
	                   Query);
	if (bHasPreviousTrace)
	{
		const float MaximumReach = FMath::Max(LocalStart.Size(), LocalEnd.Size()) *
		                           FMath::Max(MaximumScale, PreviousTraceTransform.GetScale3D().GetAbsMax());
		const float RotationAngle =
		    PreviousTraceTransform.GetRotation().AngularDistance(CurrentTransform.GetRotation());
		const float MaximumStepAngle = 2.f * FMath::Acos(FMath::Clamp(1.f - Radius * 0.25f / MaximumReach, -1.f, 1.f));
		if (!FMath::IsFinite(MaximumStepAngle) || MaximumStepAngle <= 0.f)
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: %s: tracing stopped; radius %f cannot resolve weapon reach %f (angular step=%f)."),
			       __FUNCTION__,
			       *GetPathName(),
			       Radius,
			       MaximumReach,
			       MaximumStepAngle);
			SetTracingEnabled(false);
			return;
		}
		const int32 RotationSteps = FMath::Max(1, FMath::CeilToInt(RotationAngle / MaximumStepAngle));
		const float MaximumLength = FVector::Distance(LocalStart, LocalEnd) *
		                            FMath::Max(MaximumScale, PreviousTraceTransform.GetScale3D().GetAbsMax());
		const int32 BladeSteps = FMath::Max(1, FMath::CeilToInt(MaximumLength / Radius));
		FTransform SegmentStartTransform = PreviousTraceTransform;
		for (int32 RotationIndex = 1; RotationIndex <= RotationSteps; ++RotationIndex)
		{
			const float Fraction = float(RotationIndex) / RotationSteps;
			const FTransform SegmentEndTransform(
			    FQuat::Slerp(PreviousTraceTransform.GetRotation(), CurrentTransform.GetRotation(), Fraction),
			    FMath::Lerp(PreviousTraceTransform.GetLocation(), CurrentTransform.GetLocation(), Fraction),
			    FMath::Lerp(PreviousTraceTransform.GetScale3D(), CurrentTransform.GetScale3D(), Fraction));
			for (int32 BladeIndex = 0; BladeIndex <= BladeSteps; ++BladeIndex)
			{
				const FVector LocalPoint = FMath::Lerp(LocalStart, LocalEnd, float(BladeIndex) / BladeSteps);
				SweepWeaponSegment(SegmentStartTransform.TransformPosition(LocalPoint),
				                   SegmentEndTransform.TransformPosition(LocalPoint),
				                   Radius,
				                   Fighter,
				                   Query);
			}
			SegmentStartTransform = SegmentEndTransform;
		}
	}
	PreviousTraceTransform = CurrentTransform;
	bHasPreviousTrace = true;
}

void AVampsWeapon::SweepWeaponSegment(const FVector& SweepStart,
                                      const FVector& SweepEnd,
                                      float Radius,
                                      AVampsCharacter* Fighter,
                                      const FCollisionQueryParams& Query)
{
	TArray<FHitResult> Hits;
	GetWorld()->SweepMultiByObjectType(Hits,
	                                   SweepStart,
	                                   SweepEnd,
	                                   FQuat::Identity,
	                                   FCollisionObjectQueryParams(ECC_Pawn),
	                                   FCollisionShape::MakeSphere(Radius),
	                                   Query);
	for (const FHitResult& Hit : Hits)
	{
		AVampsCharacter* Target = Cast<AVampsCharacter>(Hit.GetActor());
		if (!Fighter->CanHitCombatTarget(Target) || HitActors.Contains(Target))
		{
			continue;
		}
		FCollisionQueryParams WallQuery = Query;
		WallQuery.AddIgnoredActor(Target);
		FHitResult Wall;
		if (GetWorld()->LineTraceSingleByChannel(
		        Wall, Fighter->GetActorLocation(), Target->GetActorLocation(), ECC_Visibility, WallQuery))
		{
			continue;
		}
		HitActors.Add(Target);
		GetWorld()->GetSubsystem<UVampsCombatResolver>()->QueueCombatContact(Fighter, Target);
	}
}
