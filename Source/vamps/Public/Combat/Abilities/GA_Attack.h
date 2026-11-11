#pragma once

#include "Abilities/GameplayAbility.h"
#include "GA_Attack.generated.h"

/**
 * Plays one attack montage for the entire combo. The Montage itself is the ability pretty much
 *
 * Checklist for the ability not to explode:
 * - CombatSettings with at least one weapon
 * - AttackDamage and NARecoveryDuration
 * - valid attack Montage:
 *    - At least one section
 *    - Disable Enable Auto Blend Out (character and this ability take care of that)
 * 	  - Provide a ComboSteps entry for every section in montage order. 
 *    - ContactReactionDuration, TargetAlignmentDistance and KnockbackDistance
 * - Required branching points (notifies):
 *   - One ANS_AttackTrace per section (if a weapon hits more than once per attacktrace, use AN_ResetWeapoHits)
 *   - One ANS_ComboWindow per non final section. Final section with no combo window is ok
 *   - Start combo window after the trace starts. End it after trace end, and before the section ends
 *   - One AN_EndRecovery per section. This should mark when the character can move again
 */
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
