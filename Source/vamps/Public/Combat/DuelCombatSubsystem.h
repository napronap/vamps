#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "DuelCombatSubsystem.generated.h"

class AVampsDuelCharacter;

/** Local duel clock: visits phase boundaries and resolves ordered contacts before another step. */
UCLASS()
class VAMPS_API UDuelCombatSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:

	virtual bool DoesSupportWorldType(EWorldType::Type Type) const override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	void Register(AVampsDuelCharacter* Fighter);
	void Unregister(AVampsDuelCharacter* Fighter);

	void Synchronize();
	void AdvanceTo(double Time);

	double Now() const
	{
		return Time;
	}

	uint64 NextSerial()
	{
		return ++Serial;
	}

private:

	TArray<TWeakObjectPtr<AVampsDuelCharacter>> Fighters;
	double Time = 0;
	uint64 Serial = 0;
	bool bAdvancing = false;

	void ResolveContacts();
};
