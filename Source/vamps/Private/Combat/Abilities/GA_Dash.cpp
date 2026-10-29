#include "Combat/Abilities/GA_Dash.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Characters/VampsCharacter.h"
#include "Characters/VampsLockOnComponent.h"
#include "Combat/Attributes/VampsAttributeSet.h"
#include "Combat/Data/VampsCombatSettings.h"
#include "Combat/VampsCombatLog.h"
#include "Combat/VampsCombatMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
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
	AVampsCharacter* Fighter = Cast<AVampsCharacter>(GetAvatarActorFromActorInfo());
	UVampsCombatMovementComponent* Movement =
	    Fighter ? Cast<UVampsCombatMovementComponent>(Fighter->GetCharacterMovement()) : nullptr;
	if (!Fighter || !Movement || !Fighter->CombatSettings || !Fighter->Attributes || !ActorInfo->GetAnimInstance())
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s' requires a VampsCharacter with combat movement, settings, attributes and animation."),
		       __FUNCTION__,
		       *GetPathName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	Fighter->SetCombatPhase(Fighter->CombatState.Phase);
	if (!Fighter->CombatState.bCanDash)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	const bool bBackwardDash = Fighter->RequestedDashDirection.IsNearlyZero();
	UAnimMontage* DashMontage = bBackwardDash ? Fighter->CombatSettings->BackwardDashMontage.Get()
	                                          : Fighter->CombatSettings->ForwardDashMontage.Get();
	if (!DashMontage || !DashMontage->HasRootMotion() || DashMontage->GetPlayLength() <= 0.f ||
	    !FMath::IsFinite(DashMontage->RateScale) || DashMontage->RateScale <= 0.f ||
	    !DashMontage->bEnableAutoBlendOut || DashMontage == Fighter->CombatSettings->AttackMontage ||
	    DashMontage == Fighter->CombatSettings->GuardReactionMontage ||
	    DashMontage == Fighter->CombatSettings->HitReactionMontage)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': dash montage '%s' must be independent, enable root motion, have positive length/rate "
		            "and Auto Blend Out."),
		       __FUNCTION__,
		       *Fighter->GetPathName(),
		       *GetPathNameSafe(DashMontage));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	AVampsCharacter* TargetFighter = Fighter->LockOn ? Fighter->LockOn->GetCurrentTarget() : nullptr;
	if (bBackwardDash &&
	    (!IsValid(TargetFighter) ||
	     (TargetFighter->GetActorLocation() - Fighter->GetActorLocation()).GetSafeNormal2D().IsNearlyZero()))
	{
		UE_LOG(LogVampsCombat,
		       Warning,
		       TEXT("%hs: '%s': backward dash requires a target at a different position; target='%s'."),
		       __FUNCTION__,
		       *Fighter->GetPathName(),
		       *GetPathNameSafe(TargetFighter));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogVampsCombat,
		       Warning,
		       TEXT("%hs: '%s': CommitAbility rejected dash."),
		       __FUNCTION__,
		       *Fighter->GetPathName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	Movement->StopVoluntaryMotion();
	const FVector DashFacing = bBackwardDash
	                               ? (TargetFighter->GetActorLocation() - Fighter->GetActorLocation()).GetSafeNormal2D()
	                               : Fighter->RequestedDashDirection;
	Fighter->CombatState.bIsDashing = true;
	Fighter->CombatState.bIsSprinting = false;
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
	    this, NAME_None, DashMontage, 1.f, NAME_None, true, 1.f, 0.f, true);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_Dash::FinishDash);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_Dash::FinishDash);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_Dash::FinishDash);
	MontageTask->ReadyForActivation();
	if (!IsActive() || !ActorInfo->GetAnimInstance()->Montage_IsPlaying(DashMontage))
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': dash montage '%s' did not start; verify skeleton and slot."),
		       __FUNCTION__,
		       *Fighter->GetPathName(),
		       *DashMontage->GetPathName());
		Fighter->bMobilityHeld = false;
		if (IsActive())
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}
		return;
	}
	Fighter->SetActorRotation(DashFacing.Rotation());
	Fighter->AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetMobilityEnergyAttribute(),
	                                                Fighter->Attributes->GetMobilityEnergy() -
	                                                    Fighter->CombatSettings->DashEnergyCost);
	Fighter->MobilityRecoveryStartsAt =
	    Fighter->GetWorld()->GetTimeSeconds() + Fighter->CombatSettings->MobilityRecoveryDelay;
	Fighter->SetCombatPhase(Fighter->CombatState.Phase);
}

void UGA_Dash::FinishDash()
{
	if (IsActive())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UGA_Dash::EndAbility(const FGameplayAbilitySpecHandle Handle,
                          const FGameplayAbilityActorInfo* ActorInfo,
                          const FGameplayAbilityActivationInfo ActivationInfo,
                          bool bReplicateEndAbility,
                          bool bWasCancelled)
{
	if (AVampsCharacter* Fighter = Cast<AVampsCharacter>(GetAvatarActorFromActorInfo()))
	{
		if (Fighter->CombatState.bIsDashing)
		{
			Fighter->CombatState.bIsDashing = false;
			Fighter->GetCharacterMovement()->StopMovementImmediately();
		}
		Fighter->SetCombatPhase(Fighter->CombatState.Phase);
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
