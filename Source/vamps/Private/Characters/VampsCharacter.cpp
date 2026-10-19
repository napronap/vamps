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
	PrimaryActorTick.bCanEverTick = true;
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
	if (CombatSettings && CombatSettings->HasValidReactionSettings() && CombatSettings->HasValidMobilitySettings() &&
	    InitializeCombatWeapons())
	{
		if (HasAuthority())
		{
			AttackHandle = AbilitySystem->GiveAbility(FGameplayAbilitySpec(UGA_Attack::StaticClass(), 1));
			AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetMaxGuardLoadAttribute(),
			                                       CombatSettings->GuardCapacity);
			AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetMaxMobilityEnergyAttribute(),
			                                       CombatSettings->MobilityEnergyCapacity);
			AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetMobilityEnergyAttribute(),
			                                       CombatSettings->MobilityEnergyCapacity);
		}
	}
	else
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': combat initialization failed; CombatSettings='%s'. See configuration diagnostics."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(CombatSettings));
		DestroyCombatWeapons();
	}
	SetCombatPhase(EVampsActionPhase::Idle);
	Super::BeginPlay();
}

void AVampsCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!HasAuthority() || !CombatState.bIsAlive || !AttackHandle.IsValid())
	{
		return;
	}
	const double Now = GetWorld()->GetTimeSeconds();
	if (CombatState.Reaction != EVampsCombatReaction::None && Now >= CombatState.ReactionEndsAt)
	{
		FinishCombatReaction();
	}
	if (CombatState.bDefenseRequestArmed && Now >= CombatState.DefenseRequestEndsAt)
	{
		CombatState.bDefenseRequestArmed = false;
	}
	if (CombatState.DefendedAttacker.IsValid() && CombatState.DefendedAttacker->CombatGeneration != DefendedGeneration)
	{
		CombatState.DefendedAttacker.Reset();
		CombatState.DefendedActionNumber = INDEX_NONE;
		CombatState.bGuardDashReady = false;
	}
	if (CombatState.Reaction == EVampsCombatReaction::None && Now >= GuardRecoveryStartsAt &&
	    Attributes->GetGuardLoad() > 0.f)
	{
		const float RecoverySeconds = FMath::Min(DeltaSeconds, float(Now - GuardRecoveryStartsAt));
		AbilitySystem->SetNumericAttributeBase(
		    UVampsAttributeSet::GetGuardLoadAttribute(),
		    FMath::Max(0.f, Attributes->GetGuardLoad() - CombatSettings->GuardRecoveryPerSecond * RecoverySeconds));
	}
	SetCombatPhase(CombatState.Phase);
	UpdateMobilityEnergy(DeltaSeconds);
}

void AVampsCharacter::SetMobilityDirection(const FVector& WorldDirection)
{
	if (WorldDirection.ContainsNaN())
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': invalid mobility direction %s."),
		       __FUNCTION__,
		       *GetPathName(),
		       *WorldDirection.ToString());
		ClearMobilityIntent();
		return;
	}
	MobilityDirection = WorldDirection.GetSafeNormal2D();
}

void AVampsCharacter::ClearMobilityIntent()
{
	bMobilityHeld = false;
	CombatState.bIsSprinting = false;
	CombatState.bDashRequested = false;
	CombatState.bGuardDashReady = false;
	DashRequestActionNumber = INDEX_NONE;
}

