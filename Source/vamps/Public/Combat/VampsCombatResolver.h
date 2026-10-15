#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "VampsCombatResolver.generated.h"

class AVampsCharacter;

struct FVampsPendingContact
{
	TWeakObjectPtr<AVampsCharacter> Attacker;
	TWeakObjectPtr<AVampsCharacter> Recipient;
	int32 ActionNumber = 0;
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
	void DiscardContactsFor(const AVampsCharacter* Fighter);

protected:

	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

private:

	TArray<FVampsPendingContact> PendingContacts;
	FDelegateHandle PostActorTickHandle;

	void ResolveCombatContacts(UWorld* TickedWorld, ELevelTick TickType, float DeltaSeconds);
};
