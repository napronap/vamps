#include "Combat/Abilities/GA_NA.h"
#include "Combat/CombatTags.h"
#include "Characters/VampsDuelCharacter.h"

UGA_NA::UGA_NA()
{
	ActivationOwnedTags.AddTag(DuelTags::NA);
}

void UGA_NA::Start(double Now, double Duration)
{
	Clock.Start(Now, Duration);
}

void UGA_NA::Advance(double Now)
{
	if (!IsActive())
	{
		return;
	}

	Clock.Advance(Now);

	if (Clock.Remaining <= 0)
	{
		Finish();
	}
}

void UGA_NA::OnDamage(double Now)
{
	if (IsActive())
	{
		Clock.Pause(Now, Fighter()->Config().NAResumeDelay);
	}
}
