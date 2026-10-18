#include "Characters/VampsCharacter.h"
#include "Characters/VampsLockOnComponent.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Combat/Abilities/GA_Attack.h"
#include "Combat/Attributes/VampsAttributeSet.h"
#include "Combat/Data/VampsCombatSettings.h"
#include "Combat/VampsCombatTags.h"
#include "Combat/Effects/GE_VampsDamage.h"
#include "Combat/VampsWeapon.h"
#include "Combat/VampsCombatResolver.h"
#include "Combat/VampsCombatMovementComponent.h"
#include "Combat/VampsCombatLog.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"

AVampsCharacter::AVampsCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UVampsCombatMovementComponent>(
          ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;
	AutoPossessAI = EAutoPossessAI::Disabled;
	GetCapsuleComponent()->InitCapsuleSize(34.f, 88.f);
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	GetMesh()->bEnableUpdateRateOptimizations = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 420.f;

	AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
	AbilitySystem->SetIsReplicated(true);
	Attributes = CreateDefaultSubobject<UVampsAttributeSet>(TEXT("Attributes"));
	LockOn = CreateDefaultSubobject<UVampsLockOnComponent>(TEXT("LockOn"));
}

void AVampsCharacter::BeginPlay()
{
	AbilitySystem->InitAbilityActorInfo(this, this);
	AbilitySystem->GetGameplayAttributeValueChangeDelegate(UVampsAttributeSet::GetHealthAttribute())
	    .AddUObject(this, &AVampsCharacter::HandleCombatHealthChanged);
	if (InitializeCombatWeapons())
	{
		if (HasAuthority())
		{
			AttackHandle = AbilitySystem->GiveAbility(FGameplayAbilitySpec(UGA_Attack::StaticClass(), 1));
		}
	}
	else
	{
		DestroyCombatWeapons();
	}
	SetCombatPhase(EVampsActionPhase::Idle);
	Super::BeginPlay();
}

bool AVampsCharacter::InitializeCombatWeapons()
{
	if (!CombatSettings || CombatSettings->Weapons.IsEmpty())
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: CombatSettings '%s' must define at least one weapon."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(CombatSettings));
		return false;
	}
	if (!Cast<UVampsCombatMovementComponent>(GetCharacterMovement()))
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: expected VampsCombatMovementComponent, got '%s'."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(GetCharacterMovement()));
		return false;
	}
	for (const FVampsWeaponSettings& WeaponSettings : CombatSettings->Weapons)
	{
		if (!WeaponSettings.WeaponClass || WeaponSettings.WeaponClass->HasAnyClassFlags(CLASS_Abstract))
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: %s: CombatSettings '%s' requires a concrete weapon class; got '%s'."),
			       __FUNCTION__,
			       *GetPathName(),
			       *CombatSettings->GetPathName(),
			       *GetPathNameSafe(WeaponSettings.WeaponClass.Get()));
			return false;
		}
		if (WeaponSettings.AttachSocket.IsNone() || !GetMesh()->DoesSocketExist(WeaponSettings.AttachSocket))
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: %s: weapon '%s' cannot attach; mesh '%s' has no socket '%s'."),
			       __FUNCTION__,
			       *GetPathName(),
			       *WeaponSettings.WeaponClass->GetPathName(),
			       *GetPathNameSafe(GetMesh()->GetSkeletalMeshAsset()),
			       *WeaponSettings.AttachSocket.ToString());
			return false;
		}
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;
		SpawnParameters.Instigator = this;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AVampsWeapon* EquippedWeapon =
		    GetWorld()->SpawnActor<AVampsWeapon>(WeaponSettings.WeaponClass, GetActorTransform(), SpawnParameters);
		if (!IsValid(EquippedWeapon))
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: %s: spawning weapon '%s' failed or its BeginPlay rejected the configuration."),
			       __FUNCTION__,
			       *GetPathName(),
			       *WeaponSettings.WeaponClass->GetPathName());
			return false;
		}
		Weapons.Add(EquippedWeapon);
		const bool bWeaponAttached = EquippedWeapon->AttachToComponent(
		    GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSettings.AttachSocket);
		if (!bWeaponAttached)
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: %s: attaching weapon '%s' to socket '%s' failed."),
			       __FUNCTION__,
			       *GetPathName(),
			       *EquippedWeapon->GetPathName(),
			       *WeaponSettings.AttachSocket.ToString());
			return false;
		}
		EquippedWeapon->SetActorRelativeTransform(WeaponSettings.RelativeTransform);
	}
	return true;
}

