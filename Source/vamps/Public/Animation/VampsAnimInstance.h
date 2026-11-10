#pragma once

#include "Animation/AnimInstance.h"
#include "VampsAnimInstance.generated.h"

class UBlendSpace;

UCLASS(Transient, Blueprintable)
class VAMPS_API UVampsAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly, Transient)
	TObjectPtr<UBlendSpace> LocomotionBlendSpace;

	UPROPERTY(BlueprintReadOnly)
	float Speed = 0;
};
