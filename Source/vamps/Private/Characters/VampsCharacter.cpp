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
#include "GameModes/VampsMatchGameMode.h"
#include "UI/VampsCharacterIndicatorWidget.h"

AVampsCharacter::AVampsCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UVampsCombatMovementComponent>(ACharacter::CharacterMovementComponentName))
{
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
	AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetMaxHealthAttribute(), StartingMaxHealth);
	AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetHealthAttribute(), StartingMaxHealth);
	AbilitySystem->GetGameplayAttributeValueChangeDelegate(UVampsAttributeSet::GetHealthAttribute()).AddUObject(this, &AVampsCharacter::HandleCombatHealthChanged);

	if (InitializeCombatWeapons() && HasAuthority())
	{
		AttackHandle = AbilitySystem->GiveAbility(FGameplayAbilitySpec(UGA_Attack::StaticClass(), 1));
		DashHandle = AbilitySystem->GiveAbility(FGameplayAbilitySpec(UGA_Dash::StaticClass(), 1));
		AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetMaxGuardLoadAttribute(), CombatSettings->GuardCapacity);
		AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetMaxMobilityEnergyAttribute(), CombatSettings->MobilityEnergyCapacity);
		AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetMobilityEnergyAttribute(), CombatSettings->MobilityEnergyCapacity);
	}

	if (AVampsMatchGameMode* MatchMode = GetWorld()->GetAuthGameMode<AVampsMatchGameMode>())
	{
		MatchMode->OnRoundPhaseChanged.AddUObject(this, &AVampsCharacter::RefreshCombatPermissions);
	}

	RefreshCombatPermissions();
	Super::BeginPlay();

	if (GetNetMode() != NM_DedicatedServer)
	{
		CharacterIndicator->InitWidget();
		if (UVampsCharacterIndicatorWidget* IndicatorWidget =
		        Cast<UVampsCharacterIndicatorWidget>(CharacterIndicator->GetUserWidgetObject()))
		{
			IndicatorWidget->ObserveFighter(this);
		}
	}
	LockOn->InitializeCandidates();
}

void AVampsCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!HasAuthority() || !CombatState.bIsAlive || !AttackHandle.IsValid() || !IsMatchCombatAllowed())
	{
		return;
	}

	const double Now = GetWorld()->GetTimeSeconds();

	// guard load recovery
	if (CombatState.Reaction == EVampsCombatReaction::None && Now >= GuardRecoveryStartsAt && Attributes->GetGuardLoad() > 0.f)
	{
		const float RecoverySeconds = FMath::Min(DeltaSeconds, float(Now - GuardRecoveryStartsAt));

		AbilitySystem->SetNumericAttributeBase(
		    UVampsAttributeSet::GetGuardLoadAttribute(),
		    FMath::Max(0.f, Attributes->GetGuardLoad() - CombatSettings->GuardRecoveryPerSecond * RecoverySeconds));
	}

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

	if (!CanStartCombatDash())
	{
		return;
	}

	RequestedDashDirection = DashDirection.GetSafeNormal2D();
	bMobilityHeld = true;
	CombatState.bDashRequested = true;

	// eesolve hits first so an impact can cancel the dash before it spends energy
	GetWorld()->GetSubsystem<UVampsCombatResolver>()->QueueCombatDash(*this);
}

void AVampsCharacter::ResolveCombatDash()
{
	CombatState.bDashRequested = false;

	if (!CanStartCombatDash() || !AbilitySystem->TryActivateAbility(DashHandle) || !CombatState.bIsDashing)
	{
		bMobilityHeld = false;
	}
}

bool AVampsCharacter::CanStartCombatDash() const
{
	return CombatState.bCanMove && DashHandle.IsValid() && !CombatState.bIsDashing &&
	       Attributes->GetMobilityEnergy() >= CombatSettings->DashEnergyCost &&
	       GetCharacterMovement()->IsMovingOnGround() && IsMatchCombatAllowed();
}