void AVampsCharacter::EndPlay(const EEndPlayReason::Type Reason)
{
	AbilitySystem->GetGameplayAttributeValueChangeDelegate(UVampsAttributeSet::GetHealthAttribute()).RemoveAll(this);
	ResetCombatState();
	DestroyCombatWeapons();
	Super::EndPlay(Reason);
}

void AVampsCharacter::DestroyCombatWeapons()
{
	for (AVampsWeapon* EquippedWeapon : Weapons)
	{
		if (IsValid(EquippedWeapon))
		{
			EquippedWeapon->Destroy();
		}
	}
	Weapons.Reset();
}

void AVampsCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AbilitySystem->InitAbilityActorInfo(this, this);
}

void AVampsCharacter::UnPossessed()
{
	ResetCombatState();
	Super::UnPossessed();
	AbilitySystem->InitAbilityActorInfo(this, this);
}

UAbilitySystemComponent* AVampsCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

void AVampsCharacter::AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce)
{
	Super::AddMovementInput(WorldDirection, ScaleValue, bForce);
	if (HasAuthority() && !WorldDirection.IsNearlyZero() && !FMath::IsNearlyZero(ScaleValue) &&
	    CombatState.Phase == EVampsActionPhase::Recovery)
	{
		GetWorld()->GetSubsystem<UVampsCombatResolver>()->QueueCombatLocomotion(this);
	}
}

void AVampsCharacter::RequestCombatAction()
{
	if (!HasAuthority())
	{
		return;
	}

	++CombatState.RequestCount;
	CombatState.LastRequest = EVampsCombatRequest::Rejected;
	if (!AttackHandle.IsValid())
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: attack ability is unavailable; combat initialization did not complete. See earlier "
		            "configuration errors."),
		       __FUNCTION__,
		       *GetPathName());
		return;
	}
	if (!CombatState.bCanRequestCombat)
	{
		return;
	}

	if (CombatState.bCanStartAction)
	{
		if (AbilitySystem->TryActivateAbility(AttackHandle) && CombatState.MontageInstanceID != INDEX_NONE)
		{
			CombatState.LastRequest = EVampsCombatRequest::ActionStarted;
		}
		return;
	}

	if (CombatState.Phase != EVampsActionPhase::Recovery)
	{
		CombatState.bCanDefend = true;
		CombatState.LastRequest = EVampsCombatRequest::DefenseRearmed;
	}
	if (CombatState.bComboWindowOpen && !CombatState.bOffenseRestricted)
	{
		if (!CombatSettings || !CombatSettings->AttackMontage)
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: %s: cannot request a combo continuation without CombatSettings and AttackMontage."),
			       __FUNCTION__,
			       *GetPathName());
			return;
		}
		if (CombatState.ComboStep >= CombatSettings->AttackMontage->GetNumSections())
		{
			return;
		}
		CombatState.bContinuationRequested = true;
		CombatState.LastRequest = EVampsCombatRequest::ContinuationRequested;
	}
}

bool AVampsCharacter::BeginCombatAction(int32 PlaybackInstanceID)
{
	if (!CombatState.bCanStartAction || PlaybackInstanceID == INDEX_NONE)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: cannot begin attack; CanStart=%d, montage instance=%d."),
		       __FUNCTION__,
		       *GetPathName(),
		       CombatState.bCanStartAction,
		       PlaybackInstanceID);
		return false;
	}
	CombatState.MontageInstanceID = PlaybackInstanceID;
	CombatState.ComboStep = 1;
	if (!BeginCombatStep())
	{
		CombatState.MontageInstanceID = INDEX_NONE;
		CombatState.ComboStep = 0;
		return false;
	}
	return true;
}

