#include "Animation/DuelAnimInstance.h"
#include "Characters/VampsCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UDuelAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (const AVampsCharacter* Fighter = Cast<AVampsCharacter>(TryGetPawnOwner()))
	{
		Speed = Fighter->GetVelocity().Size2D();
		Direction = Speed > 1.f ? FMath::FindDeltaAngleDegrees(Fighter->GetActorRotation().Yaw,
		                                                       Fighter->GetVelocity().Rotation().Yaw)
		                        : 0.f;
		bInAir = Fighter->GetCharacterMovement()->IsFalling();
		bIsGuarding = Fighter->IsGuarding();
	}
}