void AVampsCharacter::UpdateMobilityEnergy(float DeltaSeconds)
{
	const auto* Movement = static_cast<UVampsCombatMovementComponent*>(GetCharacterMovement());
	const float CurrentEnergy = Attributes->GetMobilityEnergy();
	const float MaxEnergy = Attributes->GetMaxMobilityEnergy();

	const bool bSprinting = bMobilityHeld && !CombatState.bDashRequested && bHasMobilityInput &&
	                        !CombatState.bIsDashing && CombatState.bCanMove && Movement->IsMovingOnGround() &&
	                        Movement->CombatMotion == EVampsCombatMotion::None && CurrentEnergy > 0.f;
	CombatState.bIsSprinting = bSprinting;

	const double Now = GetWorld()->GetTimeSeconds();

	// sprint drains energy each frame and postpones recovery until sprinting stops
	if (bSprinting)
	{
		const float Energy = FMath::Max(0.f, CurrentEnergy - CombatSettings->SprintEnergyPerSecond * DeltaSeconds);
		AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetMobilityEnergyAttribute(), Energy);
		MobilityRecoveryStartsAt = Now + CombatSettings->MobilityRecoveryDelay;

		if (Energy <= 0.f)
		{
			bMobilityHeld = false;
			CombatState.bIsSprinting = false;
		}
	}
	// refill only for the part of this frame after the recovery delay
	else if (!CombatState.bIsDashing && CurrentEnergy < MaxEnergy && Now >= MobilityRecoveryStartsAt)
	{
		const float RecoverySeconds = FMath::Min(DeltaSeconds, float(Now - MobilityRecoveryStartsAt));
		AbilitySystem->SetNumericAttributeBase(
		    UVampsAttributeSet::GetMobilityEnergyAttribute(),
		    FMath::Min(MaxEnergy, CurrentEnergy + CombatSettings->MobilityRecoveryPerSecond * RecoverySeconds));
	}
}

bool AVampsCharacter::InitializeCombatWeapons()
{
	if (!CombatSettings || CombatSettings->Weapons.IsEmpty())
	{
		return false;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = this;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (const FVampsWeaponSettings& WeaponSettings : CombatSettings->Weapons)
	{
		if (!GetMesh()->DoesSocketExist(WeaponSettings.AttachSocket))
		{
			return false;
		}

		AVampsWeapon* EquippedWeapon = GetWorld()->SpawnActor<AVampsWeapon>(WeaponSettings.WeaponClass, GetActorTransform(), SpawnParameters);

		if (!IsValid(EquippedWeapon))
		{
			return false;
		}

		Weapons.Add(EquippedWeapon);

		EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSettings.AttachSocket);
		EquippedWeapon->SetActorRelativeTransform(WeaponSettings.RelativeTransform);
	}
	return true;
}

void AVampsCharacter::EndPlay(const EEndPlayReason::Type Reason)
{
	if (AVampsMatchGameMode* MatchMode = GetWorld()->GetAuthGameMode<AVampsMatchGameMode>())
	{
		MatchMode->OnRoundPhaseChanged.RemoveAll(this);
	}

	AbilitySystem->GetGameplayAttributeValueChangeDelegate(UVampsAttributeSet::GetHealthAttribute()).RemoveAll(this);

	ClearCombatTimers();

	++CombatGeneration;
	CombatState.MontageInstanceID = INDEX_NONE;
	AbilitySystem->CancelAllAbilities();

	if (UVampsCombatResolver* Resolver = GetWorld()->GetSubsystem<UVampsCombatResolver>())
	{
		Resolver->DiscardPendingCombatFor(*this);
	}

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
	AbilitySystem->RefreshAbilityActorInfo();
}

