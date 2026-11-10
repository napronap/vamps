#include "Animation/VampsAnimInstance.h"
#include "Animation/BlendSpace.h"
#include "Characters/VampsCharacter.h"
#include "Combat/Data/VampsCombatSettings.h"

void UVampsAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// apparently this reassignment to nullptr is very important
	LocomotionBlendSpace = nullptr;

	if (const AVampsCharacter* Fighter = Cast<AVampsCharacter>(GetOwningActor()); Fighter && Fighter->CombatSettings)
	{
		LocomotionBlendSpace = Fighter->CombatSettings->LocomotionBlendSpace;
	}
}

void UVampsAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (const AVampsCharacter* Fighter = Cast<AVampsCharacter>(GetOwningActor()))
	{
		Speed = Fighter->GetVelocity().Size2D();
	}
}
