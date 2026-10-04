#pragma once

#include "Combat/Abilities/DuelGameplayAbility.h"
#include "Combat/Data/CombatConfig.h"
#include "GA_Attack.generated.h"

UCLASS()
class VAMPS_API UGA_Attack : public UDuelGameplayAbility
{
	GENERATED_BODY()

public:

	UGA_Attack();

	// State shared with contact resolution and presentation.
	int32 StrikeIndex = 0;
	uint64 Serial = 0;
	double StartedAt = 0;
	FVector Direction = FVector::ForwardVector;
	TSet<TWeakObjectPtr<AVampsDuelCharacter>> HitActors;

	void Start(double Now);
	void Advance(double Now);
	bool Continue(double Now);
	void Leave(double Now);

	// Logical phase boundaries; animation playback follows these times.
	EDuelPhase Phase(double Now) const;
	double ActiveAt() const;
	double RecoveryAt() const;
	double EndAt() const;
	double NextBoundary(double Now) const;

private:

	float ApproachDistance = 0;
	float ApproachApplied = 0;
	EDuelPhase PresentedPhase = EDuelPhase::Default;

	void StartStrike(double Now);
};