void AVampsCharacter::UnPossessed()
{
	ResetCombatState();
	Super::UnPossessed();
	AbilitySystem->RefreshAbilityActorInfo();
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

	++CombatState.RequestCount;

	// Record a rejection for F9 unless a branch below accepts the request.
	CombatState.LastRequest = EVampsCombatRequest::Rejected;

	if (!CombatState.bCanRequestCombat || !IsMatchCombatAllowed())
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

	// an input that cannot start a new attack can rearm defense outside recovery state
	if (CombatState.Phase != EVampsActionPhase::Recovery)
	{
		ArmDefenseRequest();
		RefreshCombatPermissions();
		CombatState.LastRequest = EVampsCombatRequest::DefenseRearmed;
	}

	// buffer the next combo step until the window closes and the resolver checks the hit
	if (CanRequestCombatContinuation())
	{
		CombatState.bContinuationRequested = true;
		CombatState.LastRequest = EVampsCombatRequest::ContinuationRequested;
	}
}

bool AVampsCharacter::CanRequestCombatContinuation() const
{
	return CombatState.bCanRequestCombat && CombatState.bComboWindowOpen && !CombatState.bOffenseRestricted &&
	       CombatState.ComboStep < CombatSettings->AttackMontage->GetNumSections();
}

bool AVampsCharacter::CanEngageEnemy(const AVampsCharacter* OtherFighter) const
{
	if (!IsValid(OtherFighter) || OtherFighter == this || !OtherFighter->HasActorBegunPlay() ||
	    OtherFighter->IsActorBeingDestroyed() || !OtherFighter->CombatState.bIsAlive)
	{
		return false;
	}

	const AVampsMatchGameMode* MatchMode = Cast<AVampsMatchGameMode>(GetWorld()->GetAuthGameMode());
	return !MatchMode ||
	       (MatchMode->RoundPhase == EVampsMatchPhase::Active && MatchMode->AreEnemies(this, OtherFighter));
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
	ClearCombatTimers();
	CombatState.Reaction = EVampsCombatReaction::None;
	CombatState.DefendedAttacker.Reset();
	CombatState.Phase = EVampsActionPhase::Idle;
	ClearMobilityIntent();
	CancelCombatOffense();
	GetCharacterMovement()->DisableMovement();
	GetMesh()->bPauseAnims = CombatState.bIsAlive;
}

void AVampsCharacter::PresentCombatDeath(bool bBlockedDeath)
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();

	if (UAnimInstance* Animation = GetMesh()->GetAnimInstance())
	{
		Animation->Montage_Play(bBlockedDeath ? CombatSettings->GuardDeathMontage.Get() : CombatSettings->DeathMontage.Get());
	}
}

bool AVampsCharacter::BeginCombatAction(int32 PlaybackInstanceID)
{
	if (!CombatState.bCanStartAction)
	{
		return false;
	}

	CombatState.MontageInstanceID = PlaybackInstanceID;

	if (AVampsMatchGameMode* MatchMode = Cast<AVampsMatchGameMode>(GetWorld()->GetAuthGameMode()))
	{
		MatchMode->EndSpawnProtection(this);
	}

	CombatState.ComboStep = 1;

	BeginCombatStep();
	OnCombatFramingRequested.Broadcast(this);
	return true;
}

