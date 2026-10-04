#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CombatConfig.generated.h"

class UAnimMontage;

UENUM(BlueprintType)
enum class EDuelPhase : uint8
{
	Default,
	Parry,
	Buildup,
	Active,
	Recovery,
	Guard,
	HitReaction,
	NA,
	Defeated
};

UENUM(BlueprintType)
enum class EDuelResult : uint8
{
	None,
	Hit,
	Block,
	Parry
};

UENUM(BlueprintType)
enum class EDuelAIMode : uint8
{
	Passive,
	Combo,
	Reactive,
	Sidestep,
	Mixed
};

USTRUCT(BlueprintType)
struct FDuelStrike
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.01"))
	float Buildup = 0.17f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.01"))
	float Active = 0.10f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.01"))
	float Continuation = 0.22f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0"))
	float Damage = 12.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1"))
	float Reach = 155.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1"))
	float Radius = 28.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName PrepareSection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ActiveSection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName RecoverySection;
};

/** All timing values are seconds; distances are centimeters. The same asset configures both fighters. */
UCLASS(BlueprintType)
class VAMPS_API UCombatConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	UCombatConfig();

	// Attack data and presentation assets.
	UPROPERTY(EditAnywhere, EditFixedSize, BlueprintReadOnly, Category = "Attacks")
	TArray<FDuelStrike> Strikes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attacks")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
	TObjectPtr<UAnimMontage> GuardMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
	TObjectPtr<UAnimMontage> HitMontage;

	// Timing windows are authoritative; animation only follows them.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timing", meta = (ClampMin = "0"))
	float InitialParry = 0.08f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timing", meta = (ClampMin = "0"))
	float OffensiveNA = 0.45f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timing", meta = (ClampMin = "0"))
	float DefensiveNA = 0.18f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timing", meta = (ClampMin = "0"))
	float NAResumeDelay = 0.20f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timing", meta = (ClampMin = "0.01"))
	float InputBuffer = 0.14f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timing", meta = (ClampMin = "0.01"))
	float GuardWindow = 0.22f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timing", meta = (ClampMin = "0.01"))
	float GuardContinuation = 0.42f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timing", meta = (ClampMin = "0"))
	float HitLock = 0.14f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timing", meta = (ClampMin = "0.01"))
	float HitResponse = 0.18f;

	// Defense, movement and contact response.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defense", meta = (ClampMin = "0", ClampMax = "360"))
	float DefenseAngle = 180.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "0"))
	float InitialApproach = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "0", ClampMax = "180"))
	float InitialTurnLimit = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "0"))
	float MoveSpeed = 420.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "0"))
	float HitPushback = 24.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "0"))
	float BlockPushback = 12.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "0"))
	float ParryPushback = 18.f;

	// Health and diagnostics.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", meta = (ClampMin = "1"))
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
	bool bDrawCombat = true;
	const FDuelStrike& Strike(int32 Index) const;
};
