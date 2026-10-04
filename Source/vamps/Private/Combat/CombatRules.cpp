#include "Combat/CombatRules.h"

void FDuelNAClock::Start(double Now, double Duration)
{
	Remaining = FMath::Max(0., Duration);
	ResumeAt = LastUpdate = Now;
}

void FDuelNAClock::Advance(double Now)
{
	if (Now <= LastUpdate)
	{
		return;
	}

	Remaining = FMath::Max(0., Remaining - FMath::Max(0., Now - FMath::Max(LastUpdate, ResumeAt)));
	LastUpdate = Now;
}

void FDuelNAClock::Pause(double Now, double Delay)
{
	Advance(Now);

	if (Remaining > 0)
	{
		ResumeAt = Now + FMath::Max(0., Delay);
	}
}

bool DuelRules::InsideDefense(const FVector& Forward, const FVector& ToAttacker, float TotalDegrees)
{
	const FVector Direction = ToAttacker.GetSafeNormal2D();

	if (Direction.IsNearlyZero())
	{
		return true;
	}

	return FVector::DotProduct(Forward.GetSafeNormal2D(), Direction) + KINDA_SMALL_NUMBER >=
	       FMath::Cos(FMath::DegreesToRadians(FMath::Clamp(TotalDegrees, 0.f, 360.f) * 0.5f));
}
