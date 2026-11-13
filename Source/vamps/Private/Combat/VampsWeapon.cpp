#include "Combat/VampsWeapon.h"
#include "Characters/VampsCharacter.h"
#include "Combat/VampsCombatLog.h"
#include "Components/BoxComponent.h"
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

	TraceBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TraceBox"));
	TraceBox->SetupAttachment(WeaponRoot);
	TraceBox->InitBoxExtent(FVector(8.f));
	TraceBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TraceBox->SetGenerateOverlapEvents(false);
	TraceBox->SetCanEverAffectNavigation(false);
}

bool AVampsWeapon::InitializeCombatTrace(AVampsCharacter& Fighter)
{
	OwningFighter = &Fighter;
	AddTickPrerequisiteComponent(Fighter.GetMesh());
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
}

void AVampsWeapon::SetTracingEnabled(bool bEnabled)
{
	SetActorTickEnabled(bEnabled);
}

void AVampsWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	AVampsCharacter* Fighter = OwningFighter.Get();

	if (!Fighter)
	{
		SetTracingEnabled(false);
		return;
	}

	FCollisionQueryParams Query(SCENE_QUERY_STAT(WeaponHit), false, Fighter);

	for (AVampsWeapon* EquippedWeapon : Fighter->Weapons)
	{
		Query.AddIgnoredActor(EquippedWeapon);
	}

	TArray<FHitResult> Hits;

	GetWorld()->SweepMultiByObjectType(Hits,
	                                   TraceStart->GetComponentLocation(),
	                                   TraceEnd->GetComponentLocation(),
	                                   TraceBox->GetComponentQuat(),
	                                   FCollisionObjectQueryParams(ECC_Pawn),
	                                   TraceBox->GetCollisionShape(),
	                                   Query);

	for (const FHitResult& Hit : Hits)
	{
		AVampsCharacter* Target = Cast<AVampsCharacter>(Hit.GetActor());

		if (HitActors.Contains(Target) || !Fighter->CanHitCombatTarget(Target))
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
		Fighter->QueueWeaponContact(*Target);
	}
}
