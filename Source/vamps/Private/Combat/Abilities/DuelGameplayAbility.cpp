#include "Combat/Abilities/DuelGameplayAbility.h"
#include "Characters/VampsDuelCharacter.h"

UDuelGameplayAbility::UDuelGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

AVampsDuelCharacter* UDuelGameplayAbility::Fighter() const
{
	return Cast<AVampsDuelCharacter>(GetAvatarActorFromActorInfo());
}

void UDuelGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}

void UDuelGameplayAbility::Finish(bool bCancelled)
{
	if (IsActive())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bCancelled);
	}
}
