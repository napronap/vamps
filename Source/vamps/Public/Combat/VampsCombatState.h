#pragma once

#include "CoreMinimal.h"
#include "VampsCombatState.generated.h"

class AVampsCharacter;

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

UENUM(BlueprintType)
enum class EVampsCombatRequest : uint8
{
	None,
	ActionStarted,
	DefenseRearmed,
	ContinuationRequested,
	ComboAdvanced,
	Rejected
};

UENUM(BlueprintType)
enum class EVampsContactResult : uint8
{
	None,
	Miss,
	OpenHit,
	Blocked,
	GuardBreak,
	Mixed
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
	bool bIsRecovering = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 ComboStep = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bComboWindowOpen = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bContinuationRequested = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bRecoveryVisualReleased = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bOffenseRestricted = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	double ReactionEndsAt = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	EVampsCombatReaction Reaction = EVampsCombatReaction::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bCanMove = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mobility")
	bool bCanDash = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mobility")
	bool bGuardDashReady = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mobility")
	bool bIsSprinting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mobility")
	bool bDashRequested = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bDefenseRequestArmed = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	double DefenseRequestEndsAt = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TWeakObjectPtr<AVampsCharacter> DefendedAttacker;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 DefendedActionNumber = INDEX_NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TWeakObjectPtr<AVampsCharacter> ReactionSource;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	bool bLastIncomingCritical = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	float LastDamageReceived = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	double NARecoveryEndsAt = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 MontageInstanceID = INDEX_NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	EVampsCombatRequest LastRequest = EVampsCombatRequest::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	EVampsContactResult LastOutgoingContact = EVampsContactResult::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	EVampsContactResult LastIncomingContact = EVampsContactResult::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	int32 RequestCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	int32 ActionNumber = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	bool bActionConnected = false;

	UPROPERTY(VisibleAnywhere, Category = "Debug")
	uint64 LastOutgoingContactFrame = 0;

	UPROPERTY(VisibleAnywhere, Category = "Debug")
	uint64 LastIncomingContactFrame = 0;
};
