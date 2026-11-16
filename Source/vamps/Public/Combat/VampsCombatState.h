#pragma once

#include "CoreMinimal.h"
#include "VampsCombatState.generated.h"

// fighter's reaction, the animation that is playing
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

	// attack step BEFORE weapon traces
	Preparation,

	// weapon traces window open
	Contacts,

	// after weapon traces
	// inputs in this state can still register combo continuations and, if any, the next attack will start at the end of this state
	// |Time---------------------------------------------------->
	//                                 ______
	//                                |     |----> this part is Recovery
	// <------ Contacts ------------->|     |
	//     < --- Combo window --------------> <----- NA ----- >
	Recovery,

	// NA or "No act", the status after finishing recovery that doesn't allow the fighter to attack or defend
	NARecovery
};

// fighter's current combat and mobility state
USTRUCT(BlueprintType)
struct VAMPS_API FVampsCombatState
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	EVampsActionPhase Phase = EVampsActionPhase::Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsAlive = true;

	// whether regular attack input can be considered for a new attack or a combo continuation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bCanRequestCombat = true;

	// whether the fighter can start a fresh attack from idle
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bCanStartAction = true;

	// whether weapon traces are enabled (read by the weapon)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bCanDealDamage = false;

	// whether the resolver can consume the armed defense request to block an incoming contact
	// (activated when the attack starts)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bCanDefend = false;

	// 1 based combo section number. 0 means no combo step is tracked
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 ComboStep = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bComboWindowOpen = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bContinuationRequested = false;

	// active guard, hit, or stun reaction restricts offense
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bOffenseRestricted = false;

	// CURRENT reaction
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	EVampsCombatReaction Reaction = EVampsCombatReaction::None;

	// whether the fighter can move. Move is restricted after hit to make combos easier (see VampsCombatSettings.h)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bCanMove = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bInHitstop = false;

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

	// counter incremented for each attack step so contacts and callbacks can identify the step they belong to
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 ActionNumber = 0;

	// whether the resolver credited this step with a hit. Combo continuation requires it.
	// Fighters can't continue a combo if they didn't hit the previous step
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bActionConnected = false;
};