void AVampsCharacter::BeginCombatStep()
{
	++CombatState.ActionNumber;

	CombatState.bDashRequested = false;
	CombatState.bIsSprinting = false;

	ArmDefenseRequest();

	CombatState.bActionConnected = false;
	CombatState.bComboWindowOpen = false;
	CombatState.bContinuationRequested = false;
	CombatState.LastOutgoingContact = EVampsContactResult::None;

	for (AVampsWeapon* EquippedWeapon : Weapons)
	{
		EquippedWeapon->ResetSectionHits();
	}

	SetCombatPhase(EVampsActionPhase::Preparation);

	if (CombatState.ComboStep == 1)
	{
		static_cast<UVampsCombatMovementComponent*>(GetCharacterMovement())->BeginOffensiveAdvance();
	}
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
	}

	static_cast<UVampsCombatMovementComponent*>(GetCharacterMovement())->StopVoluntaryMotion();

	CombatState.MontageInstanceID = INDEX_NONE;
	CombatState.bComboWindowOpen = false;
	CombatState.bContinuationRequested = false;

	if (CombatState.Phase != EVampsActionPhase::NARecovery)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': attack montage '%s', step %d ended without AN_EndRecovery. Attack aborted."),
		       __FUNCTION__,
		       *GetPathName(),
		       *CombatSettings->AttackMontage->GetPathName(),
		       CombatState.ComboStep
		);

		CombatState.ComboStep = 0;
		SetCombatPhase(EVampsActionPhase::Idle);
	}
}

void AVampsCharacter::EndCombatRecovery(const FBranchingPointNotifyPayload& Payload)
{
	if (!IsCurrentCombatNotify(Payload) || CombatState.Phase != EVampsActionPhase::Recovery ||
	    CombatState.bComboWindowOpen)
	{
		return;
	}

	// keep the montage here until the resolver decides whether the combo continues
	GetMesh()->GetAnimInstance()->Montage_Pause(CombatSettings->AttackMontage);
	GetWorld()->GetSubsystem<UVampsCombatResolver>()->QueueCombatRecoveryEnd(*this);
}

void AVampsCharacter::ResolveCombatRecoveryEnd(int32 RecoveryActionNumber)
{
	if (!CombatState.bIsAlive || CombatState.ActionNumber != RecoveryActionNumber || CombatState.Phase != EVampsActionPhase::Recovery)
	{
		return;
	}

	BeginNARecovery();
	AbilitySystem->CancelAbilityHandle(AttackHandle);
}

void AVampsCharacter::BeginNARecovery()
{
	GetWorldTimerManager().SetTimer(NARecoveryTimer, this, &AVampsCharacter::FinishNARecovery, CombatSettings->NARecoveryDuration);
	SetCombatPhase(EVampsActionPhase::NARecovery);
}

bool AVampsCharacter::IsCurrentCombatNotify(const FBranchingPointNotifyPayload& Payload) const
{
	return HasAuthority() && CombatState.MontageInstanceID != INDEX_NONE &&
	       Payload.MontageInstanceID == CombatState.MontageInstanceID &&
	       CombatSettings->AttackMontage->GetSectionIndexFromPosition(Payload.NotifyEvent->GetTriggerTime()) ==
	           CombatState.ComboStep - 1;
}

void AVampsCharacter::SetCombatContacts(const FBranchingPointNotifyPayload& Payload, bool bEnabled)
{
	if (!IsCurrentCombatNotify(Payload))
	{
		return;
	}

	const EVampsActionPhase ExpectedPhase = bEnabled ? EVampsActionPhase::Preparation : EVampsActionPhase::Contacts;

	if (CombatState.Phase != ExpectedPhase)
	{
		AbilitySystem->CancelAbilityHandle(AttackHandle);
		return;
	}

	SetCombatPhase(bEnabled ? EVampsActionPhase::Contacts : EVampsActionPhase::Recovery);
}

void AVampsCharacter::ResetCombatWeaponHits(const FBranchingPointNotifyPayload& Payload, const TArray<int32>& WeaponIndices)
{
	if (!IsCurrentCombatNotify(Payload) || CombatState.Phase != EVampsActionPhase::Contacts)
	{
		return;
	}

	for (int32 WeaponIndex : WeaponIndices)
	{
		AVampsWeapon* EquippedWeapon = Weapons[WeaponIndex];
		if (IsValid(EquippedWeapon))
		{
			EquippedWeapon->ResetImpactHits();
		}
	}
}