bool AVampsCharacter::BeginCombatStep()
{
	float SectionDuration;
	if (!TryCalculateCombatSectionDuration(SectionDuration))
	{
		return false;
	}
	for (AVampsWeapon* EquippedWeapon : Weapons)
	{
		if (!IsValid(EquippedWeapon))
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: %s: combo step %d has a missing or destroyed weapon."),
			       __FUNCTION__,
			       *GetPathName(),
			       CombatState.ComboStep);
			return false;
		}
	}
	++CombatState.ActionNumber;
	CombatState.bCanDefend = true;
	CombatState.bActionConnected = false;
	CombatState.bComboWindowOpen = false;
	CombatState.bContinuationRequested = false;
	CombatState.bRecoveryVisualReleased = false;
	CombatStepEndsAt = GetWorld()->GetTimeSeconds() + SectionDuration;
	CombatState.LastOutgoingContact = EVampsContactResult::None;
	CombatState.LastOutgoingContactFrame = 0;
	for (AVampsWeapon* EquippedWeapon : Weapons)
	{
		EquippedWeapon->ResetSectionHits();
	}
	SetCombatPhase(EVampsActionPhase::Preparation);
	return true;
}

void AVampsCharacter::FinishCombatAction()
{
	if (CombatState.MontageInstanceID == INDEX_NONE)
	{
		return;
	}
	if (!CombatState.bActionConnected)
	{
		CombatState.LastOutgoingContact = EVampsContactResult::Miss;
		CombatState.LastOutgoingContactFrame = GFrameCounter;
	}
	CombatState.MontageInstanceID = INDEX_NONE;
	CombatState.bComboWindowOpen = false;
	CombatState.bContinuationRequested = false;
	if (!CombatState.bIsAlive)
	{
		SetCombatPhase(EVampsActionPhase::Idle);
		return;
	}
	const double RemainingRecovery = CombatStepEndsAt - GetWorld()->GetTimeSeconds();
	if (CombatState.bRecoveryVisualReleased && RemainingRecovery > 0)
	{
		SetCombatPhase(EVampsActionPhase::Recovery);
		GetWorldTimerManager().SetTimer(
		    ActionRecoveryTimer, this, &AVampsCharacter::BeginNARecovery, float(RemainingRecovery));
		return;
	}
	BeginNARecovery();
}

void AVampsCharacter::BeginNARecovery()
{
	if (!CombatState.bIsAlive)
	{
		SetCombatPhase(EVampsActionPhase::Idle);
		return;
	}
	if (!CombatSettings || !FMath::IsFinite(CombatSettings->NARecoveryDuration) ||
	    CombatSettings->NARecoveryDuration <= 0.f)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: CombatSettings '%s' must define a finite, positive NARecoveryDuration. Combat remains "
		            "unavailable until reset."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(CombatSettings));
		CombatState.NARecoveryEndsAt = 0;
		SetCombatPhase(EVampsActionPhase::NARecovery);
		return;
	}
	CombatState.NARecoveryEndsAt = GetWorld()->GetTimeSeconds() + CombatSettings->NARecoveryDuration;
	SetCombatPhase(EVampsActionPhase::NARecovery);
	GetWorldTimerManager().SetTimer(
	    NARecoveryTimer, this, &AVampsCharacter::FinishNARecovery, CombatSettings->NARecoveryDuration);
}

bool AVampsCharacter::IsCurrentCombatNotify(const FBranchingPointNotifyPayload& Payload) const
{
	if (!HasAuthority() || CombatState.MontageInstanceID == INDEX_NONE)
	{
		return false;
	}
	if (!CombatSettings || !CombatSettings->AttackMontage || !Payload.NotifyEvent)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: active combat notify has missing CombatSettings, AttackMontage or NotifyEvent."),
		       __FUNCTION__,
		       *GetPathName());
		return false;
	}
	return Payload.SkelMeshComponent == GetMesh() && Payload.SequenceAsset == CombatSettings->AttackMontage &&
	       Payload.MontageInstanceID == CombatState.MontageInstanceID && Payload.NotifyEvent &&
	       CombatSettings->AttackMontage->GetSectionIndexFromPosition(Payload.NotifyEvent->GetTriggerTime()) ==
	           CombatState.ComboStep - 1;
}