void AVampsCharacter::SetMobilityHeld(bool bHeld)
{
	if (!HasAuthority())
	{
		return;
	}
	if (!bHeld)
	{
		bMobilityHeld = false;
		CombatState.bIsSprinting = false;
		return;
	}
	SetCombatPhase(CombatState.Phase);
	const UVampsCombatMovementComponent* Movement = Cast<UVampsCombatMovementComponent>(GetCharacterMovement());
	if (!Movement)
	{
		UE_LOG(
		    LogVampsCombat, Error, TEXT("%hs: '%s': missing combat movement component."), __FUNCTION__, *GetPathName());
		return;
	}
	if (!CombatState.bCanDash || Movement->CombatMotion == EVampsCombatMotion::Dash ||
	    (CombatState.MontageInstanceID != INDEX_NONE && !CombatState.bComboWindowOpen &&
	     CombatState.Phase != EVampsActionPhase::Recovery))
	{
		return;
	}
	PendingDashDirection = MobilityDirection;
	if (PendingDashDirection.IsNearlyZero())
	{
		const AVampsCharacter* TargetFighter = LockOn ? LockOn->GetCurrentTarget() : nullptr;
		if (!IsValid(TargetFighter))
		{
			UE_LOG(LogVampsCombat,
			       Warning,
			       TEXT("%hs: '%s': backward dash requires an eligible target or directional input."),
			       __FUNCTION__,
			       *GetPathName());
			return;
		}
		PendingDashDirection = (GetActorLocation() - TargetFighter->GetActorLocation()).GetSafeNormal2D();
		if (PendingDashDirection.IsNearlyZero())
		{
			UE_LOG(LogVampsCombat,
			       Warning,
			       TEXT("%hs: '%s': backward dash direction is undefined at target '%s'."),
			       __FUNCTION__,
			       *GetPathName(),
			       *TargetFighter->GetPathName());
			return;
		}
	}
	bMobilityHeld = true;
	CombatState.bDashRequested = true;
	DashRequestActionNumber = CombatState.ActionNumber;
	GetWorld()->GetSubsystem<UVampsCombatResolver>()->QueueCombatDash(this);
}

void AVampsCharacter::ResolveCombatDash()
{
	if (!CombatState.bDashRequested)
	{
		return;
	}
	if (CombatState.MontageInstanceID != INDEX_NONE && CombatState.bComboWindowOpen)
	{
		return;
	}
	CombatState.bDashRequested = false;
	SetCombatPhase(CombatState.Phase);
	if (!CombatState.bCanDash || DashRequestActionNumber != CombatState.ActionNumber)
	{
		bMobilityHeld = false;
		return;
	}
	UVampsCombatMovementComponent* Movement = Cast<UVampsCombatMovementComponent>(GetCharacterMovement());
	UAnimMontage* DashMontage = Movement ? Movement->FindCombatDashMontage(PendingDashDirection) : nullptr;
	if (!DashMontage)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': dash cancelled; movement or authored montage is unavailable."),
		       __FUNCTION__,
		       *GetPathName());
		bMobilityHeld = false;
		return;
	}
	if (CombatState.MontageInstanceID != INDEX_NONE)
	{
		ResolveCombatLocomotion();
	}
	if (!Movement->TryStartCombatDash(PendingDashDirection, DashMontage))
	{
		bMobilityHeld = false;
		return;
	}
	if (CombatState.Reaction == EVampsCombatReaction::Guard)
	{
		FinishCombatReaction();
		CombatState.bDefenseRequestArmed = false;
		CombatState.DefenseRequestEndsAt = 0;
	}
	AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetMobilityEnergyAttribute(),
	                                       Attributes->GetMobilityEnergy() - CombatSettings->DashEnergyCost);
	MobilityRecoveryStartsAt = GetWorld()->GetTimeSeconds() + CombatSettings->MobilityRecoveryDelay;
	CombatState.bGuardDashReady = false;
	SetCombatPhase(CombatState.Phase);
}