void AVampsCharacter::SetCombatComboWindow(const FBranchingPointNotifyPayload& Payload, bool bEnabled)
{
	if (!IsCurrentCombatNotify(Payload))
	{
		return;
	}

	CombatState.bComboWindowOpen = bEnabled;
	RefreshCombatPermissions();

	if (!bEnabled && CombatState.bContinuationRequested)
	{
		GetWorld()->GetSubsystem<UVampsCombatResolver>()->QueueCombatContinuation(*this);
	}
}

void AVampsCharacter::ResolveCombatContinuation()
{
	if (!CombatState.bContinuationRequested)
	{
		return;
	}

	CombatState.bContinuationRequested = false;

	if (!CombatState.bActionConnected)
	{
		CombatState.LastRequest = EVampsCombatRequest::Rejected;
		return;
	}

	UAnimInstance* Animation = GetMesh()->GetAnimInstance();

	const FAnimMontageInstance* Playback = Animation ? Animation->GetMontageInstanceForID(CombatState.MontageInstanceID) : nullptr;

	if (!Playback || !Playback->IsValid())
	{
		AbilitySystem->CancelAbilityHandle(AttackHandle);
		return;
	}

	if (Playback->IsStopped())
	{
		CombatState.LastRequest = EVampsCombatRequest::Rejected;
		return;
	}

	UAnimMontage* Montage = Playback->Montage;
	const FName NextSection = Montage->GetSectionName(CombatState.ComboStep);
	++CombatState.ComboStep;

	BeginCombatStep();

	Animation->Montage_JumpToSection(NextSection, Montage);
	Animation->Montage_Resume(Montage);

	CombatState.LastRequest = EVampsCombatRequest::ComboAdvanced;
}

void AVampsCharacter::ArmDefenseRequest()
{
	GetWorldTimerManager().SetTimer(DefenseRequestTimer, this, &AVampsCharacter::ExpireDefenseRequest, CombatSettings->DefenseRequestDuration);
	CombatState.bDefenseRequestArmed = DefenseRequestTimer.IsValid();
}

void AVampsCharacter::ClearDefenseRequest()
{
	GetWorldTimerManager().ClearTimer(DefenseRequestTimer);
	CombatState.bDefenseRequestArmed = false;
}

void AVampsCharacter::ExpireDefenseRequest()
{
	ClearDefenseRequest();
	RefreshCombatPermissions();
}

void AVampsCharacter::ClearCombatTimers()
{
	ClearDefenseRequest();
	GetWorldTimerManager().ClearTimer(NARecoveryTimer);
	GetWorldTimerManager().ClearTimer(CombatReactionTimer);
}

float AVampsCharacter::GetCombatReactionTimeRemaining() const
{
	return FMath::Max(0.f, GetWorldTimerManager().GetTimerRemaining(CombatReactionTimer));
}

float AVampsCharacter::GetNARecoveryTimeRemaining() const
{
	return FMath::Max(0.f, GetWorldTimerManager().GetTimerRemaining(NARecoveryTimer));
}

bool AVampsCharacter::MatchesDefendedContact(const FVampsPendingContact& Contact) const
{
	return CombatState.DefendedAttacker == Contact.Attacker &&
	       CombatState.DefendedActionNumber == Contact.ActionNumber && DefendedGeneration == Contact.AttackerGeneration;
}

void AVampsCharacter::RememberDefendedContact(const FVampsPendingContact& Contact)
{
	ClearDefenseRequest();

	CombatState.DefendedAttacker = Contact.Attacker;
	CombatState.DefendedActionNumber = Contact.ActionNumber;

	DefendedGeneration = Contact.AttackerGeneration;

	RefreshCombatPermissions();

	LockOn->SelectTarget(Contact.Attacker.Get());

	OnCombatFramingRequested.Broadcast(Contact.Attacker.Get());
}

