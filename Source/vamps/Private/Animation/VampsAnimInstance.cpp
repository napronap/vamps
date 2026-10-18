#include "Animation/VampsAnimInstance.h"
#include "Characters/VampsCharacter.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Combat/VampsCombatLog.h"

void UVampsAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	APawn* OwningPawn = TryGetPawnOwner();
	if (!OwningPawn || !GetWorld()->IsGameWorld())
	{
		return;
	}

	const AVampsCharacter* OwningCharacter = Cast<AVampsCharacter>(OwningPawn);
	if (!OwningCharacter || !OwningCharacter->GetCharacterMovement())
	{
		UE_LOG(
		    LogVampsCombat,
		    Error,
		    TEXT("%hs: %s: animation update rejected; pawn '%s' must be a VampsCharacter with a movement component."),
		    __FUNCTION__,
		    *GetPathName(),
		    *GetPathNameSafe(OwningPawn));
		return;
	}
	const FVector Velocity = OwningCharacter->GetVelocity();
	Speed = Velocity.Size2D();
	Direction = Speed > 1.f
	                ? FMath::FindDeltaAngleDegrees(OwningCharacter->GetActorRotation().Yaw, Velocity.Rotation().Yaw)
	                : 0.f;
	bInAir = OwningCharacter->GetCharacterMovement()->IsFalling();
	bIsGuarding = OwningCharacter->CombatState.bCanDefend;
}
