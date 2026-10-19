#pragma once

#include "Abilities/GameplayAbility.h"
#include "GA_Dash.generated.h"

UCLASS()
class VAMPS_API UGA_Dash : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UGA_Dash();

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
	void FinishDash();
};
