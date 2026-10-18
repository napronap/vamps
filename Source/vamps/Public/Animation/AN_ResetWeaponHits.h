#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_ResetWeaponHits.generated.h"

UCLASS(meta = (DisplayName = "Reset Weapon Hits"))
class VAMPS_API UAN_ResetWeaponHits : public UAnimNotify
{
	GENERATED_BODY()

public:

	UAN_ResetWeaponHits();
	virtual void BranchingPointNotify(FBranchingPointNotifyPayload& Payload) override;

	UPROPERTY(EditAnywhere, Category = "Combat", meta = (ClampMin = "0"))
	TArray<int32> WeaponIndices = {0};
};
