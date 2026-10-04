#include "Animation/DuelAnimInstance.h"
#include "Characters/VampsDuelCharacter.h"
#include "Combat/Abilities/GA_Attack.h"
#include "GameFramework/CharacterMovementComponent.h"

void UDuelAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (const AVampsDuelCharacter* Fighter = Cast<AVampsDuelCharacter>(TryGetPawnOwner()))
	{
		Speed = Fighter->GetVelocity().Size2D();
		Direction = Speed > 1.f ? FMath::FindDeltaAngleDegrees(Fighter->GetActorRotation().Yaw,
		                                                       Fighter->GetVelocity().Rotation().Yaw)
		                        : 0.f;
		bInAir = Fighter->GetCharacterMovement()->IsFalling();
		Phase = Fighter->GetPhase();
		StrikeIndex = Fighter->Attack && Fighter->Attack->IsActive() ? Fighter->Attack->StrikeIndex : 0;
	}
}
