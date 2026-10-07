#pragma once

#include "Abilities/GameplayAbility.h"
#include "GA_Guard.generated.h"

UCLASS()
class VAMPS_API UGA_Guard : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UGA_Guard();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	                             const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
};