void AVampsCharacter::UpdateMobilityEnergy(float DeltaSeconds)
{
	const UVampsCombatMovementComponent* Movement = Cast<UVampsCombatMovementComponent>(GetCharacterMovement());
	if (!Movement)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': energy update requires combat movement."),
		       __FUNCTION__,
		       *GetPathName());
		ClearMobilityIntent();
		return;
	}
	const bool bSprinting = bMobilityHeld && !CombatState.bDashRequested && !MobilityDirection.IsNearlyZero() &&
	                        CombatState.bCanMove && Movement->IsMovingOnGround() &&
	                        Movement->CombatMotion == EVampsCombatMotion::None &&
	                        (CombatState.MontageInstanceID == INDEX_NONE || CombatState.bRecoveryVisualReleased) &&
	                        Attributes->GetMobilityEnergy() > 0.f;
	CombatState.bIsSprinting = bSprinting;
	const double Now = GetWorld()->GetTimeSeconds();
	if (bSprinting)
	{
		const float Energy =
		    FMath::Max(0.f, Attributes->GetMobilityEnergy() - CombatSettings->SprintEnergyPerSecond * DeltaSeconds);
		AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetMobilityEnergyAttribute(), Energy);
		MobilityRecoveryStartsAt = Now + CombatSettings->MobilityRecoveryDelay;
		if (Energy <= 0.f)
		{
			bMobilityHeld = false;
			CombatState.bIsSprinting = false;
		}
	}
	else if (Now >= MobilityRecoveryStartsAt && Movement->CombatMotion != EVampsCombatMotion::Dash)
	{
		const float RecoverySeconds = FMath::Min(DeltaSeconds, float(Now - MobilityRecoveryStartsAt));
		AbilitySystem->SetNumericAttributeBase(
		    UVampsAttributeSet::GetMobilityEnergyAttribute(),
		    FMath::Min(Attributes->GetMaxMobilityEnergy(),
		               Attributes->GetMobilityEnergy() + CombatSettings->MobilityRecoveryPerSecond * RecoverySeconds));
	}
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
	if (!CombatState.bCanMove)
	{
		return;
	}
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

	SetCombatPhase(CombatState.Phase);
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
		ArmDefenseRequest();
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
	CombatState.bDashRequested = false;
	CombatState.bIsSprinting = false;
	ArmDefenseRequest();
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
	UVampsCombatMovementComponent* Movement = Cast<UVampsCombatMovementComponent>(GetCharacterMovement());
	if (!Movement)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': attack step requires combat movement."),
		       __FUNCTION__,
		       *GetPathName());
		return false;
	}
	Movement->BeginOffensiveAdvance(CombatSettings->ComboSteps[CombatState.ComboStep - 1]);
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
	if (CombatState.bDashRequested)
	{
		CombatState.bDashRequested = false;
		bMobilityHeld = false;
	}
	if (UVampsCombatMovementComponent* Movement = Cast<UVampsCombatMovementComponent>(GetCharacterMovement()))
	{
		Movement->StopVoluntaryMotion();
	}
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
	if (!bEnabled && CombatState.bDashRequested)
	{
		GetWorld()->GetSubsystem<UVampsCombatResolver>()->QueueCombatDash(this);
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

void AVampsCharacter::ArmDefenseRequest()
{
	CombatState.bDefenseRequestArmed = true;
	CombatState.DefenseRequestEndsAt = GetWorld()->GetTimeSeconds() + CombatSettings->DefenseRequestDuration;
	SetCombatPhase(CombatState.Phase);
}

bool AVampsCharacter::MatchesDefendedContact(const FVampsPendingContact& Contact) const
{
	return CombatState.DefendedAttacker == Contact.Attacker &&
	       CombatState.DefendedActionNumber == Contact.ActionNumber && DefendedGeneration == Contact.AttackerGeneration;
}

void AVampsCharacter::RememberDefendedContact(const FVampsPendingContact& Contact)
{
	CombatState.bDefenseRequestArmed = false;
	CombatState.DefenseRequestEndsAt = 0;
	CombatState.DefendedAttacker = Contact.Attacker;
	CombatState.DefendedActionNumber = Contact.ActionNumber;
	DefendedGeneration = Contact.AttackerGeneration;
	LockOn->SelectTarget(Contact.Attacker.Get());
}

void AVampsCharacter::CancelCombatOffense()
{
	if (UVampsCombatMovementComponent* Movement = Cast<UVampsCombatMovementComponent>(GetCharacterMovement()))
	{
		Movement->StopVoluntaryMotion();
	}
	CombatState.MontageInstanceID = INDEX_NONE;
	CombatState.bComboWindowOpen = false;
	CombatState.bContinuationRequested = false;
	CombatState.bRecoveryVisualReleased = false;
	CombatState.ComboStep = 0;
	CombatStepEndsAt = 0;
	GetWorldTimerManager().ClearTimer(ActionRecoveryTimer);
	SetCombatPhase(CombatState.Phase == EVampsActionPhase::NARecovery ? EVampsActionPhase::NARecovery
	                                                                  : EVampsActionPhase::Idle);
	AbilitySystem->CancelAbilityHandle(AttackHandle);
}

void AVampsCharacter::FinishCombatReaction()
{
	CombatState.bGuardDashReady = false;
	if (CombatState.Reaction == EVampsCombatReaction::Stun)
	{
		AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetGuardLoadAttribute(), 0.f);
		GuardRecoveryStartsAt = GetWorld()->GetTimeSeconds();
	}
	UAnimInstance* Animation = GetMesh()->GetAnimInstance();
	FAnimMontageInstance* Playback =
	    Animation ? Animation->GetMontageInstanceForID(ReactionPlaybackInstanceID) : nullptr;
	if (Playback)
	{
		Animation->Montage_Stop(0.05f, Playback->Montage);
	}
	ReactionPlaybackInstanceID = INDEX_NONE;
	CombatState.Reaction = EVampsCombatReaction::None;
	CombatState.ReactionEndsAt = 0;
	CombatState.ReactionSource.Reset();
	CombatState.DefendedAttacker.Reset();
	CombatState.DefendedActionNumber = INDEX_NONE;
	SetCombatPhase(CombatState.Phase);
}

