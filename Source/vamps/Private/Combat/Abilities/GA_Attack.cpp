#include "Combat/Abilities/GA_Attack.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Characters/VampsDuelCharacter.h"
#include "Combat/CombatTags.h"
#include "Combat/FistHitboxComponent.h"

UGA_Attack::UGA_Attack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	ActivationOwnedTags.AddTag(DuelTags::Attack);
	ActivationBlockedTags.AddTag(DuelTags::Guard);
}

void UGA_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                 const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                 const FGameplayEventData* TriggerEventData)
{
	AVampsDuelCharacter* Fighter = Cast<AVampsDuelCharacter>(GetAvatarActorFromActorInfo());
	if (!Fighter || !Fighter->AttackMontage || !Fighter->FistHitbox->HasFistSocket() ||
	    !CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	Fighter->FistHitbox->ResetHits();
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
	    this, NAME_None, Fighter->AttackMontage, 1.f, NAME_None, true, 1.f, 0.f, true);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_Attack::FinishAttack);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_Attack::FinishAttack);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_Attack::FinishAttack);
	MontageTask->OnBlendOut.AddDynamic(this, &UGA_Attack::StopTracing);
	MontageTask->ReadyForActivation();
}

void UGA_Attack::FinishAttack()
{
	if (IsActive())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UGA_Attack::StopTracing()
{
	if (AVampsDuelCharacter* Fighter = Cast<AVampsDuelCharacter>(GetAvatarActorFromActorInfo()))
	{
		Fighter->FistHitbox->StopTracing();
	}
}

void UGA_Attack::EndAbility(const FGameplayAbilitySpecHandle Handle,
                            const FGameplayAbilityActorInfo* ActorInfo,
                            const FGameplayAbilityActivationInfo ActivationInfo,
                            bool bReplicateEndAbility,
                            bool bWasCancelled)
{
	StopTracing();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
