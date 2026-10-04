#pragma once

#include "Abilities/GameplayAbility.h"
#include "DuelGameplayAbility.generated.h"

class AVampsDuelCharacter;

UCLASS(Abstract)
class VAMPS_API UDuelGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UDuelGameplayAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	                             const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

	AVampsDuelCharacter* Fighter() const;
	void Finish(bool bCancelled = false);
};
