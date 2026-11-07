#pragma once

#include "Engine/DataAsset.h"
#include "VampsCombatSettings.generated.h"

class UAnimMontage;
class UBlendSpace;
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "0.001", Units = "s"))
	float ContactReactionDuration = 0.5f;
};

UCLASS(BlueprintType)
class VAMPS_API UVampsCombatSettings : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character")
	FText CharacterName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UBlendSpace> LocomotionBlendSpace;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TArray<FVampsWeaponSettings> Weapons;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "0.001"))
	float NARecoveryDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TArray<FVampsCombatStepSettings> ComboSteps = {{100.f, 60.f}, {100.f, 60.f}, {100.f, 180.f}};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reaction")
	TObjectPtr<UAnimMontage> HitReactionMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reaction")
	TObjectPtr<UAnimMontage> GuardReactionMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death")
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death")
	TObjectPtr<UAnimMontage> GuardDeathMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage", meta = (ClampMin = "0"))
	float AttackDamage = 12.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage", meta = (ClampMin = "0"))
	float CriticalDamage = 24.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage", meta = (ClampMin = "0", ClampMax = "100"))
	float CriticalChancePercent = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard", meta = (ClampMin = "0"))
	float BlockDamageReduction = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard", meta = (ClampMin = "0.001"))
	float GuardCapacity = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard", meta = (ClampMin = "0"))
	float GuardLoadPerHit = 25.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard", meta = (ClampMin = "0", Units = "s"))
	float GuardRecoveryDelay = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard", meta = (ClampMin = "0"))
	float GuardRecoveryPerSecond = 20.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard", meta = (ClampMin = "0.001", Units = "s"))
	float StunDuration = 2.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard", meta = (ClampMin = "0.001", Units = "s"))
	float DefenseRequestDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mobility")
	TObjectPtr<UAnimMontage> ForwardDashMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mobility")
	TObjectPtr<UAnimMontage> BackwardDashMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Approach", meta = (ClampMin = "0", Units = "cm"))
	float ApproachDistance = 250.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Approach", meta = (ClampMin = "0.001", Units = "s"))
	float ApproachDuration = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Approach", meta = (ClampMin = "0", Units = "cm"))
	float ApproachStopDistance = 130.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mobility", meta = (ClampMin = "0.001"))
	float MobilityEnergyCapacity = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mobility", meta = (ClampMin = "0"))
	float DashEnergyCost = 25.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mobility", meta = (ClampMin = "0"))
	float SprintEnergyPerSecond = 20.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mobility", meta = (ClampMin = "0"))
	float MobilityRecoveryPerSecond = 25.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mobility", meta = (ClampMin = "0", Units = "s"))
	float MobilityRecoveryDelay = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mobility", meta = (ClampMin = "0.001", Units = "cm/s"))
	float SprintSpeed = 700.f;

	bool HasValidReactionSettings() const;
	bool HasValidMobilitySettings() const;
	bool HasValidDeathMontages() const;
};