void AVampsCharacter::FinishNARecovery()
{
	CombatState.NARecoveryEndsAt = 0;
	CombatState.ComboStep = 0;
	SetCombatPhase(EVampsActionPhase::Idle);
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
	ClearMobilityIntent();
	MobilityDirection = FVector::ZeroVector;
	PendingDashDirection = FVector::ZeroVector;
	MobilityRecoveryStartsAt = 0;
	if (CombatSettings && CombatSettings->HasValidMobilitySettings())
	{
		AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetMaxMobilityEnergyAttribute(),
		                                       CombatSettings->MobilityEnergyCapacity);
		AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetMobilityEnergyAttribute(),
		                                       CombatSettings->MobilityEnergyCapacity);
	}
	++CombatGeneration;
	CombatState.MontageInstanceID = INDEX_NONE;
	AbilitySystem->CancelAllAbilities();
	GetWorldTimerManager().ClearTimer(NARecoveryTimer);
	GetWorldTimerManager().ClearTimer(ActionRecoveryTimer);
	ReactionPlaybackInstanceID = INDEX_NONE;
	GuardRecoveryStartsAt = 0;
	AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetGuardLoadAttribute(), 0.f);
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
	const bool bInReaction = CombatState.Reaction != EVampsCombatReaction::None;
	const bool bDefenseAllowed =
	    CombatState.bIsAlive && AttackHandle.IsValid() && NewPhase != EVampsActionPhase::NARecovery &&
	    CombatState.Reaction != EVampsCombatReaction::Stun && (NewPhase != EVampsActionPhase::Recovery || bInReaction);
	CombatState.bOffenseRestricted = bInReaction;
	CombatState.bCanMove = CombatState.bIsAlive && !bInReaction;
	const UVampsCombatMovementComponent* Mobility = Cast<UVampsCombatMovementComponent>(GetCharacterMovement());
	CombatState.bCanDash = CombatState.bIsAlive && AttackHandle.IsValid() && CombatSettings &&
	                       Attributes->GetMobilityEnergy() >= CombatSettings->DashEnergyCost && Mobility &&
	                       Mobility->CombatMotion != EVampsCombatMotion::Dash &&
	                       (CombatState.MontageInstanceID == INDEX_NONE || CombatState.bComboWindowOpen ||
	                        NewPhase == EVampsActionPhase::Recovery) &&
	                       GetCharacterMovement()->IsMovingOnGround() &&
	                       (!bInReaction || (CombatState.Reaction == EVampsCombatReaction::Guard &&
	                                         CombatState.bGuardDashReady && CombatState.DefendedAttacker.IsValid() &&
	                                         CombatState.DefendedAttacker->CombatGeneration == DefendedGeneration));
	CombatState.bCanRequestCombat =
	    CombatState.bIsAlive && AttackHandle.IsValid() && NewPhase != EVampsActionPhase::NARecovery &&
	    CombatState.Reaction != EVampsCombatReaction::Stun &&
	    (NewPhase != EVampsActionPhase::Recovery || CombatState.bComboWindowOpen || bInReaction);
	CombatState.bCanStartAction = CombatState.bCanRequestCombat && NewPhase == EVampsActionPhase::Idle && !bInReaction;
	CombatState.bCanDealDamage = CombatState.bIsAlive && NewPhase == EVampsActionPhase::Contacts && !bInReaction;
	CombatState.bCanDefend =
	    bDefenseAllowed &&
	    ((CombatState.bDefenseRequestArmed && GetWorld()->GetTimeSeconds() < CombatState.DefenseRequestEndsAt) ||
	     (CombatState.Reaction == EVampsCombatReaction::Guard && CombatState.DefendedAttacker.IsValid()));

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
		ClearMobilityIntent();
		CancelCombatOffense();
		GetWorldTimerManager().ClearTimer(NARecoveryTimer);
		CombatState.NARecoveryEndsAt = 0;
		CombatState.Reaction = EVampsCombatReaction::None;
		CombatState.ReactionEndsAt = 0;
		CombatState.DefendedAttacker.Reset();
		CombatState.bDefenseRequestArmed = false;
		GetWorld()->GetSubsystem<UVampsCombatResolver>()->DiscardContactsFor(this);
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
		SetCombatPhase(EVampsActionPhase::Idle);
	}
}

