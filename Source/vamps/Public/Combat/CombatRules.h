#pragma once

#include "CoreMinimal.h"

enum class EDuelInputIntent : uint8
{
	None,
	StartAttack,
	ContinueAttack,
	Guard
};

/** One expiring press. Its intent cannot change when an action is interrupted. */
struct FDuelInputBuffer
{
	EDuelInputIntent Intent = EDuelInputIntent::None;
	double ExpiresAt = 0;

	void Store(EDuelInputIntent InIntent, double Now, double Duration)
	{
		Intent = InIntent;
		ExpiresAt = Now + FMath::Max(0., Duration);
	}

	bool Has(EDuelInputIntent InIntent, double Now) const
	{
		return Intent == InIntent && Now < ExpiresAt;
	}

	void Clear()
	{
		Intent = EDuelInputIntent::None;
	}

	void Expire(double Now)
	{
		if (Now >= ExpiresAt)
		{
			Clear();
		}
	}
};

/** Used only by GA_NA. A new hit replaces the pause deadline, never the remaining cooldown. */
struct FDuelNAClock
{
	double Remaining = 0;
	double ResumeAt = 0;
	double LastUpdate = 0;

	void Start(double Now, double Duration);
	void Advance(double Now);
	void Pause(double Now, double Delay);

	double EndAt() const
	{
		return FMath::Max(LastUpdate, ResumeAt) + Remaining;
	}
};

namespace DuelRules
{
	bool InsideDefense(const FVector& Forward, const FVector& ToAttacker, float TotalDegrees);
}
