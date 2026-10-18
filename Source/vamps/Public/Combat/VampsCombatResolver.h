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
	float OffensiveRestrictionDuration = 0.f;
	FVector AttackOrigin = FVector::ZeroVector;
	FVector AttackDirection = FVector::ForwardVector;
	FVampsCombatStepSettings StepSettings;
	bool bBlocked = false;
};

UCLASS()
class VAMPS_API UVampsCombatResolver : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void QueueCombatContact(AVampsCharacter* Attacker, AVampsCharacter* Recipient);
	void QueueCombatContinuation(AVampsCharacter* Fighter);
	void QueueCombatLocomotion(AVampsCharacter* Fighter);
	void DiscardContactsFor(const AVampsCharacter* Fighter);

protected:

	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

private:

	TArray<FVampsPendingContact> PendingContacts;
	TArray<TWeakObjectPtr<AVampsCharacter>> PendingContinuations;
	TArray<TWeakObjectPtr<AVampsCharacter>> PendingLocomotion;
	FDelegateHandle PostActorTickHandle;

	void ResolveCombatContacts(UWorld* TickedWorld, ELevelTick TickType, float DeltaSeconds);
};
