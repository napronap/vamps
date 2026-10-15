#include "Combat/Abilities/GA_Attack.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Animation/ANS_AttackTrace.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Characters/VampsCharacter.h"
#include "Combat/FistHitboxComponent.h"
#include "Components/SkeletalMeshComponent.h"

UGA_Attack::UGA_Attack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UGA_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                 const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                 const FGameplayEventData* TriggerEventData)
{
	AVampsCharacter* Fighter = CastChecked<AVampsCharacter>(GetAvatarActorFromActorInfo());
	checkf(Fighter->AttackMontage, TEXT("%s: AttackMontage must be assigned."), *Fighter->GetName());
	checkf(Fighter->FistHitbox->HasFistSocket(), TEXT("%s: the fist socket must exist."), *Fighter->GetName());
	const FAnimNotifyEvent* ContactNotify = nullptr;
	for (const FAnimNotifyEvent& NotifyEvent : Fighter->AttackMontage->Notifies)
	{
		if (NotifyEvent.NotifyStateClass && NotifyEvent.NotifyStateClass->IsA<UANS_AttackTrace>())
		{
			checkf(!ContactNotify, TEXT("AttackMontage must have exactly one Attack Trace state."));
			ContactNotify = &NotifyEvent;
		}
	}
	checkf(ContactNotify, TEXT("AttackMontage requires an Attack Trace notify state."));
	checkf(Fighter->AttackMontage->bEnableAutoBlendOut, TEXT("AttackMontage requires automatic blend-out to finish."));
	checkf(ContactNotify->GetTriggerTime() > 0.f &&
	           ContactNotify->GetEndTriggerTime() > ContactNotify->GetTriggerTime() &&
	           ContactNotify->GetEndTriggerTime() < Fighter->AttackMontage->GetPlayLength(),
	       TEXT("AttackMontage must order preparation, contacts and recovery."));
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
	    this, NAME_None, Fighter->AttackMontage, 1.f, NAME_None, true, 1.f, 0.f, true);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_Attack::FinishAttack);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_Attack::FinishAttack);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_Attack::FinishAttack);
	MontageTask->ReadyForActivation();
	if (IsActive())
	{
		UAnimInstance* Animation = Fighter->GetMesh()->GetAnimInstance();
		const FAnimMontageInstance* Playback = Animation->GetActiveInstanceForMontage(Fighter->AttackMontage);
		checkf(Playback, TEXT("The combat montage must have started before tracking its playback."));
		Fighter->BeginCombatAction(Playback->GetInstanceID());
	}
}

void UGA_Attack::FinishAttack()
{
	if (IsActive())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UGA_Attack::EndAbility(const FGameplayAbilitySpecHandle Handle,
                            const FGameplayAbilityActorInfo* ActorInfo,
                            const FGameplayAbilityActivationInfo ActivationInfo,
                            bool bReplicateEndAbility,
                            bool bWasCancelled)
{
	if (AVampsCharacter* Fighter = Cast<AVampsCharacter>(GetAvatarActorFromActorInfo()))
	{
		Fighter->FinishCombatAction();
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