void AVampsCharacter::SetCombatContacts(const FBranchingPointNotifyPayload& Payload, bool bEnabled)
{
	if (!IsCurrentCombatNotify(Payload))
	{
		return;
	}

	if (bEnabled)
	{
		if (CombatState.Phase != EVampsActionPhase::Preparation)
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: %s: AttackTrace begin expected Preparation, got phase %d in step %d. Attack cancelled."),
			       __FUNCTION__,
			       *GetPathName(),
			       int32(CombatState.Phase),
			       CombatState.ComboStep);
			AbilitySystem->CancelAbilityHandle(AttackHandle);
			return;
		}
		SetCombatPhase(EVampsActionPhase::Contacts);
		return;
	}
	if (CombatState.Phase != EVampsActionPhase::Contacts)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: AttackTrace end expected Contacts, got phase %d in step %d. Attack cancelled."),
		       __FUNCTION__,
		       *GetPathName(),
		       int32(CombatState.Phase),
		       CombatState.ComboStep);
		AbilitySystem->CancelAbilityHandle(AttackHandle);
		return;
	}
	SetCombatPhase(EVampsActionPhase::Recovery);
}

void AVampsCharacter::ResetCombatWeaponHits(const FBranchingPointNotifyPayload& Payload,
                                            const TArray<int32>& WeaponIndices)
{
	if (!IsCurrentCombatNotify(Payload))
	{
		return;
	}
	if (CombatState.Phase != EVampsActionPhase::Contacts || WeaponIndices.IsEmpty())
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: notify '%s' rejected in step %d; expected Contacts and at least one weapon index, "
		            "got phase %d and %d indices."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(Payload.NotifyEvent->Notify),
		       CombatState.ComboStep,
		       int32(CombatState.Phase),
		       WeaponIndices.Num());
		return;
	}
	for (int32 WeaponIndex : WeaponIndices)
	{
		if (!Weapons.IsValidIndex(WeaponIndex) || !IsValid(Weapons[WeaponIndex]))
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: %s: notify '%s' rejected; weapon index %d must select a valid equipped weapon from "
			            "CombatSettings '%s' (%d weapons). No weapon hits were reset."),
			       __FUNCTION__,
			       *GetPathName(),
			       *GetPathNameSafe(Payload.NotifyEvent->Notify),
			       WeaponIndex,
			       *GetPathNameSafe(CombatSettings),
			       Weapons.Num());
			return;
		}
	}
	for (int32 WeaponIndex : WeaponIndices)
	{
		Weapons[WeaponIndex]->ResetImpactHits();
	}
}

void AVampsCharacter::SetCombatComboWindow(const FBranchingPointNotifyPayload& Payload, bool bEnabled)
{
	if (!IsCurrentCombatNotify(Payload))
	{
		return;
	}
	CombatState.bComboWindowOpen = bEnabled;
	SetCombatPhase(CombatState.Phase);
	if (!bEnabled && CombatState.bContinuationRequested)
	{
		GetWorld()->GetSubsystem<UVampsCombatResolver>()->QueueCombatContinuation(this);
	}
}

