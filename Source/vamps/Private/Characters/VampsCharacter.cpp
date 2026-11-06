#include "Characters/VampsCharacter.h"
#include "Characters/VampsLockOnComponent.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Combat/Abilities/GA_Attack.h"
#include "Combat/Abilities/GA_Dash.h"
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
#include "Components/WidgetComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Player/VampsPlayerCameraManager.h"
#include "GameModes/VampsMatchGameMode.h"
#include "UI/VampsCharacterIndicatorWidget.h"

namespace
{
	void RequestCombatFraming(AVampsCharacter* ViewingFighter, AVampsCharacter* ChainSource)
	{
		APlayerController* LocalController = Cast<APlayerController>(ViewingFighter->GetController());
		if (!LocalController || !LocalController->IsLocalPlayerController())
		{
			return;
		}
		AVampsPlayerCameraManager* CombatCamera = Cast<AVampsPlayerCameraManager>(LocalController->PlayerCameraManager);
		if (!CombatCamera)
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: '%s': combat framing requires VampsPlayerCameraManager; got '%s'."),
			       __FUNCTION__,
			       *ViewingFighter->GetPathName(),
			       *GetPathNameSafe(LocalController->PlayerCameraManager));
			return;
		}
		CombatCamera->BeginCombatFraming(ChainSource);
	}
} // namespace

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
	CharacterIndicator = CreateDefaultSubobject<UWidgetComponent>(TEXT("CharacterIndicator"));
	CharacterIndicator->SetupAttachment(GetRootComponent());
	CharacterIndicator->SetRelativeLocation(FVector(0.f, 0.f, 128.f));
	CharacterIndicator->SetWidgetSpace(EWidgetSpace::Screen);
	CharacterIndicator->SetDrawSize(FVector2D(240.f, 80.f));
	CharacterIndicator->SetPivot(FVector2D(0.5f, 1.f));
	CharacterIndicator->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AVampsCharacter::BeginPlay()
{
	AbilitySystem->InitAbilityActorInfo(this, this);
	if (!FMath::IsFinite(StartingMaxHealth) || StartingMaxHealth <= 0.f)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': StartingMaxHealth must be finite and positive; received %f."),
		       __FUNCTION__,
		       *GetPathName(),
		       StartingMaxHealth);
		Super::BeginPlay();
		return;
	}
	AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetMaxHealthAttribute(), StartingMaxHealth);
	AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetHealthAttribute(), StartingMaxHealth);
	AbilitySystem->GetGameplayAttributeValueChangeDelegate(UVampsAttributeSet::GetHealthAttribute())
	    .AddUObject(this, &AVampsCharacter::HandleCombatHealthChanged);
	if (CombatSettings && CombatSettings->HasValidReactionSettings() && CombatSettings->HasValidMobilitySettings() &&
	    InitializeCombatWeapons())
	{
		if (HasAuthority())
		{
			AttackHandle = AbilitySystem->GiveAbility(FGameplayAbilitySpec(UGA_Attack::StaticClass(), 1));
			DashHandle = AbilitySystem->GiveAbility(FGameplayAbilitySpec(UGA_Dash::StaticClass(), 1));
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
	if (GetNetMode() != NM_DedicatedServer)
	{
		CharacterIndicator->InitWidget();
		UVampsCharacterIndicatorWidget* IndicatorWidget =
		    Cast<UVampsCharacterIndicatorWidget>(CharacterIndicator->GetUserWidgetObject());
		if (!IndicatorWidget)
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: '%s': CharacterIndicator requires a VampsCharacterIndicatorWidget; class='%s'."),
			       __FUNCTION__,
			       *GetPathName(),
			       *GetPathNameSafe(CharacterIndicator->GetWidgetClass()));
			return;
		}
		IndicatorWidget->ObserveFighter(this);
	}
}

void AVampsCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!HasAuthority() || !CombatState.bIsAlive || !AttackHandle.IsValid())
	{
		return;
	}
	if (const AVampsMatchGameMode* MatchMode = Cast<AVampsMatchGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (!MatchMode->AllowsCombat(this))
		{
			SetCombatPhase(CombatState.Phase);
			return;
		}
	}
	if (CombatState.MontageInstanceID != INDEX_NONE)
	{
		UAnimInstance* Animation = GetMesh()->GetAnimInstance();
		const FAnimMontageInstance* Playback =
		    Animation ? Animation->GetMontageInstanceForID(CombatState.MontageInstanceID) : nullptr;
		if ((!Playback || (Playback->IsActive() && !Playback->IsPlaying())) &&
		    !GetWorld()->GetSubsystem<UVampsCombatResolver>()->HasQueuedCombatRecoveryEnd(*this))
		{
			AbilitySystem->CancelAbilityHandle(AttackHandle);
		}
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

void AVampsCharacter::SetMobilityInput(bool bHasInput)
{
	bHasMobilityInput = bHasInput;
}

void AVampsCharacter::ClearMobilityIntent()
{
	bMobilityHeld = false;
	CombatState.bIsSprinting = false;
	CombatState.bDashRequested = false;
	DashRequestActionNumber = INDEX_NONE;
	RequestedDashDirection = FVector::ZeroVector;
	AbilitySystem->CancelAbilityHandle(DashHandle);
}

void AVampsCharacter::SetMobilityHeld(bool bHeld, const FVector& DashDirection)
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
	if (!CombatState.bCanDash)
	{
		return;
	}
	if (DashDirection.ContainsNaN())
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': dash requires a finite input direction; received %s."),
		       __FUNCTION__,
		       *GetPathName(),
		       *DashDirection.ToString());
		return;
	}
	RequestedDashDirection = DashDirection.GetSafeNormal2D();
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
	CombatState.bDashRequested = false;
	SetCombatPhase(CombatState.Phase);
	if (!CombatState.bCanDash || DashRequestActionNumber != CombatState.ActionNumber ||
	    !AbilitySystem->TryActivateAbility(DashHandle) || !CombatState.bIsDashing)
	{
		bMobilityHeld = false;
	}
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
	const bool bSprinting = bMobilityHeld && !CombatState.bDashRequested && bHasMobilityInput &&
	                        !CombatState.bIsDashing && CombatState.bCanMove && Movement->IsMovingOnGround() &&
	                        Movement->CombatMotion == EVampsCombatMotion::None &&
	                        CombatState.MontageInstanceID == INDEX_NONE && Attributes->GetMobilityEnergy() > 0.f;
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
	else if (Now >= MobilityRecoveryStartsAt && !CombatState.bIsDashing)
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
		AbilitySystem->CancelAbilityHandle(DashHandle);
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

bool AVampsCharacter::CanEngageEnemy(const AVampsCharacter* OtherFighter) const
{
	if (!IsValid(OtherFighter) || OtherFighter == this || !OtherFighter->HasActorBegunPlay() ||
	    OtherFighter->IsActorBeingDestroyed() || !OtherFighter->CombatState.bIsAlive)
	{
		return false;
	}
	const AVampsMatchGameMode* MatchMode = Cast<AVampsMatchGameMode>(GetWorld()->GetAuthGameMode());
	return !MatchMode || MatchMode->AreEnemies(this, OtherFighter);
}

bool AVampsCharacter::CanHitCombatTarget(const AVampsCharacter* OtherFighter) const
{
	if (!CanEngageEnemy(OtherFighter))
	{
		return false;
	}
	const AVampsMatchGameMode* MatchMode = Cast<AVampsMatchGameMode>(GetWorld()->GetAuthGameMode());
	return !MatchMode || (MatchMode->AllowsCombat(this) && !MatchMode->IsSpawnProtected(OtherFighter));
}

void AVampsCharacter::StopMatchCombat()
{
	ClearMobilityIntent();
	CancelCombatOffense();
	GetWorldTimerManager().ClearTimer(NARecoveryTimer);
	CombatState.NARecoveryEndsAt = 0;
	CombatState.Reaction = EVampsCombatReaction::None;
	CombatState.ReactionEndsAt = 0;
	CombatState.bDefenseRequestArmed = false;
	CombatState.DefendedAttacker.Reset();
	SetCombatPhase(EVampsActionPhase::Idle);
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetMesh()->bPauseAnims = true;
}

void AVampsCharacter::PresentCombatDeath(bool bBlockedDeath)
{
	UAnimInstance* Animation = GetMesh()->GetAnimInstance();
	if (!CombatSettings || !CombatSettings->HasValidDeathMontages() || !Animation)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': death presentation requires valid settings, death montages and AnimInstance."),
		       __FUNCTION__,
		       *GetPathName());
		return;
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();
	UAnimMontage* DeathAnimation =
	    bBlockedDeath ? CombatSettings->GuardDeathMontage.Get() : CombatSettings->DeathMontage.Get();
	if (Animation->Montage_Play(DeathAnimation) <= 0.f)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': death montage '%s' did not start; verify skeleton and slot."),
		       __FUNCTION__,
		       *GetPathName(),
		       *DeathAnimation->GetPathName());
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
	if (AVampsMatchGameMode* MatchMode = Cast<AVampsMatchGameMode>(GetWorld()->GetAuthGameMode()))
	{
		MatchMode->EndSpawnProtection(this);
	}
	CombatState.ComboStep = 1;
	if (!BeginCombatStep())
	{
		CombatState.MontageInstanceID = INDEX_NONE;
		CombatState.ComboStep = 0;
		return false;
	}
	RequestCombatFraming(this, this);
	return true;
}

bool AVampsCharacter::BeginCombatStep()
{
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
	if (CombatState.ComboStep == 1)
	{
		Movement->BeginOffensiveAdvance();
	}
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
	if (CombatState.Phase != EVampsActionPhase::NARecovery)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': attack montage '%s', step %d ended without AN_EndRecovery. Attack aborted."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(CombatSettings ? CombatSettings->AttackMontage.Get() : nullptr),
		       CombatState.ComboStep);
		CombatState.ComboStep = 0;
		SetCombatPhase(EVampsActionPhase::Idle);
		return;
	}
	SetCombatPhase(EVampsActionPhase::NARecovery);
}

