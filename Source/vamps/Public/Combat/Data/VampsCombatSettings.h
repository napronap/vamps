#pragma once

#include "Engine/DataAsset.h"
#include "VampsCombatSettings.generated.h"

class UAnimMontage;
class AVampsWeapon;

USTRUCT(BlueprintType)
struct VAMPS_API FVampsWeaponSettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AVampsWeapon> WeaponClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName AttachSocket = TEXT("hand_r");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FTransform RelativeTransform = FTransform::Identity;
};

USTRUCT(BlueprintType)
struct VAMPS_API FVampsCombatStepSettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (Units = "cm"))
	float TargetAlignmentDistance = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (Units = "cm"))
	float KnockbackDistance = 60.f;

	UPROPERTY(EditDefaultsOnly,
	          BlueprintReadOnly,
	          Category = "Combat",
	          meta = (ClampMin = "0.0",
	                  Units = "s",
	                  ToolTip = "Extra seconds added to the attack section duration after a blocked hit."))
	float AdditionalOffensiveRestrictionDuration = 0.f;
};

UCLASS(BlueprintType)
class VAMPS_API UVampsCombatSettings : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TArray<FVampsWeaponSettings> Weapons;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "0.001"))
	float NARecoveryDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TArray<FVampsCombatStepSettings> ComboSteps = {{100.f, 60.f}, {100.f, 60.f}, {100.f, 180.f}};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> HitReactionMontage;
};
