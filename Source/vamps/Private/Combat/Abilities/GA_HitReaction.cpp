#include "Combat/Abilities/GA_HitReaction.h"
#include "Combat/CombatTags.h"
#include "Characters/VampsDuelCharacter.h"

UGA_HitReaction::UGA_HitReaction()
{
	ActivationOwnedTags.AddTag(DuelTags::HitReaction);
}

void UGA_HitReaction::Start(double Now)
{
	UnlockAt = Now + FMath::Max(0.f, Fighter()->Config().HitLock);
	EndAt = UnlockAt + FMath::Max(0.01f, Fighter()->Config().HitResponse);
	Fighter()->PlayReaction(false);
}

void UGA_HitReaction::Advance(double Now)
{
	if (IsActive() && Now >= EndAt)
	{
		Finish();
	}
}