bool AVampsCharacter::ReceiveCombatHit(const FVampsPendingContact& Contact)
{
	AVampsCharacter* Attacker = Contact.Attacker.Get();
	if (!HasAuthority() || !CombatState.bIsAlive || CombatGeneration != Contact.RecipientGeneration)
	{
		return false;
	}
	if (!IsValid(Attacker) || Attacker == this || Attacker->CombatGeneration != Contact.AttackerGeneration ||
	    !FMath::IsFinite(Contact.ContactDamage) || Contact.ContactDamage < 0.f)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': rejected captured contact from '%s', damage=%f."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(Attacker),
		       Contact.ContactDamage);
		return false;
	}
	const float DamageAmount =
	    Contact.bBlocked ? FMath::Max(0.f, Contact.ContactDamage - Contact.BlockReduction) : Contact.ContactDamage;
	FGameplayEffectContextHandle EffectContext = Attacker->AbilitySystem->MakeEffectContext();
	EffectContext.AddSourceObject(Attacker);
	FGameplayEffectSpecHandle DamageSpec =
	    Attacker->AbilitySystem->MakeOutgoingSpec(UGE_VampsDamage::StaticClass(), 1, EffectContext);
	if (!DamageSpec.IsValid())
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': could not create damage effect for captured contact from '%s'."),
		       __FUNCTION__,
		       *GetPathName(),
		       *Attacker->GetPathName());
		return false;
	}
	CombatState.LastIncomingContact =
	    Contact.bGuardBreak ? EVampsContactResult::GuardBreak
	                        : (Contact.bBlocked ? EVampsContactResult::Blocked : EVampsContactResult::OpenHit);
	CombatState.LastIncomingContactFrame = GFrameCounter;
	CombatState.bLastIncomingCritical = Contact.bCritical;
	CombatState.LastDamageReceived = DamageAmount;
	if (Contact.bBlocked)
	{
		AbilitySystem->SetNumericAttributeBase(
		    UVampsAttributeSet::GetGuardLoadAttribute(),
		    FMath::Min(Attributes->GetMaxGuardLoad(), Attributes->GetGuardLoad() + Contact.GuardLoad));
		GuardRecoveryStartsAt = Contact.ContactTime + CombatSettings->GuardRecoveryDelay;
	}
	DamageSpec.Data->SetSetByCallerMagnitude(VampsTags::Damage, -DamageAmount);
	Attacker->AbilitySystem->ApplyGameplayEffectSpecToTarget(*DamageSpec.Data, AbilitySystem);
	UE_LOG(LogVampsCombat,
	       Display,
	       TEXT("%hs: '%s' from '%s', result=%s, critical=%d, damage=%.2f, guard=%.2f/%.2f."),
	       __FUNCTION__,
	       *GetPathName(),
	       *Attacker->GetPathName(),
	       *StaticEnum<EVampsContactResult>()->GetNameStringByValue(int64(CombatState.LastIncomingContact)),
	       Contact.bCritical,
	       DamageAmount,
	       Attributes->GetGuardLoad(),
	       Attributes->GetMaxGuardLoad());
	return true;
}

