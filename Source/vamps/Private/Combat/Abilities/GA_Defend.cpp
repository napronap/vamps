#include "Combat/Abilities/GA_Defend.h"
#include "Characters/VampsCharacter.h"
#include "Combat/Data/VampsCombatSettings.h"
#include "Combat/VampsCombatLog.h"
#include "Engine/World.h"
#include "TimerManager.h"

UGA_Defend::UGA_Defend()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	bRetriggerInstancedAbility = true;
}

bool UGA_Defend::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                    const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayTagContainer* SourceTags,
                                    const FGameplayTagContainer* TargetTags,
                                    FGameplayTagContainer* OptionalRelevantTags) const
{
	// A retrigger needs GAS to end the previous activation synchronously.
	if (ScopeLockCount > 0 || bIsAbilityEnding)
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_Defend::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                 const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                 const FGameplayEventData* TriggerEventData)
{
	AVampsCharacter* Fighter = Cast<AVampsCharacter>(GetAvatarActorFromActorInfo());
	if (!Fighter)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const float RequestDuration = Fighter->CombatSettings->DefenseRequestDuration;
	if (!FMath::IsFinite(RequestDuration) || RequestDuration <= 0.f)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s' has an invalid defense request duration (%g)."),
		       __FUNCTION__,
		       *Fighter->GetPathName(),
		       RequestDuration);
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	Fighter->CombatState.bDefenseRequestArmed = true;
	Fighter->RefreshCombatPermissions();
	if (!IsActive())
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(
	    DefenseRequestTimer, this, &UGA_Defend::ExpireDefenseRequest, RequestDuration);
}

void UGA_Defend::EndAbility(const FGameplayAbilitySpecHandle Handle,
                            const FGameplayAbilityActorInfo* ActorInfo,
                            const FGameplayAbilityActivationInfo ActivationInfo,
                            bool bReplicateEndAbility,
                            bool bWasCancelled)
{
	if (!IsEndAbilityValid(Handle, ActorInfo))
	{
		return;
	}

	// GAS may defer ending while the ability is locked. Disarm when it actually ends.
	if (ScopeLockCount == 0)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(DefenseRequestTimer);
		}

		if (AVampsCharacter* Fighter = Cast<AVampsCharacter>(GetAvatarActorFromActorInfo());
		    Fighter && Fighter->CombatState.bDefenseRequestArmed)
		{
			Fighter->CombatState.bDefenseRequestArmed = false;
			Fighter->RefreshCombatPermissions();
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Defend::ExpireDefenseRequest()
{
	const AVampsCharacter* Fighter = Cast<AVampsCharacter>(GetAvatarActorFromActorInfo());
	if (Fighter && Fighter->CombatState.MontageInstanceID != INDEX_NONE)
	{
		// The character cancels this request when it clears the attack playback.
		return;
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
