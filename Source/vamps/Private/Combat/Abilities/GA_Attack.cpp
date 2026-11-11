#include "Combat/Abilities/GA_Attack.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Characters/VampsCharacter.h"
#include "Combat/Data/VampsCombatSettings.h"
#include "Combat/VampsCombatLog.h"
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
	AVampsCharacter* Fighter = Cast<AVampsCharacter>(GetAvatarActorFromActorInfo());
	if (!IsValid(Fighter))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAnimMontage* Montage = Fighter->CombatSettings->AttackMontage;

	// task plays the montage when activated and stops it if the ability ends first
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
	    this, NAME_None, Montage, 1.f, NAME_None, true, 1.f, 0.f, true);

	// this ability subscribes to the task's events to end the attack
	// Most of these can't happen as this ability doesn't consume resources etc and all the variants to its usage are handled
	// within the character. But this seems to be usual setup
	MontageTask->OnCompleted.AddDynamic(this, &UGA_Attack::FinishAttack);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_Attack::FinishAttack);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_Attack::FinishAttack);
	MontageTask->ReadyForActivation();

	if (!IsActive())
	{
		return;
	}

	UAnimInstance* Animation = Fighter->GetMesh()->GetAnimInstance();
	FAnimMontageInstance* Playback = Animation ? Animation->GetActiveInstanceForMontage(Montage) : nullptr;

	if (!Playback)
	{
		UE_LOG(
		    LogVampsCombat,
		    Error,
		    TEXT("%hs: %s: attack rejected; check montage '%s'"),
		    __FUNCTION__,
		    *Fighter->GetName(),
		    *GetPathNameSafe(Montage));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// clear automatic section links on this playback. The resolver advances approved combos explicitly
	// this shouldn't happen though if montages are correctly set up with no linked sections
	for (int32 SectionIndex = 0; SectionIndex < Montage->GetNumSections(); ++SectionIndex)
	{
		Playback->SetNextSectionID(SectionIndex, INDEX_NONE);
	}

	// start the first combat step and record the playback ID used to match combat notifies
	if (!Fighter->BeginCombatAction(Playback->GetInstanceID()))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
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
	if (AVampsCharacter* Fighter = Cast<AVampsCharacter>(GetAvatarActorFromActorInfo());
	    Fighter && Fighter->CombatState.MontageInstanceID != INDEX_NONE)
	{
		// A registered playback ended early (defense, etc). Clear its combat state while GAS ends the ability
		Fighter->CancelCombatOffense(false);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
