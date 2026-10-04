#pragma once

#include "Animation/AnimInstance.h"
#include "Combat/Data/CombatConfig.h"
#include "DuelAnimInstance.generated.h"

/** Presentation data for a new AnimBP. No notify participates in combat decisions. */
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
	EDuelPhase Phase = EDuelPhase::Default;

	UPROPERTY(BlueprintReadOnly, Category = "Duel")
	int32 StrikeIndex = 0;
};