void AVampsCharacter::ResolveCombatContinuation()
{
	if (!CombatState.bIsAlive || CombatState.bComboWindowOpen || !CombatState.bContinuationRequested)
	{
		return;
	}
	CombatState.bContinuationRequested = false;
	if (CombatState.bOffenseRestricted || !CombatState.bActionConnected)
	{
		CombatState.LastRequest = EVampsCombatRequest::Rejected;
		return;
	}
	UAnimMontage* Montage = CombatSettings ? CombatSettings->AttackMontage.Get() : nullptr;
	if (!Montage || !Montage->IsValidSectionIndex(CombatState.ComboStep))
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: montage '%s' has no next section at index %d. Attack cancelled."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(Montage),
		       CombatState.ComboStep);
		AbilitySystem->CancelAbilityHandle(AttackHandle);
		return;
	}
	UAnimInstance* Animation = GetMesh()->GetAnimInstance();
	const FAnimMontageInstance* Playback =
	    Animation ? Animation->GetMontageInstanceForID(CombatState.MontageInstanceID) : nullptr;
	if (!Playback || Playback->Montage != Montage)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: missing or mismatched montage instance %d for '%s'. Attack cancelled."),
		       __FUNCTION__,
		       *GetPathName(),
		       CombatState.MontageInstanceID,
		       *Montage->GetPathName());
		AbilitySystem->CancelAbilityHandle(AttackHandle);
		return;
	}
	if (Playback->IsStopped())
	{
		CombatState.LastRequest = EVampsCombatRequest::Rejected;
		return;
	}
	const FName NextSection = Montage->GetSectionName(CombatState.ComboStep);
	++CombatState.ComboStep;
	if (!BeginCombatStep())
	{
		--CombatState.ComboStep;
		AbilitySystem->CancelAbilityHandle(AttackHandle);
		return;
	}
	Animation->Montage_JumpToSection(NextSection, Montage);
	CombatState.LastRequest = EVampsCombatRequest::ComboAdvanced;
}

void AVampsCharacter::ResolveCombatLocomotion()
{
	if (!CombatState.bIsAlive || CombatState.Phase != EVampsActionPhase::Recovery || CombatState.bComboWindowOpen ||
	    CombatState.bRecoveryVisualReleased)
	{
		return;
	}
	CombatState.bRecoveryVisualReleased = true;
	AbilitySystem->CancelAbilityHandle(AttackHandle);
}

bool AVampsCharacter::TryCalculateCombatSectionDuration(float& SectionDuration) const
{
	const UAnimMontage* Montage = CombatSettings ? CombatSettings->AttackMontage.Get() : nullptr;
	if (!Montage || !Montage->IsValidSectionIndex(CombatState.ComboStep - 1))
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: cannot calculate step %d duration; CombatSettings '%s', montage '%s'."),
		       __FUNCTION__,
		       *GetPathName(),
		       CombatState.ComboStep,
		       *GetPathNameSafe(CombatSettings),
		       *GetPathNameSafe(Montage));
		return false;
	}
	UAnimInstance* Animation = GetMesh()->GetAnimInstance();
	const FAnimMontageInstance* Playback =
	    Animation ? Animation->GetMontageInstanceForID(CombatState.MontageInstanceID) : nullptr;
	if (!Playback || Playback->Montage != Montage)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: cannot calculate duration; montage instance %d is missing or does not play '%s'."),
		       __FUNCTION__,
		       *GetPathName(),
		       CombatState.MontageInstanceID,
		       *Montage->GetPathName());
		return false;
	}
	const float EffectivePlayRate = Playback->GetPlayRate() * Montage->RateScale;
	if (!FMath::IsFinite(EffectivePlayRate) || EffectivePlayRate <= 0.f)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: montage '%s' requires a finite positive play rate; got %f."),
		       __FUNCTION__,
		       *GetPathName(),
		       *Montage->GetPathName(),
		       EffectivePlayRate);
		return false;
	}
	const float CalculatedDuration = Montage->GetSectionLength(CombatState.ComboStep - 1) / EffectivePlayRate;
	if (!FMath::IsFinite(CalculatedDuration) || CalculatedDuration <= 0.f)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: montage '%s', step %d has invalid duration %f."),
		       __FUNCTION__,
		       *GetPathName(),
		       *Montage->GetPathName(),
		       CombatState.ComboStep,
		       CalculatedDuration);
		return false;
	}
	SectionDuration = CalculatedDuration;
	return true;
}

