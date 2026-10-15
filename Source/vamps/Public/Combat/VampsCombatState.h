#pragma once

#include "CoreMinimal.h"
#include "VampsCombatState.generated.h"

UENUM(BlueprintType)
enum class EVampsActionPhase : uint8
{
	Idle,
	Preparation,
	Contacts,
	Recovery
};

UENUM(BlueprintType)
enum class EVampsCombatRequest : uint8
{
	None,
	ActionStarted,
	DefenseRearmed,
	Rejected
};

UENUM(BlueprintType)
enum class EVampsContactResult : uint8
{
	None,
	Miss,
	OpenHit,
	Blocked,
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
