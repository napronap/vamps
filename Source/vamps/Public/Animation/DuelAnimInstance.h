#pragma once

#include "Animation/AnimInstance.h"
#include "DuelAnimInstance.generated.h"

/** Locomotion and guard presentation; no independent combat state. */
UCLASS(Transient, Blueprintable)
class VAMPS_API UDuelAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly, Category = "Duel")
	float Speed = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Duel")
	float Direction = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Duel")
	bool bInAir = false;

	UPROPERTY(BlueprintReadOnly, Category = "Duel")
	bool bIsGuarding = false;
};