void AVampsCharacter::CancelCombatOffense()
{
	static_cast<UVampsCombatMovementComponent*>(GetCharacterMovement())->StopVoluntaryMotion();

	CombatState.MontageInstanceID = INDEX_NONE;
	CombatState.bComboWindowOpen = false;
	CombatState.bContinuationRequested = false;
	CombatState.ComboStep = 0;

	SetCombatPhase(CombatState.Phase == EVampsActionPhase::NARecovery ? EVampsActionPhase::NARecovery : EVampsActionPhase::Idle);

	AbilitySystem->CancelAbilityHandle(AttackHandle);
}

void AVampsCharacter::FinishCombatReaction()
{
	if (CombatState.Reaction == EVampsCombatReaction::Stun)
	{
		AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetGuardLoadAttribute(), 0.f);
		GuardRecoveryStartsAt = GetWorld()->GetTimeSeconds();
	}

	StopCombatReactionMontage();

	CombatState.Reaction = EVampsCombatReaction::None;
	CombatState.ReactionSource.Reset();
	CombatState.DefendedAttacker.Reset();
	CombatState.DefendedActionNumber = INDEX_NONE;

	RefreshCombatPermissions();
}

void AVampsCharacter::StopCombatReactionMontage()
{
	UAnimInstance* Animation = GetMesh()->GetAnimInstance();
	const FAnimMontageInstance* Playback = Animation ? Animation->GetMontageInstanceForID(ReactionPlaybackInstanceID) : nullptr;

	if (Playback)
	{
		Animation->Montage_Stop(0.05f, Playback->Montage);
	}

	ReactionPlaybackInstanceID = INDEX_NONE;
}

void AVampsCharacter::FinishNARecovery()
{
	CombatState.ComboStep = 0;
	SetCombatPhase(EVampsActionPhase::Idle);
}

void AVampsCharacter::RegisterCombatContact(bool bBlocked)
{
	const EVampsContactResult ContactResult = bBlocked ? EVampsContactResult::Blocked : EVampsContactResult::OpenHit;

	CombatState.LastOutgoingContact = CombatState.bActionConnected && CombatState.LastOutgoingContact != ContactResult
	                                ? EVampsContactResult::Mixed
																	: ContactResult;

	CombatState.bActionConnected = true;
}

void AVampsCharacter::ResetCombatState()
{
	ClearMobilityIntent();
	ClearCombatTimers();

	bHasMobilityInput = false;
	MobilityRecoveryStartsAt = 0;

	if (CombatSettings)
	{
		AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetMaxMobilityEnergyAttribute(), CombatSettings->MobilityEnergyCapacity);
		AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetMobilityEnergyAttribute(), CombatSettings->MobilityEnergyCapacity);
	}

	++CombatGeneration;
	CombatState.MontageInstanceID = INDEX_NONE;

	AbilitySystem->CancelAllAbilities();

	ReactionPlaybackInstanceID = INDEX_NONE;

	GuardRecoveryStartsAt = 0;

	AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetGuardLoadAttribute(), 0.f);

	GetWorld()->GetSubsystem<UVampsCombatResolver>()->DiscardPendingCombatFor(*this);

	for (AVampsWeapon* EquippedWeapon : Weapons)
	{
		if (IsValid(EquippedWeapon))
		{
			EquippedWeapon->ResetSectionHits();
		}
	}

	static_cast<UVampsCombatMovementComponent*>(GetCharacterMovement())->ResetCombatMotion();

	// restore the default combat state before rebuilding permissions from health and round state
	CombatState = FVampsCombatState();

	RefreshCombatPermissions();
}

void AVampsCharacter::SetCombatPhase(EVampsActionPhase NewPhase)
{
	CombatState.Phase = NewPhase;
	RefreshCombatPermissions();
}

bool AVampsCharacter::IsMatchCombatAllowed() const
{
	const AVampsMatchGameMode* MatchMode = GetWorld()->GetAuthGameMode<AVampsMatchGameMode>();
	return !MatchMode || MatchMode->AllowsCombat(this);
}

