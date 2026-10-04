#pragma once

#include "Combat/Abilities/DuelGameplayAbility.h"
#include "GA_Guard.generated.h"

UCLASS()
class VAMPS_API UGA_Guard : public UDuelGameplayAbility
{
	GENERATED_BODY()

public:

	UGA_Guard();

	double ArmedUntil = 0;
	double ContinueUntil = 0;
	bool bAwaitingPress = false;

	void Start(double Now, bool bAlreadyBlocked);
	bool Press(double Now);
	bool CanBlock(double Now) const;
	void Blocked(double Now);
	void Advance(double Now);
	void Leave(double Now);
};