void AVampsCharacter::EndCombatRecovery(const FBranchingPointNotifyPayload& Payload)
{
	if (!IsCurrentCombatNotify(Payload))
	{
		return;
	}
	if (CombatState.Phase != EVampsActionPhase::Recovery || CombatState.bComboWindowOpen)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': AN_EndRecovery in step %d requires Recovery and a closed combo window; phase=%d, "
		            "window=%d."),
		       __FUNCTION__,
		       *GetPathName(),
		       CombatState.ComboStep,
		       int32(CombatState.Phase),
		       CombatState.bComboWindowOpen);
		return;
	}
	UAnimInstance* Animation = GetMesh()->GetAnimInstance();
	if (!Animation)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': AN_EndRecovery requires an AnimInstance."),
		       __FUNCTION__,
		       *GetPathName());
		return;
	}
	Animation->Montage_Pause(CombatSettings->AttackMontage);
	GetWorld()->GetSubsystem<UVampsCombatResolver>()->QueueCombatRecoveryEnd(this);
}

void AVampsCharacter::ResolveCombatRecoveryEnd(int32 RecoveryActionNumber)
{
	if (!CombatState.bIsAlive || CombatState.ActionNumber != RecoveryActionNumber ||
	    CombatState.Phase != EVampsActionPhase::Recovery)
	{
		return;
	}
	BeginNARecovery();
	AbilitySystem->CancelAbilityHandle(AttackHandle);
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
	Animation->Montage_Resume(Montage);
	CombatState.LastRequest = EVampsCombatRequest::ComboAdvanced;
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
	RequestCombatFraming(this, Contact.Attacker.Get());
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
	CombatState.ComboStep = 0;
	SetCombatPhase(CombatState.Phase == EVampsActionPhase::NARecovery ? EVampsActionPhase::NARecovery
	                                                                  : EVampsActionPhase::Idle);
	AbilitySystem->CancelAbilityHandle(AttackHandle);
}

void AVampsCharacter::FinishCombatReaction()
{
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
	bHasMobilityInput = false;
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
	CombatState = FVampsCombatState();
	SetCombatPhase(EVampsActionPhase::Idle);
}

void AVampsCharacter::SetCombatPhase(EVampsActionPhase NewPhase)
{
	const bool bWasDamageEnabled = CombatState.bCanDealDamage;
	CombatState.Phase = NewPhase;
	CombatState.bIsAlive = Attributes->GetHealth() > 0.f;
	const AVampsMatchGameMode* MatchMode = Cast<AVampsMatchGameMode>(GetWorld()->GetAuthGameMode());
	const bool bMatchAllowsCombat = !MatchMode || MatchMode->AllowsCombat(this);
	CombatState.bIsRecovering = NewPhase == EVampsActionPhase::Recovery || NewPhase == EVampsActionPhase::NARecovery;
	const bool bInReaction = CombatState.Reaction != EVampsCombatReaction::None;
	const bool bDefenseAllowed = CombatState.bIsAlive && bMatchAllowsCombat && AttackHandle.IsValid() &&
	                             NewPhase != EVampsActionPhase::NARecovery &&
	                             CombatState.Reaction != EVampsCombatReaction::Stun &&
	                             (NewPhase != EVampsActionPhase::Recovery || bInReaction);
	CombatState.bOffenseRestricted = bInReaction;
	CombatState.bCanMove = CombatState.bIsAlive && bMatchAllowsCombat && !bInReaction &&
	                       (NewPhase == EVampsActionPhase::Idle || NewPhase == EVampsActionPhase::NARecovery);
	const UVampsCombatMovementComponent* Mobility = Cast<UVampsCombatMovementComponent>(GetCharacterMovement());
	CombatState.bCanDash = CombatState.bCanMove && DashHandle.IsValid() && CombatSettings &&
	                       Attributes->GetMobilityEnergy() >= CombatSettings->DashEnergyCost && Mobility &&
	                       !CombatState.bIsDashing && GetCharacterMovement()->IsMovingOnGround();
	CombatState.bCanRequestCombat =
	    CombatState.bIsAlive && bMatchAllowsCombat && AttackHandle.IsValid() &&
	    NewPhase != EVampsActionPhase::NARecovery && CombatState.Reaction != EVampsCombatReaction::Stun &&
	    (NewPhase != EVampsActionPhase::Recovery || CombatState.bComboWindowOpen || bInReaction);
	CombatState.bCanStartAction = CombatState.bCanRequestCombat && NewPhase == EVampsActionPhase::Idle && !bInReaction;
	CombatState.bCanDealDamage =
	    CombatState.bIsAlive && bMatchAllowsCombat && NewPhase == EVampsActionPhase::Contacts && !bInReaction;
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
	if (!CombatState.bIsAlive)
	{
		if (AVampsMatchGameMode* MatchMode = Cast<AVampsMatchGameMode>(GetWorld()->GetAuthGameMode()))
		{
			MatchMode->RegisterCombatDeath(this, Attacker, Contact.bBlocked);
		}
	}
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

void AVampsCharacter::ApplyCombatReaction(const FVampsPendingContact& Contact)
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
