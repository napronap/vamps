#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "Combat/Data/VampsCombatSettings.h"
#include "VampsCombatResolver.generated.h"

class AVampsCharacter;
class AVampsWeapon;

// Captures the action and the values needed to resolve a weapon contact after all actors have ticked.
struct FVampsPendingContact
{
	TWeakObjectPtr<AVampsCharacter> Attacker;
	TWeakObjectPtr<AVampsCharacter> Recipient;
	int32 ActionNumber = 0;
	uint32 AttackerGeneration = 0;
	uint32 RecipientGeneration = 0;
	float ContactDamage = 0.f;
	float BlockReduction = 0.f;
	float GuardLoad = 0.f;
	FVector AttackOrigin = FVector::ZeroVector;
	FVector AttackDirection = FVector::ForwardVector;
	FVampsCombatStepSettings StepSettings;
	bool bBlocked = false;
	bool bCritical = false;
	bool bGuardBreak = false;
};

UCLASS()
class VAMPS_API UVampsCombatResolver : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

protected:

	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

private:

	friend class AVampsCharacter;
	friend class AVampsWeapon;

	void QueueCombatContact(AVampsCharacter& Attacker, AVampsCharacter& Recipient);
	void QueueCombatDash(AVampsCharacter& Fighter);
	void QueueCombatContinuation(AVampsCharacter& Fighter);
	void QueueCombatRecoveryEnd(AVampsCharacter& Fighter);
	void DiscardPendingCombatFor(AVampsCharacter& Fighter);

	void ResolveCombatFrame(UWorld* TickedWorld, ELevelTick TickType, float DeltaSeconds);
	void ResolvePendingContacts();

	TArray<FVampsPendingContact> PendingContacts;
	TArray<TWeakObjectPtr<AVampsCharacter>> PendingContinuations;
	TMap<TWeakObjectPtr<AVampsCharacter>, int32> PendingRecoveryEnds;
	TArray<TWeakObjectPtr<AVampsCharacter>> PendingDashes;
	FDelegateHandle PostActorTickHandle;
};
