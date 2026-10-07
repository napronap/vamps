#include "Combat/Abilities/GA_Guard.h"
#include "Combat/CombatTags.h"

UGA_Guard::UGA_Guard()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	ActivationOwnedTags.AddTag(DuelTags::Guard);
	ActivationBlockedTags.AddTag(DuelTags::Attack);
}

void UGA_Guard::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                const FGameplayAbilityActorInfo* ActorInfo,
                                const FGameplayAbilityActivationInfo ActivationInfo,
                                const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	// The ability stays active until the guard button is released.
}