void AVampsCharacter::RefreshCombatPermissions()
{
	const EVampsActionPhase Phase = CombatState.Phase;

	const bool bWasDamageEnabled = CombatState.bCanDealDamage;

	CombatState.bIsAlive = Attributes->GetHealth() > 0.f;

	// recovery follows the active hit window. NA is the separate attack lock after the chain
	const bool bInRecovery = Phase == EVampsActionPhase::Recovery;
	const bool bInNA = Phase == EVampsActionPhase::NARecovery;
	CombatState.bIsRecovering = bInRecovery || bInNA;

	const bool bInReaction = CombatState.Reaction != EVampsCombatReaction::None;

	const bool bCombatAllowed = CombatState.bIsAlive && IsMatchCombatAllowed();

	// responding requires a granted attack ability, and NA or stun should lock that grant
	const bool bCanRespond =
	    bCombatAllowed && AttackHandle.IsValid() && !bInNA && CombatState.Reaction != EVampsCombatReaction::Stun;

	CombatState.bOffenseRestricted = bInReaction;

	CombatState.bCanMove = bCombatAllowed && !bInReaction && (Phase == EVampsActionPhase::Idle || bInNA);

	CombatState.bCanRequestCombat = bCanRespond && (!bInRecovery || CombatState.bComboWindowOpen || bInReaction);

	CombatState.bCanStartAction = CombatState.bCanRequestCombat && Phase == EVampsActionPhase::Idle && !bInReaction;

	CombatState.bCanDealDamage = bCombatAllowed && Phase == EVampsActionPhase::Contacts && !bInReaction;

	// defense uses a fresh request or the remembered attacker during guard
	CombatState.bCanDefend =
	    bCanRespond && (!bInRecovery || bInReaction) &&
	    (CombatState.bDefenseRequestArmed ||
	     (CombatState.Reaction == EVampsCombatReaction::Guard && CombatState.DefendedAttacker.IsValid()));

	if (bWasDamageEnabled != CombatState.bCanDealDamage)
	{
		SetCombatWeaponTracing(CombatState.bCanDealDamage);
	}
}

void AVampsCharacter::SetCombatWeaponTracing(bool bEnabled)
{
	for (AVampsWeapon* EquippedWeapon : Weapons)
	{
		if (IsValid(EquippedWeapon) && EquippedWeapon->SetTracingEnabled(bEnabled))
		{
			continue;
		}
		if (bEnabled)
		{
			AbilitySystem->CancelAbilityHandle(AttackHandle);
			return;
		}
	}
}

void AVampsCharacter::HandleCombatHealthChanged(const FOnAttributeChangeData&)
{
	const bool bAliveNow = Attributes->GetHealth() > 0.f;

	if (CombatState.bIsAlive == bAliveNow)
	{
		return;
	}
	if (bAliveNow)
	{
		RefreshCombatPermissions();
	}
	else
	{
		ClearCombatTimers();

		CombatState.Reaction = EVampsCombatReaction::None;
		CombatState.DefendedAttacker.Reset();
		CombatState.Phase = EVampsActionPhase::Idle;

		ClearMobilityIntent();
		CancelCombatOffense();

		GetWorld()->GetSubsystem<UVampsCombatResolver>()->DiscardPendingCombatFor(*this);
		static_cast<UVampsCombatMovementComponent*>(GetCharacterMovement())->ResetCombatMotion();
	}
	OnCombatAliveChanged.Broadcast();
}