void AVampsCharacter::ApplyOffensiveRestriction(float RestrictionDuration)
{
	if (!HasAuthority() || !FMath::IsFinite(RestrictionDuration) || RestrictionDuration <= 0.f)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: offensive restriction requires authority and finite positive duration; authority=%d, "
		            "duration=%f."),
		       __FUNCTION__,
		       *GetPathName(),
		       HasAuthority(),
		       RestrictionDuration);
		return;
	}
	const double Now = GetWorld()->GetTimeSeconds();
	CombatState.OffensiveRestrictionEndsAt =
	    FMath::Max(CombatState.OffensiveRestrictionEndsAt, Now + RestrictionDuration);
	CombatState.bOffenseRestricted = true;
	if (CombatState.bContinuationRequested)
	{
		CombatState.LastRequest = EVampsCombatRequest::Rejected;
	}
	CombatState.bContinuationRequested = false;
	SetCombatPhase(CombatState.Phase);
	GetWorldTimerManager().SetTimer(OffensiveRestrictionTimer,
	                                this,
	                                &AVampsCharacter::FinishOffensiveRestriction,
	                                float(CombatState.OffensiveRestrictionEndsAt - Now));
}

void AVampsCharacter::FinishOffensiveRestriction()
{
	CombatState.bOffenseRestricted = false;
	CombatState.OffensiveRestrictionEndsAt = 0;
	SetCombatPhase(CombatState.Phase);
}

void AVampsCharacter::FinishNARecovery()
{
	CombatState.NARecoveryEndsAt = 0;
	CombatState.ComboStep = 0;
	SetCombatPhase(EVampsActionPhase::Idle);
}

void AVampsCharacter::ConsumeDefenseRequest()
{
	CombatState.bCanDefend = false;
}

void AVampsCharacter::RegisterCombatContact(bool bBlocked)
{
	const EVampsContactResult ContactResult = bBlocked ? EVampsContactResult::Blocked : EVampsContactResult::OpenHit;
	CombatState.LastOutgoingContactFrame = GFrameCounter;
	CombatState.LastOutgoingContact = CombatState.bActionConnected && CombatState.LastOutgoingContact != ContactResult
	                                      ? EVampsContactResult::Mixed
	                                      : ContactResult;
	CombatState.bActionConnected = true;
}

void AVampsCharacter::ResetCombatState()
{
	AbilitySystem->CancelAllAbilities();
	GetWorldTimerManager().ClearTimer(NARecoveryTimer);
	GetWorldTimerManager().ClearTimer(ActionRecoveryTimer);
	GetWorldTimerManager().ClearTimer(OffensiveRestrictionTimer);
	GetWorld()->GetSubsystem<UVampsCombatResolver>()->DiscardContactsFor(this);
	for (AVampsWeapon* EquippedWeapon : Weapons)
	{
		if (IsValid(EquippedWeapon))
		{
			EquippedWeapon->ResetSectionHits();
		}
	}
	UVampsCombatMovementComponent* CombatMovement = Cast<UVampsCombatMovementComponent>(GetCharacterMovement());
	if (CombatMovement)
	{
		CombatMovement->ResetCombatMotion();
	}
	else
	{
		UE_LOG(
		    LogVampsCombat,
		    Error,
		    TEXT("%hs: %s: cannot reset combat motion; movement component '%s' is not VampsCombatMovementComponent."),
		    __FUNCTION__,
		    *GetPathName(),
		    *GetPathNameSafe(GetCharacterMovement()));
	}
	CombatStepEndsAt = 0;
	CombatState = FVampsCombatState();
	SetCombatPhase(EVampsActionPhase::Idle);
}

