#pragma once

#include "Abilities/GameplayAbility.h"
#include "Engine/TimerHandle.h"
#include "GA_Defend.generated.h"

// Each request renews one defense window. An active attack keeps it armed until a block or the attack ends.
UCLASS()
class VAMPS_API UGA_Defend : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UGA_Defend();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	                                const FGameplayAbilityActorInfo* ActorInfo,
	                                const FGameplayTagContainer* SourceTags,
	                                const FGameplayTagContainer* TargetTags,
	                                FGameplayTagContainer* OptionalRelevantTags) const override;
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

	FTimerHandle DefenseRequestTimer;

	void ExpireDefenseRequest();
};
