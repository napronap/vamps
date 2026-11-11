#include "Combat/Abilities/GA_Dash.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimMontage.h"
#include "Characters/VampsCharacter.h"
#include "Characters/VampsLockOnComponent.h"
#include "Combat/Attributes/VampsAttributeSet.h"
#include "Combat/Data/VampsCombatSettings.h"
#include "Combat/VampsCombatLog.h"
#include "Combat/VampsCombatMovementComponent.h"
#include "Engine/World.h"

UGA_Dash::UGA_Dash()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UGA_Dash::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                               const FGameplayAbilityActorInfo* ActorInfo,
                               const FGameplayAbilityActivationInfo ActivationInfo,
                               const FGameplayEventData* TriggerEventData)
{
	AVampsCharacter* Fighter = static_cast<AVampsCharacter*>(GetAvatarActorFromActorInfo());

	const bool bBackwardDash = Fighter->RequestedDashDirection.IsNearlyZero();

	UAnimMontage* DashMontage = bBackwardDash ? Fighter->CombatSettings->BackwardDashMontage.Get()
	                                          : Fighter->CombatSettings->ForwardDashMontage.Get();

	// invalid config
	if (!DashMontage || !DashMontage->HasRootMotion() || !DashMontage->bEnableAutoBlendOut)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': check config for dash montage '%s'"),
		       __FUNCTION__,
		       *Fighter->GetPathName(),
		       *GetPathNameSafe(DashMontage));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// the backward montage moves away from the fighter's facing. Aim it at the target when available
	const AVampsCharacter* TargetFighter = bBackwardDash ? Fighter->LockOn->GetCurrentTarget() : nullptr;
	const FVector DashFacing = TargetFighter
	                               ? (TargetFighter->GetActorLocation() - Fighter->GetActorLocation()).GetSafeNormal2D()
	                               : Fighter->RequestedDashDirection;

	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
	    this, NAME_None, DashMontage, 1.f, NAME_None, true, 1.f, 0.f, true);

	MontageTask->OnCompleted.AddDynamic(this, &UGA_Dash::FinishDash);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_Dash::FinishDash);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_Dash::FinishDash);
	MontageTask->ReadyForActivation();

	if (!IsActive())
	{
		return;
	}

	static_cast<UVampsCombatMovementComponent*>(Fighter->GetCharacterMovement())->StopVoluntaryMotion();

	// while writing this I realized these and others could be checked more easily by querying the ASC directly
	// maybe some day I'll do it. Until then fat struct it is
	Fighter->CombatState.bIsDashing = true;
	// is sprinting is different though, and it also affects this ability, so the ability would have to be split in two
	Fighter->CombatState.bIsSprinting = false;

	if (!DashFacing.IsNearlyZero())
	{
		Fighter->SetActorRotation(DashFacing.Rotation());
	}

	const float MobilityEnergy = Fighter->Attributes->GetMobilityEnergy();
	const float DashCost = Fighter->CombatSettings->DashEnergyCost;

	Fighter->AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetMobilityEnergyAttribute(), MobilityEnergy - DashCost);

	Fighter->MobilityRecoveryStartsAt = Fighter->GetWorld()->GetTimeSeconds() + Fighter->CombatSettings->MobilityRecoveryDelay;
}

void UGA_Dash::FinishDash()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Dash::EndAbility(const FGameplayAbilitySpecHandle Handle,
                          const FGameplayAbilityActorInfo* ActorInfo,
                          const FGameplayAbilityActivationInfo ActivationInfo,
                          bool bReplicateEndAbility,
                          bool bWasCancelled)
{
	AVampsCharacter* Fighter = Cast<AVampsCharacter>(GetAvatarActorFromActorInfo());

	if (Fighter && Fighter->CombatState.bIsDashing)
	{
		Fighter->CombatState.bIsDashing = false;
		Fighter->GetCharacterMovement()->StopMovementImmediately();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