void AVampsCharacter::ApplyCombatReaction(const FVampsPendingContact& Contact, bool bFinalGuardImpact)
{
	if (!CombatState.bIsAlive)
	{
		return;
	}
	CombatState.ReactionSource = Contact.Attacker;
	ClearMobilityIntent();
	if (CombatState.Reaction == EVampsCombatReaction::Stun)
	{
		return;
	}
	CancelCombatOffense();
	UAnimInstance* Animation = GetMesh()->GetAnimInstance();
	FAnimMontageInstance* PreviousReaction =
	    Animation ? Animation->GetMontageInstanceForID(ReactionPlaybackInstanceID) : nullptr;
	if (PreviousReaction)
	{
		Animation->Montage_Stop(0.05f, PreviousReaction->Montage);
	}
	ReactionPlaybackInstanceID = INDEX_NONE;
	ConsumeMovementInputVector();
	GetCharacterMovement()->StopMovementImmediately();
	CombatState.Reaction = Contact.bGuardBreak
	                           ? EVampsCombatReaction::Stun
	                           : (Contact.bBlocked ? EVampsCombatReaction::Guard : EVampsCombatReaction::Hit);
	const float Duration =
	    Contact.bGuardBreak ? CombatSettings->StunDuration : Contact.StepSettings.ContactReactionDuration;
	CombatState.ReactionEndsAt = Contact.ContactTime + Duration;
	CombatState.bGuardDashReady = CombatState.Reaction == EVampsCombatReaction::Guard && bFinalGuardImpact;
	if (CombatState.Reaction != EVampsCombatReaction::Guard)
	{
		CombatState.DefendedAttacker.Reset();
		CombatState.DefendedActionNumber = INDEX_NONE;
		CombatState.bDefenseRequestArmed = false;
	}
	SetCombatPhase(CombatState.Phase);
	UAnimMontage* ReactionMontage =
	    CombatState.Reaction == EVampsCombatReaction::Guard
	        ? CombatSettings->GuardReactionMontage.Get()
	        : (CombatState.Reaction == EVampsCombatReaction::Hit ? CombatSettings->HitReactionMontage.Get() : nullptr);
	if (ReactionMontage)
	{
		if (!Animation || ReactionMontage == CombatSettings->AttackMontage || !ReactionMontage->bEnableAutoBlendOut ||
		    ReactionMontage->GetPlayLength() <= 0.f || !FMath::IsFinite(ReactionMontage->RateScale) ||
		    ReactionMontage->RateScale <= 0.f || ReactionMontage->TimeStretchCurve.IsValid())
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: '%s': invalid reaction montage '%s'; requires AnimInstance, positive length, "
			            "constant positive RateScale, Auto Blend Out and a different montage from AttackMontage."),
			       __FUNCTION__,
			       *GetPathName(),
			       *GetPathNameSafe(ReactionMontage));
			return;
		}
		const float PlaybackDuration = Animation->Montage_Play(
		    ReactionMontage, ReactionMontage->GetPlayLength() / (Duration * ReactionMontage->RateScale));
		const FAnimMontageInstance* Playback = Animation->GetActiveInstanceForMontage(ReactionMontage);
		if (PlaybackDuration <= 0.f || !Playback)
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: '%s': reaction montage '%s' failed to start; verify skeleton and slot."),
			       __FUNCTION__,
			       *GetPathName(),
			       *ReactionMontage->GetPathName());
			return;
		}
		ReactionPlaybackInstanceID = Playback->GetInstanceID();
	}
}