void AVampsCharacter::SetCombatPhase(EVampsActionPhase NewPhase)
{
	const bool bWasDamageEnabled = CombatState.bCanDealDamage;
	CombatState.Phase = NewPhase;
	CombatState.bIsAlive = Attributes->GetHealth() > 0.f;
	CombatState.bIsRecovering = NewPhase == EVampsActionPhase::Recovery || NewPhase == EVampsActionPhase::NARecovery;
	CombatState.bCanRequestCombat = CombatState.bIsAlive && AttackHandle.IsValid() &&
	                                NewPhase != EVampsActionPhase::NARecovery &&
	                                (NewPhase != EVampsActionPhase::Recovery || CombatState.bComboWindowOpen);
	CombatState.bCanStartAction = CombatState.bIsAlive && AttackHandle.IsValid() &&
	                              NewPhase == EVampsActionPhase::Idle && !CombatState.bOffenseRestricted;
	CombatState.bCanDealDamage = CombatState.bIsAlive && NewPhase == EVampsActionPhase::Contacts;
	CombatState.bCanDefend = CombatState.bCanDefend && CombatState.bIsAlive &&
	                         (NewPhase == EVampsActionPhase::Preparation || NewPhase == EVampsActionPhase::Contacts ||
	                          (NewPhase == EVampsActionPhase::Idle && CombatState.bOffenseRestricted));

	if (bWasDamageEnabled == CombatState.bCanDealDamage)
	{
		return;
	}
	for (AVampsWeapon* EquippedWeapon : Weapons)
	{
		if (!IsValid(EquippedWeapon))
		{
			if (CombatState.bCanDealDamage)
			{
				UE_LOG(LogVampsCombat,
				       Error,
				       TEXT("%hs: %s: cannot enable contacts with missing weapon '%s'. Attack cancelled."),
				       __FUNCTION__,
				       *GetPathName(),
				       *GetPathNameSafe(EquippedWeapon));
				AbilitySystem->CancelAbilityHandle(AttackHandle);
				return;
			}
			continue;
		}
		if (!EquippedWeapon->SetTracingEnabled(CombatState.bCanDealDamage))
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: %s: weapon '%s' rejected tracing configuration. Attack cancelled; see the weapon error "
			            "above."),
			       __FUNCTION__,
			       *GetPathName(),
			       *EquippedWeapon->GetPathName());
			AbilitySystem->CancelAbilityHandle(AttackHandle);
			return;
		}
	}
}

void AVampsCharacter::HandleCombatHealthChanged(const FOnAttributeChangeData&)
{
	SetCombatPhase(CombatState.Phase);
	if (!CombatState.bIsAlive)
	{
		UVampsCombatMovementComponent* CombatMovement = Cast<UVampsCombatMovementComponent>(GetCharacterMovement());
		if (!CombatMovement)
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: %s: cannot clear motion after death; invalid combat movement component '%s'."),
			       __FUNCTION__,
			       *GetPathName(),
			       *GetPathNameSafe(GetCharacterMovement()));
			return;
		}
		CombatMovement->ResetCombatMotion();
	}
}

void AVampsCharacter::ReceiveCombatHit(AVampsCharacter* Attacker, bool bBlocked)
{
	if (!IsValid(Attacker) || Attacker == this)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: expected a different valid attacker, got '%s'."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(Attacker));
		return;
	}
	if (!HasAuthority())
	{
		return;
	}

	CombatState.LastIncomingContact = bBlocked ? EVampsContactResult::Blocked : EVampsContactResult::OpenHit;
	CombatState.LastIncomingContactFrame = GFrameCounter;
	if (bBlocked)
	{
		return;
	}

	FGameplayEffectContextHandle Context = Attacker->AbilitySystem->MakeEffectContext();
	Context.AddSourceObject(Attacker);
	FGameplayEffectSpecHandle Damage =
	    Attacker->AbilitySystem->MakeOutgoingSpec(UGE_VampsDamage::StaticClass(), 1, Context);
	if (!Damage.IsValid() || !FMath::IsFinite(Attacker->AttackDamage) || Attacker->AttackDamage < 0.f)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: cannot apply damage from '%s'; valid effect spec=%d, AttackDamage=%f (expected finite "
		            "and nonnegative)."),
		       __FUNCTION__,
		       *GetPathName(),
		       *Attacker->GetPathName(),
		       Damage.IsValid(),
		       Attacker->AttackDamage);
		return;
	}
	Damage.Data->SetSetByCallerMagnitude(VampsTags::Damage, -Attacker->AttackDamage);
	Attacker->AbilitySystem->ApplyGameplayEffectSpecToTarget(*Damage.Data, AbilitySystem);
}
