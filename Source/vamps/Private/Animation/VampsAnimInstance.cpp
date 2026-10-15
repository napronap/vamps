#include "Animation/VampsAnimInstance.h"
#include "Characters/VampsCharacter.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"

void UVampsAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	APawn* OwningPawn = TryGetPawnOwner();
	if (!OwningPawn || !GetWorld()->IsGameWorld())
	{
		return;
	}

	const AVampsCharacter* OwningCharacter = CastChecked<AVampsCharacter>(OwningPawn);
	const FVector Velocity = OwningCharacter->GetVelocity();
	Speed = Velocity.Size2D();
	Direction = Speed > 1.f
	                ? FMath::FindDeltaAngleDegrees(OwningCharacter->GetActorRotation().Yaw, Velocity.Rotation().Yaw)
	                : 0.f;
	bInAir = OwningCharacter->GetCharacterMovement()->IsFalling();
	bIsGuarding = OwningCharacter->CombatState.bCanDefend;
}
