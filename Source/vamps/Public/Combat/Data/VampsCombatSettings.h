#pragma once

#include "Engine/DataAsset.h"
#include "VampsCombatSettings.generated.h"

class UAnimMontage;
class UBlendSpace;
class UCameraShakeBase;
class UNiagaraSystem;
class AVampsWeapon;

USTRUCT(BlueprintType)
struct VAMPS_API FVampsWeaponSettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AVampsWeapon> WeaponClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName AttachSocket = TEXT("hand_r");
};

// how contacts from one attack montage section affect the recipient
USTRUCT(BlueprintType)
struct VAMPS_API FVampsCombatStepSettings
{
	GENERATED_BODY()

	// On an unblocked hit, align the target this many cm along the attack direction from the attack origin before knockback
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (Units = "cm"))
	float TargetAlignmentDistance = 100.f;

	// Push distance in cm along the attack direction
	// Blocked hits apply knockback immediately, skipping alignment
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (Units = "cm"))
	float KnockbackDistance = 60.f;

	// Seconds that hit or guard restricts the recipient's movement and attacks
	// this is very important to allow the attacker to comfortably continue combos
	// since you can't predict when within the trace window the attacker will connect, the time shuold be something like
	// end of attack trace (start recovery) ~ end if combo window + start of next animation (preparation) ~ start of traces
	// it will still give the recipient some leeway to dash or move away but it will mostly guarantee the attacker can continue combo
	//
	// |------- Attack 1 ----------||---------Attack 2 ---------|
	//								|
	//				______________________________ this much time plus a bit more
	//             |				|	  |
	// Traces ---->|				|     |<----Traces------>
	// <------- Combo window ------>|         <-------Combo window ----->
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "0.001", Units = "s"))
	float ContactReactionDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "0", Units = "s"))
	float HitstopDuration = 0.07f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<UCameraShakeBase> ContactCameraShake;
};

// Per-fighter setup for equipment, animation, attacks, guard, and mobility.
UCLASS(BlueprintType)
class VAMPS_API UVampsCombatSettings : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character")
	FText CharacterName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UBlendSpace> LocomotionBlendSpace;

	// weapons to spawn and attach, in the order used by attack notifies that reference weapon indices (AN_ResetWeaponHits)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TArray<FVampsWeaponSettings> Weapons;

	// montage played for the entire attack combo, with one section per step. Disable Auto Blend Out
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> AttackMontage;

	// NA time after recovery ends
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "0.001"))
	float NARecoveryDuration = 0.5f;

	// One entry per AttackMontage section, in the same order as the montage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TArray<FVampsCombatStepSettings> ComboSteps = {{100.f, 60.f}, {100.f, 60.f}, {100.f, 180.f}};

	// no block hit reaction
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reaction")
	TObjectPtr<UAnimMontage> HitReactionMontage;

	// block hit reaction
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reaction")
	TObjectPtr<UAnimMontage> GuardReactionMontage;

	// death montage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death")
	TObjectPtr<UAnimMontage> DeathMontage;

	// death while blocking
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death")
	TObjectPtr<UAnimMontage> GuardDeathMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Impact")
	TObjectPtr<UNiagaraSystem> HitEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Impact")
	TObjectPtr<UNiagaraSystem> GuardEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Impact")
	TObjectPtr<UNiagaraSystem> GuardBreakEffect;

	// damage by non critical before calculations
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage", meta = (ClampMin = "0"))
	float AttackDamage = 12.f;

	// damage by critical hit
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage", meta = (ClampMin = "0"))
	float CriticalDamage = 24.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage", meta = (ClampMin = "0", ClampMax = "100"))
	float CriticalChancePercent = 10.f;

	// flat amount
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard", meta = (ClampMin = "0"))
	float BlockDamageReduction = 10.f;

	// max guard load
	// when guard load hits max, you get stunned
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard", meta = (ClampMin = "0.001"))
	float GuardCapacity = 100.f;

	// how much guard load this fighter deals per hit
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard", meta = (ClampMin = "0"))
	float GuardLoadPerHit = 25.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard", meta = (ClampMin = "0", Units = "s"))
	float GuardRecoveryDelay = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard", meta = (ClampMin = "0"))
	float GuardRecoveryPerSecond = 20.f;

	// how many seconds this specific fighter gets stunned
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard", meta = (ClampMin = "0.001", Units = "s"))
	float StunDuration = 2.f;

	// how long does an armed defense last for
	// this is useful when mashing RMB while already defending, since this will be the amount of time before a request resets or is consumed
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Guard", meta = (ClampMin = "0.001", Units = "s"))
	float DefenseRequestDuration = 0.5f;

	// requires Root Motion and Auto Blend Out
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mobility")
	TObjectPtr<UAnimMontage> ForwardDashMontage;

	// requires Root Motion and Auto Blend Out
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mobility")
	TObjectPtr<UAnimMontage> BackwardDashMontage;

	// max horizontal distance a fighter jumps when performing the first attack
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Approach", meta = (ClampMin = "0", Units = "cm"))
	float ApproachDistance = 250.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Approach", meta = (ClampMin = "0.001", Units = "s"))
	float ApproachDuration = 0.25f;

	// a target already within this gap causes no advance
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Approach", meta = (ClampMin = "0", Units = "cm"))
	float ApproachStopDistance = 130.f;

	// max mobility energy (sprint and dash)
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
};
