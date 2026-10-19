#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "Combat/Data/VampsCombatSettings.h"
#include "VampsCombatResolver.generated.h"

class AVampsCharacter;

struct FVampsPendingContact
{
	TWeakObjectPtr<AVampsCharacter> Attacker;
	TWeakObjectPtr<AVampsCharacter> Recipient;
	int32 ActionNumber = 0;
	uint32 AttackerGeneration = 0;
	uint32 RecipientGeneration = 0;
	double ContactTime = 0;
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

	void QueueCombatContact(AVampsCharacter* Attacker, AVampsCharacter* Recipient);
	void QueueCombatDash(AVampsCharacter* Fighter);
	void QueueCombatContinuation(AVampsCharacter* Fighter);
	void QueueCombatLocomotion(AVampsCharacter* Fighter);
	void DiscardContactsFor(const AVampsCharacter* Fighter);

protected:

	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

private:

	TArray<TWeakObjectPtr<AVampsCharacter>> PendingDashes;

	TArray<FVampsPendingContact> PendingContacts;
	TArray<TWeakObjectPtr<AVampsCharacter>> PendingContinuations;
	TArray<TWeakObjectPtr<AVampsCharacter>> PendingLocomotion;
	FDelegateHandle PostActorTickHandle;

	void ResolveCombatContacts(UWorld* TickedWorld, ELevelTick TickType, float DeltaSeconds);
};
