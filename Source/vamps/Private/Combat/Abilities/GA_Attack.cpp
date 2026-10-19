#include "Combat/Abilities/GA_Attack.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Animation/AN_ResetWeaponHits.h"
#include "Animation/ANS_AttackTrace.h"
#include "Animation/ANS_ComboWindow.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Characters/VampsCharacter.h"
#include "Combat/Data/VampsCombatSettings.h"
#include "Components/SkeletalMeshComponent.h"
#include "Combat/VampsCombatLog.h"
#include "Combat/VampsWeapon.h"

static FString ValidateAttackConfiguration(const AVampsCharacter& Fighter)
{
	const UVampsCombatSettings* Settings = Fighter.CombatSettings;
	if (!Settings || !Settings->HasValidReactionSettings() || !FMath::IsFinite(Settings->NARecoveryDuration) ||
	    !(Settings->NARecoveryDuration > 0.f))
	{
		return TEXT("CombatSettings must define a positive NA recovery duration.");
	}
	if (Fighter.Weapons.IsEmpty())
	{
		return TEXT("Combat requires equipped weapons.");
	}
	for (const AVampsWeapon* EquippedWeapon : Fighter.Weapons)
	{
		if (!IsValid(EquippedWeapon))
		{
			return TEXT("An equipped weapon is missing or destroyed. Reinitialize combat with valid weapon assets.");
		}
	}
	const UAnimMontage* Montage = Settings->AttackMontage;
	if (!Montage)
	{
		return TEXT("CombatSettings must define AttackMontage.");
	}
	if (Montage->GetNumSections() < 1 || Montage->GetNumSections() > 3)
	{
		return TEXT("AttackMontage must contain one to three ordered attack sections.");
	}
	if (!FMath::IsFinite(Montage->RateScale) || !(Montage->RateScale > 0.f) || Montage->TimeStretchCurve.IsValid())
	{
		return TEXT("AttackMontage requires positive, constant playback speed.");
	}
	if (!Montage->bEnableAutoBlendOut)
	{
		return TEXT("AttackMontage requires automatic blend-out to finish.");
	}
	if (Settings->ComboSteps.Num() < Montage->GetNumSections())
	{
		return TEXT("CombatSettings must define distances for every attack section.");
	}
	if (Montage->GetNumSections() == 3)
	{
		const TArray<FVampsCombatStepSettings>& Steps = Settings->ComboSteps;
		if (!(Steps[2].KnockbackDistance > FMath::Max(Steps[0].KnockbackDistance, Steps[1].KnockbackDistance)))
		{
			return TEXT("The third combo step must have the largest knockback distance.");
		}
	}
	for (int32 SectionIndex = 0; SectionIndex < Montage->GetNumSections(); ++SectionIndex)
	{
		float SectionStart;
		float SectionEnd;
		Montage->GetSectionStartAndEndTime(SectionIndex, SectionStart, SectionEnd);
		const FString SectionContext = FString::Printf(TEXT("Montage '%s', section '%s' [%.6f, %.6f]"),
		                                               *Montage->GetPathName(),
		                                               *Montage->GetSectionName(SectionIndex).ToString(),
		                                               SectionStart,
		                                               SectionEnd);
		const FVampsCombatStepSettings& StepSettings = Settings->ComboSteps[SectionIndex];
		if (!FMath::IsFinite(StepSettings.ContactReactionDuration) || StepSettings.ContactReactionDuration <= 0.f)
		{
			return FString::Printf(TEXT("%s: ContactReactionDuration must be finite and positive; got %f seconds."),
			                       *SectionContext,
			                       StepSettings.ContactReactionDuration);
		}
		if (!FMath::IsFinite(StepSettings.TargetAlignmentDistance) ||
		    !FMath::IsFinite(StepSettings.KnockbackDistance) ||
		    !(StepSettings.TargetAlignmentDistance > 0.f && StepSettings.KnockbackDistance > 0.f))
		{
			return SectionContext + TEXT(": ComboSteps requires positive alignment and knockback distances.");
		}
		const FAnimNotifyEvent* ContactNotify = nullptr;
		const FAnimNotifyEvent* ComboNotify = nullptr;
		for (const FAnimNotifyEvent& NotifyEvent : Montage->Notifies)
		{
			if (!NotifyEvent.NotifyStateClass ||
			    Montage->GetSectionIndexFromPosition(NotifyEvent.GetTriggerTime()) != SectionIndex)
			{
				continue;
			}
			if (NotifyEvent.NotifyStateClass->IsA<UANS_AttackTrace>())
			{
				if (ContactNotify)
				{
					return SectionContext + TEXT(": duplicate ANS_AttackTrace; keep exactly one state per section.");
				}
				ContactNotify = &NotifyEvent;
			}
			if (NotifyEvent.NotifyStateClass->IsA<UANS_ComboWindow>())
			{
				if (ComboNotify)
				{
					return SectionContext + TEXT(": duplicate ANS_ComboWindow; keep at most one state per section.");
				}
				ComboNotify = &NotifyEvent;
			}
		}
		if (!ContactNotify)
		{
			return SectionContext + TEXT(": missing ANS_AttackTrace. Add exactly one state on the montage notify "
			                             "track, not only on its source animation.");
		}
		const float ContactStart = ContactNotify->GetTriggerTime();
		const float ContactEnd = ContactNotify->GetEndTriggerTime();
		if (!(ContactStart > SectionStart && ContactEnd > ContactStart && ContactEnd < SectionEnd))
		{
			return FString::Printf(TEXT("%s: ANS_AttackTrace [%.6f, %.6f] must satisfy section start < trace start "
			                            "< trace end < section end."),
			                       *SectionContext,
			                       ContactStart,
			                       ContactEnd);
		}
		for (const FAnimNotifyEvent& NotifyEvent : Montage->Notifies)
		{
			const UAN_ResetWeaponHits* ResetNotify = Cast<UAN_ResetWeaponHits>(NotifyEvent.Notify);
			if (!ResetNotify || Montage->GetSectionIndexFromPosition(NotifyEvent.GetTriggerTime()) != SectionIndex)
			{
				continue;
			}
			const float ResetTime = NotifyEvent.GetTriggerTime();
			if (!(ResetTime > ContactStart && ResetTime < ContactEnd))
			{
				return FString::Printf(TEXT("%s: AN_ResetWeaponHits '%s' at %.6f must be strictly inside "
				                            "ANS_AttackTrace [%.6f, %.6f]."),
				                       *SectionContext,
				                       *ResetNotify->GetPathName(),
				                       ResetTime,
				                       ContactStart,
				                       ContactEnd);
			}
			if (ResetNotify->WeaponIndices.IsEmpty())
			{
				return FString::Printf(TEXT("%s: AN_ResetWeaponHits '%s' requires at least one explicit weapon index."),
				                       *SectionContext,
				                       *ResetNotify->GetPathName());
			}
			for (int32 WeaponIndex : ResetNotify->WeaponIndices)
			{
				if (!Settings->Weapons.IsValidIndex(WeaponIndex))
				{
					return FString::Printf(TEXT("%s: AN_ResetWeaponHits '%s' has invalid weapon index %d; "
					                            "CombatSettings '%s' defines %d weapons."),
					                       *SectionContext,
					                       *ResetNotify->GetPathName(),
					                       WeaponIndex,
					                       *Settings->GetPathName(),
					                       Settings->Weapons.Num());
				}
			}
		}
		if (!ComboNotify && SectionIndex != Montage->GetNumSections() - 1)
		{
			return SectionContext + TEXT(": missing ANS_ComboWindow before the final attack section.");
		}
		if (ComboNotify)
		{
			const float BlendOutLead =
			    Montage->BlendOutTriggerTime >= 0.f ? Montage->BlendOutTriggerTime : Montage->BlendOut.GetBlendTime();
			const float ComboStart = ComboNotify->GetTriggerTime();
			const float ComboEnd = ComboNotify->GetEndTriggerTime();
			const float LatestComboEnd = SectionEnd - BlendOutLead * Montage->RateScale;
			if (!(ComboStart > ContactStart && ComboEnd > ContactEnd && ComboEnd > ComboStart &&
			      ComboEnd < LatestComboEnd))
			{
				return FString::Printf(
				    TEXT("%s: ANS_ComboWindow [%.6f, %.6f] must start after trace start %.6f, end after trace end "
				         "%.6f and its own start, and end before blend-out %.6f."),
				    *SectionContext,
				    ComboStart,
				    ComboEnd,
				    ContactStart,
				    ContactEnd,
				    LatestComboEnd);
			}
		}
	}
	return {};
}

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
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: ability '%s' requires a valid VampsCharacter avatar; got '%s'."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(GetAvatarActorFromActorInfo()));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	const FString ConfigurationError = ValidateAttackConfiguration(*Fighter);
	if (!ConfigurationError.IsEmpty())
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: attack rejected. CombatSettings '%s': %s"),
		       __FUNCTION__,
		       *Fighter->GetName(),
		       *GetPathNameSafe(Fighter->CombatSettings),
		       *ConfigurationError);
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogVampsCombat,
		       Warning,
		       TEXT("%hs: %s: CommitAbility rejected the attack; check ability costs, cooldowns and activation "
		            "requirements."),
		       __FUNCTION__,
		       *Fighter->GetPathName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAnimMontage* Montage = Fighter->CombatSettings->AttackMontage;
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
	    this, NAME_None, Montage, 1.f, NAME_None, true, 1.f, 0.f, true);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_Attack::FinishAttack);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_Attack::FinishAttack);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_Attack::FinishAttack);
	MontageTask->ReadyForActivation();
	if (IsActive())
	{
		UAnimInstance* Animation = Fighter->GetMesh()->GetAnimInstance();
		FAnimMontageInstance* Playback = Animation ? Animation->GetActiveInstanceForMontage(Montage) : nullptr;
		if (!Playback)
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: %s: attack rejected; montage '%s' did not start. Verify the mesh, AnimBP, skeleton and "
			            "slot."),
			       __FUNCTION__,
			       *Fighter->GetName(),
			       *Montage->GetPathName());
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}
		for (int32 SectionIndex = 0; SectionIndex < Montage->GetNumSections(); ++SectionIndex)
		{
			Playback->SetNextSectionID(SectionIndex, INDEX_NONE);
		}
		if (!Fighter->BeginCombatAction(Playback->GetInstanceID()))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}
	}
	else
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: montage task for '%s' ended before combat began. Verify the mesh, AnimBP, skeleton and "
		            "slot."),
		       __FUNCTION__,
		       *Fighter->GetPathName(),
		       *Montage->GetPathName());
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
