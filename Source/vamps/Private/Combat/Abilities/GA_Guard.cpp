#include "Combat/Abilities/GA_Guard.h"
#include "Combat/CombatTags.h"
#include "Characters/VampsDuelCharacter.h"

UGA_Guard::UGA_Guard()
{
	ActivationOwnedTags.AddTag(DuelTags::Guard);
}

void UGA_Guard::Start(double Now, bool bAlreadyBlocked)
{
	bAwaitingPress = true;
	ContinueUntil = Now + FMath::Max(0.01f, Fighter()->Config().GuardContinuation);

	if (bAlreadyBlocked)
	{
		Blocked(Now);
	}
	else
	{
		Press(Now);
	}
}

bool UGA_Guard::Press(double Now)
{
	if (!IsActive() || !bAwaitingPress || Now >= ContinueUntil)
	{
		return false;
	}

	bAwaitingPress = false;
	ArmedUntil = Now + FMath::Max(0.01f, Fighter()->Config().GuardWindow);
	Fighter()->PlayReaction(true);
	return true;
}

bool UGA_Guard::CanBlock(double Now) const
{
	return IsActive() && !bAwaitingPress && Now < ArmedUntil;
}

void UGA_Guard::Blocked(double Now)
{
	bAwaitingPress = true;
	ArmedUntil = Now;
	ContinueUntil = Now + FMath::Max(0.01f, Fighter()->Config().GuardContinuation);
	Fighter()->PlayReaction(true);
}

void UGA_Guard::Advance(double Now)
{
	if (IsActive() && Now >= (bAwaitingPress ? ContinueUntil : ArmedUntil))
	{
		Leave(Now);
	}
}

void UGA_Guard::Leave(double Now)
{
	if (!IsActive())
	{
		return;
	}

	Finish();
	Fighter()->ClearInput();
	Fighter()->StartNA(Now, false);
}
