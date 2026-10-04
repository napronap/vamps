#pragma once

#include "Combat/Abilities/DuelGameplayAbility.h"
#include "GA_HitReaction.generated.h"

UCLASS()
class VAMPS_API UGA_HitReaction : public UDuelGameplayAbility
{
	GENERATED_BODY()

public:

	UGA_HitReaction();

	double UnlockAt = 0;
	double EndAt = 0;

	void Start(double Now);
	void Advance(double Now);

	bool CanRespond(double Now) const
	{
		return IsActive() && Now >= UnlockAt && Now < EndAt;
	}
};