bool AVampsCharacter::ReceiveCombatHit(const FVampsPendingContact& Contact, AVampsCharacter& Attacker)
{
	const float DamageAmount = Contact.bBlocked ? FMath::Max(0.f, Contact.ContactDamage - Contact.BlockReduction) : Contact.ContactDamage;

	FGameplayEffectContextHandle EffectContext = Attacker.AbilitySystem->MakeEffectContext();

	EffectContext.AddSourceObject(&Attacker);

	FGameplayEffectSpecHandle DamageSpec = Attacker.AbilitySystem->MakeOutgoingSpec(UGE_VampsDamage::StaticClass(), 1, EffectContext);

	if (!DamageSpec.IsValid())
	{
		return false;
	}

	CombatState.LastIncomingContact =
	    Contact.bGuardBreak ? EVampsContactResult::GuardBreak
	                        : (Contact.bBlocked ? EVampsContactResult::Blocked : EVampsContactResult::OpenHit);

	CombatState.bLastIncomingCritical = Contact.bCritical;

	CombatState.LastDamageReceived = DamageAmount;

	if (Contact.bBlocked)
	{
		AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetGuardLoadAttribute(),
		    FMath::Min(Attributes->GetMaxGuardLoad(), Attributes->GetGuardLoad() + Contact.GuardLoad));
		GuardRecoveryStartsAt = GetWorld()->GetTimeSeconds() + CombatSettings->GuardRecoveryDelay;
	}

	DamageSpec.Data->SetSetByCallerMagnitude(VampsTags::Damage, -DamageAmount);

	Attacker.AbilitySystem->ApplyGameplayEffectSpecToTarget(*DamageSpec.Data, AbilitySystem);

	if (!CombatState.bIsAlive)
	{
		PresentCombatDeath(Contact.bBlocked);

		if (AVampsMatchGameMode* MatchMode = Cast<AVampsMatchGameMode>(GetWorld()->GetAuthGameMode()))
		{
			MatchMode->RegisterCombatDeath(this, &Attacker);
		}
	}

	return true;
}

void AVampsCharacter::ApplyCombatReaction(const FVampsPendingContact& Contact)
{
	CombatState.ReactionSource = Contact.Attacker;

	ClearMobilityIntent();

	if (CombatState.Reaction == EVampsCombatReaction::Stun)
	{
		return;
	}

	const float Duration = Contact.bGuardBreak ? CombatSettings->StunDuration : Contact.StepSettings.ContactReactionDuration;

	if (!FMath::IsFinite(Duration) || Duration <= 0.f)
	{
		return;
	}

	CombatState.Reaction = Contact.bGuardBreak
	                     ? EVampsCombatReaction::Stun
	                     : (Contact.bBlocked ? EVampsCombatReaction::Guard : EVampsCombatReaction::Hit);

	GetWorldTimerManager().SetTimer(CombatReactionTimer, this, &AVampsCharacter::FinishCombatReaction, Duration);

	if (CombatState.Reaction != EVampsCombatReaction::Guard)
	{
		CombatState.DefendedAttacker.Reset();
		CombatState.DefendedActionNumber = INDEX_NONE;
		ClearDefenseRequest();
	}

	CancelCombatOffense();
	StopCombatReactionMontage();
	ConsumeMovementInputVector();
	GetCharacterMovement()->StopMovementImmediately();

	UAnimMontage* ReactionMontage =
	    CombatState.Reaction == EVampsCombatReaction::Guard
	        ? CombatSettings->GuardReactionMontage.Get()
	        : (CombatState.Reaction == EVampsCombatReaction::Hit ? CombatSettings->HitReactionMontage.Get() : nullptr);

	UAnimInstance* Animation = GetMesh()->GetAnimInstance();

	if (!Animation || !ReactionMontage || !FMath::IsFinite(ReactionMontage->RateScale) || ReactionMontage->RateScale <= 0.f)
	{
		return;
	}

	const float PlaybackDuration = Animation->Montage_Play(
	    ReactionMontage, ReactionMontage->GetPlayLength() / (Duration * ReactionMontage->RateScale));

	const FAnimMontageInstance* Playback = Animation->GetActiveInstanceForMontage(ReactionMontage);
	if (PlaybackDuration > 0.f && Playback)
	{
		ReactionPlaybackInstanceID = Playback->GetInstanceID();
	}
}
