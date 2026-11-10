#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_ResetWeaponHits.generated.h"

// will reset weapon hits for the weapons provided in WeaponIndices, so that they can hit more than once per montage section
UCLASS(meta = (DisplayName = "Reset Weapon Hits"))
class VAMPS_API UAN_ResetWeaponHits : public UAnimNotify
{
	GENERATED_BODY()

public:

	UAN_ResetWeaponHits();
	virtual void BranchingPointNotify(FBranchingPointNotifyPayload& Payload) override;

	// it's the index of the weapon on the data asset!!
	UPROPERTY(EditAnywhere, Category = "Combat", meta = (ClampMin = "0"))
	TArray<int32> WeaponIndices = {0};
};
