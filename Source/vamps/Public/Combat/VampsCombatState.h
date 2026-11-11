#pragma once

#include "CoreMinimal.h"
#include "VampsCombatState.generated.h"

UENUM(BlueprintType)
enum class EVampsCombatReaction : uint8
{
	None,
	Guard,
	Hit,
	Stun
};

UENUM(BlueprintType)
enum class EVampsActionPhase : uint8
{
	Idle,
	Preparation,
	Contacts,
	Recovery,
	NARecovery
};

USTRUCT(BlueprintType)
struct VAMPS_API FVampsCombatState
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	EVampsActionPhase Phase = EVampsActionPhase::Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsAlive = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bCanRequestCombat = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bCanStartAction = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bCanDealDamage = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bCanDefend = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 ComboStep = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bComboWindowOpen = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bContinuationRequested = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bOffenseRestricted = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	EVampsCombatReaction Reaction = EVampsCombatReaction::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bCanMove = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mobility")
	bool bIsDashing = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mobility")
	bool bIsSprinting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mobility")
	bool bDashRequested = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bDefenseRequestArmed = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 MontageInstanceID = INDEX_NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 ActionNumber = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bActionConnected = false;
};
