#pragma once

#include "Combat/Abilities/DuelGameplayAbility.h"
#include "Combat/CombatRules.h"
#include "GA_NA.generated.h"

UCLASS()
class VAMPS_API UGA_NA : public UDuelGameplayAbility
{
	GENERATED_BODY()

public:

	UGA_NA();

	FDuelNAClock Clock;

	void Start(double Now, double Duration);
	void Advance(double Now);
	void OnDamage(double Now);
};
