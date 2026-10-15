#pragma once

#include "Abilities/GameplayAbility.h"
#include "GA_Attack.generated.h"

UCLASS()
class VAMPS_API UGA_Attack : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UGA_Attack();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	                             const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
	                        const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo,
	                        bool bReplicateEndAbility,
	                        bool bWasCancelled) override;

private:

	UFUNCTION()
	void FinishAttack();
};
