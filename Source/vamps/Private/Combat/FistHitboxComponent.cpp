#include "Combat/FistHitboxComponent.h"
#include "Characters/VampsCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

UFistHitboxComponent::UFistHitboxComponent()
{
	InitSphereRadius(12.f);
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetGenerateOverlapEvents(false);
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickGroup = TG_PostPhysics;
}

void UFistHitboxComponent::BeginPlay()
{
	Super::BeginPlay();
	AVampsCharacter* Fighter = CastChecked<AVampsCharacter>(GetOwner());
	checkf(HasFistSocket(), TEXT("%s: missing fist socket '%s'."), *Fighter->GetName(), *SocketName.ToString());
	AttachToComponent(Fighter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
	AddTickPrerequisiteComponent(Fighter->GetMesh());
}

bool UFistHitboxComponent::HasFistSocket() const
{
	const AVampsCharacter* Fighter = CastChecked<AVampsCharacter>(GetOwner());
	return Fighter->GetMesh()->DoesSocketExist(SocketName);
}

void UFistHitboxComponent::ResetHits()
{
	HitActors.Reset();
}

void UFistHitboxComponent::StartTracing()
{
	bHasPreviousPosition = false;
	SetComponentTickEnabled(GetOwner()->HasAuthority() && HasFistSocket());
}

void UFistHitboxComponent::StopTracing()
{
	SetComponentTickEnabled(false);
	bHasPreviousPosition = false;
}

void UFistHitboxComponent::TickComponent(float DeltaTime,
                                         ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	AVampsCharacter* Fighter = CastChecked<AVampsCharacter>(GetOwner());
	if (!Fighter->HasAuthority() || !Fighter->IsAttacking())
	{
		StopTracing();
		return;
	}

	const FVector Position = Fighter->GetMesh()->GetSocketLocation(SocketName);
	// The first sample checks the current fist; later samples sweep its movement.
	TraceHitbox(bHasPreviousPosition ? PreviousPosition : Position, Position, Fighter);
	PreviousPosition = Position;
	bHasPreviousPosition = true;
}

void UFistHitboxComponent::TraceHitbox(const FVector& Start, const FVector& End, AVampsCharacter* Fighter)
{
	FCollisionQueryParams Query(SCENE_QUERY_STAT(FistHit), false, Fighter);
	TArray<FHitResult> Hits;
	GetWorld()->SweepMultiByObjectType(Hits,
	                                   Start,
	                                   End,
	                                   FQuat::Identity,
	                                   FCollisionObjectQueryParams(ECC_Pawn),
	                                   FCollisionShape::MakeSphere(FMath::Max(1.f, GetScaledSphereRadius())),
	                                   Query);

	for (const FHitResult& Hit : Hits)
	{
		if (!IsComponentTickEnabled())
		{
			break;
		}
		AVampsCharacter* Target = Cast<AVampsCharacter>(Hit.GetActor());
		if (!Target || HitActors.Contains(Target))
		{
			continue;
		}

		FCollisionQueryParams WallQuery(SCENE_QUERY_STAT(FistWall), false, Fighter);
		WallQuery.AddIgnoredActor(Target);
		FHitResult Wall;
		if (GetWorld()->LineTraceSingleByChannel(
		        Wall, Fighter->GetActorLocation(), Target->GetActorLocation(), ECC_Visibility, WallQuery))
		{
			continue;
		}

		HitActors.Add(Target);
		Target->ReceiveHit(Fighter);
	}
}
