#pragma once

#include "Animation/AnimInstance.h"
#include "VampsAnimInstance.generated.h"

UCLASS(Transient, Blueprintable)
class VAMPS_API UVampsAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	float Speed = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	float Direction = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	bool bInAir = false;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	bool bIsGuarding = false;
};
