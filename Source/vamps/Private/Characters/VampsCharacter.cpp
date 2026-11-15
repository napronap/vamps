#include "Characters/VampsCharacter.h"
#include "Characters/VampsLockOnComponent.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Combat/Abilities/GA_Attack.h"
#include "Combat/Abilities/GA_Dash.h"
#include "Combat/Abilities/GA_Defend.h"
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
	AbilitySystem->GetGameplayAttributeValueChangeDelegate(UVampsAttributeSet::GetHealthAttribute())
	    .AddUObject(this, &AVampsCharacter::HandleCombatHealthChanged);

	if (InitializeCombatWeapons() && HasAuthority())
	{
		AttackHandle = AbilitySystem->GiveAbility(FGameplayAbilitySpec(UGA_Attack::StaticClass(), 1));
		DashHandle = AbilitySystem->GiveAbility(FGameplayAbilitySpec(UGA_Dash::StaticClass(), 1));
		DefendHandle = AbilitySystem->GiveAbility(FGameplayAbilitySpec(UGA_Defend::StaticClass(), 1));
		AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetMaxGuardLoadAttribute(),
		                                       CombatSettings->GuardCapacity);
		AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetMaxMobilityEnergyAttribute(),
		                                       CombatSettings->MobilityEnergyCapacity);
		AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetMobilityEnergyAttribute(),
		                                       CombatSettings->MobilityEnergyCapacity);
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
	if (CombatState.bDashRequested)
	{
		return;
	}

	CombatState.bDashRequested = true;

	// Let impacts cancel the request before GA_Dash spends energy.
	GetWorld()->GetSubsystem<UVampsCombatResolver>()->RunAfterCombatResolution(
	    FSimpleDelegate::CreateUObject(this, &AVampsCharacter::StartRequestedDash));
}

void AVampsCharacter::StartRequestedDash()
{
	if (!CombatState.bDashRequested)
	{
		return;
	}

	CombatState.bDashRequested = false;
	if (!CanStartCombatDash())
	{
		bMobilityHeld = false;
		return;
	}
	if (!AbilitySystem->TryActivateAbility(DashHandle) || !CombatState.bIsDashing)
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

		EquippedWeapon->AttachToComponent(
		    GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSettings.AttachSocket);
		if (!EquippedWeapon->InitializeCombatTrace(*this))
		{
			return false;
		}
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

	CombatState.MontageInstanceID = INDEX_NONE;
	AbilitySystem->CancelAllAbilities();

	DestroyCombatWeapons();

	if (UVampsCombatResolver* Resolver = GetWorld()->GetSubsystem<UVampsCombatResolver>())
	{
		Resolver->DiscardPendingCombatFor(*this);
	}
	OnCombatReset.Broadcast();

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

	// fighters can request a defense if they are playing a hit or a guard animation so that they can prevent hit from the next attack
	if (CombatState.Reaction == EVampsCombatReaction::Hit || CombatState.Reaction == EVampsCombatReaction::Guard)
	{
		RequestCombatDefense();
		return;
	}

	if (!CombatState.bCanRequestCombat)
	{
		return;
	}

	if (!IsMatchCombatAllowed())
	{
		return;
	}

	RequestCombatDefense();

	if (CombatState.bCanStartAction)
	{
		AbilitySystem->CancelAbilityHandle(DashHandle);
		AbilitySystem->TryActivateAbility(AttackHandle);
		return;
	}

	if (CombatState.bContinuationRequested)
	{
		return;
	}

	if (!CanRequestCombatContinuation())
	{
		return;
	}

	// buffer one input until the combo window closes and the resolver checks the hit
	CombatState.bContinuationRequested = true;
}

void AVampsCharacter::RequestCombatDefense()
{
	if (!HasAuthority() || !CombatState.bIsAlive || !IsMatchCombatAllowed())
	{
		return;
	}
	if (CombatState.Phase == EVampsActionPhase::NARecovery)
	{
		return;
	}
	if (CombatState.Reaction == EVampsCombatReaction::Stun)
	{
		return;
	}

	AbilitySystem->TryActivateAbility(DefendHandle);
}

bool AVampsCharacter::CanRequestCombatContinuation() const
{
	if (!CombatState.bCanRequestCombat)
	{
		return false;
	}

	if (!CombatState.bComboWindowOpen)
	{
		return false;
	}
	return CombatState.ComboStep < CombatSettings->AttackMontage->GetNumSections();
}

void AVampsCharacter::ConfirmCombatDefense(const FVampsPendingContact& Contact)
{
	AbilitySystem->CancelAbilityHandle(DefendHandle);

	if (CombatState.Reaction == EVampsCombatReaction::Guard)
	{
		return;
	}

	LockOn->SelectTarget(Contact.Attacker.Get());
	OnCombatFramingRequested.Broadcast(Contact.Attacker.Get());
}

void AVampsCharacter::CreditCombatContact(const FVampsPendingContact& Contact)
{
	if (CombatState.ActionNumber != Contact.ActionNumber)
	{
		return;
	}

	if (CombatState.MontageInstanceID != Contact.AttackerMontageInstanceID)
	{
		return;
	}

	CombatState.bActionConnected = true;
}

void AVampsCharacter::ReactToCombatResolution(const FVampsCombatResolution& Resolution)
{
	const FVampsPendingContact& ReactionContact = Resolution.ReactionContact;
	bool bReactionContactApplied = false;

	for (const FVampsPendingContact& Contact : Resolution.Contacts)
	{
		AVampsCharacter* Attacker = Contact.Attacker.Get();
		if (!Attacker)
		{
			continue;
		}

		ReceiveCombatHit(Contact, *Attacker);
		if (!Contact.Recipient.IsValid() || !CombatState.bIsAlive)
		{
			return;
		}

		if (&Contact == &ReactionContact)
		{
			bReactionContactApplied = true;
		}
	}

	if (!bReactionContactApplied)
	{
		return;
	}

	ApplyCombatReaction(ReactionContact);
	auto* CombatMovement = static_cast<UVampsCombatMovementComponent*>(GetCharacterMovement());
	CombatMovement->ReceiveCombatDisplacement(ReactionContact.AttackOrigin,
	                                          ReactionContact.AttackDirection,
	                                          ReactionContact.StepSettings,
	                                          ReactionContact.bBlocked);
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

void AVampsCharacter::QueueWeaponContact(AVampsCharacter& Recipient)
{
	const UVampsCombatSettings& AttackSettings = *CombatSettings;
	const float CriticalChance = AttackSettings.CriticalChancePercent;
	bool bCritical = CriticalChance >= 100.f;
	if (!bCritical && CriticalChance > 0.f)
	{
		bCritical = FMath::FRand() * 100.f < CriticalChance;
	}

	FVampsPendingContact Contact;
	Contact.Attacker = this;
	Contact.Recipient = &Recipient;
	Contact.AttackerMontageInstanceID = CombatState.MontageInstanceID;
	if (Recipient.CombatState.bCanDealDamage)
	{
		Contact.RecipientAttackMontageInstanceID = Recipient.CombatState.MontageInstanceID;
	}
	Contact.ActionNumber = CombatState.ActionNumber;
	Contact.ContactDamage = bCritical ? AttackSettings.CriticalDamage : AttackSettings.AttackDamage;
	Contact.BlockReduction = Recipient.CombatSettings->BlockDamageReduction;
	Contact.GuardLoad = AttackSettings.GuardLoadPerHit;
	Contact.AttackOrigin = GetActorLocation();
	Contact.AttackDirection = GetActorForwardVector().GetSafeNormal2D();
	Contact.StepSettings = AttackSettings.ComboSteps[CombatState.ComboStep - 1];

	GetWorld()->GetSubsystem<UVampsCombatResolver>()->QueueCombatContact(MoveTemp(Contact));
}

void AVampsCharacter::StopMatchCombat()
{
	ClearCombatTimers();
	CombatState.Reaction = EVampsCombatReaction::None;
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
		Animation->Montage_Play(bBlockedDeath ? CombatSettings->GuardDeathMontage.Get()
		                                      : CombatSettings->DeathMontage.Get());
	}
}

bool AVampsCharacter::BeginCombatAction(int32 PlaybackInstanceID)
{
	if (!CombatState.bCanStartAction)
	{
		return false;
	}

	CombatState.MontageInstanceID = PlaybackInstanceID;
	GetWorld()->GetSubsystem<UVampsCombatResolver>()->StartAttack(*this);

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

	CombatState.bActionConnected = false;
	CombatState.bComboWindowOpen = false;
	CombatState.bContinuationRequested = false;

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

void AVampsCharacter::ClearAttackPlayback()
{
	static_cast<UVampsCombatMovementComponent*>(GetCharacterMovement())->StopVoluntaryMotion();

	const bool bHadAttackPlayback = CombatState.MontageInstanceID != INDEX_NONE;
	CombatState.MontageInstanceID = INDEX_NONE;
	CombatState.bComboWindowOpen = false;
	CombatState.bContinuationRequested = false;
	if (bHadAttackPlayback)
	{
		AbilitySystem->CancelAbilityHandle(DefendHandle);
	}
}

void AVampsCharacter::EndCombatRecovery(const FBranchingPointNotifyPayload& Payload)
{
	if (!IsCurrentCombatNotify(Payload) || CombatState.Phase != EVampsActionPhase::Recovery ||
	    CombatState.bComboWindowOpen)
	{
		return;
	}

	// Hold this step until contacts and any requested continuation have resolved.
	GetMesh()->GetAnimInstance()->Montage_Pause(CombatSettings->AttackMontage);
	GetWorld()->GetSubsystem<UVampsCombatResolver>()->RunAfterCombatResolution(
	    FSimpleDelegate::CreateUObject(this, &AVampsCharacter::FinishAttackRecovery, CombatState.ActionNumber));
}

void AVampsCharacter::FinishAttackRecovery(int32 RecoveryActionNumber)
{
	if (RecoveryActionNumber != CombatState.ActionNumber)
	{
		return;
	}
	if (CombatState.Phase != EVampsActionPhase::Recovery)
	{
		return;
	}

	ClearAttackPlayback();
	GetWorldTimerManager().SetTimer(
	    NARecoveryTimer, this, &AVampsCharacter::FinishNARecovery, CombatSettings->NARecoveryDuration);
	SetCombatPhase(EVampsActionPhase::NARecovery);
	AbilitySystem->CancelAbilityHandle(AttackHandle);
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
		CancelCombatOffense();
		return;
	}

	if (bEnabled && !CombatSettings->ComboSteps.IsValidIndex(CombatState.ComboStep - 1))
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: attack trace rejected: attacker='%s' has no settings for combo step %d in '%s'."),
		       __FUNCTION__,
		       *GetPathName(),
		       CombatState.ComboStep,
		       *CombatSettings->GetPathName());
		CancelCombatOffense();
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
		if (!Weapons.IsValidIndex(WeaponIndex))
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: '%s': weapon index %d is outside the equipped weapon list (%d entries) for '%s'."),
			       __FUNCTION__,
			       *GetPathName(),
			       WeaponIndex,
			       Weapons.Num(),
			       *GetPathNameSafe(CombatSettings));
			return;
		}

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
		// A weapon can still connect in the frame that closes the window.
		GetWorld()->GetSubsystem<UVampsCombatResolver>()->RunAfterCombatResolution(
		    FSimpleDelegate::CreateUObject(this, &AVampsCharacter::TryContinueCombatCombo, CombatState.ActionNumber));
	}
}

void AVampsCharacter::TryContinueCombatCombo(int32 RequestedActionNumber)
{
	if (RequestedActionNumber != CombatState.ActionNumber)
	{
		return;
	}
	if (!CombatState.bContinuationRequested)
	{
		return;
	}

	CombatState.bContinuationRequested = false;

	if (!CombatState.bActionConnected)
	{
		return;
	}

	UAnimInstance* Animation = GetMesh()->GetAnimInstance();
	if (!Animation)
	{
		CancelCombatOffense();
		return;
	}

	const FAnimMontageInstance* Playback = Animation->GetMontageInstanceForID(CombatState.MontageInstanceID);

	if (!Playback || !Playback->IsValid())
	{
		CancelCombatOffense();
		return;
	}

	if (Playback->IsStopped())
	{
		return;
	}

	UAnimMontage* Montage = Playback->Montage;
	const FName NextSection = Montage->GetSectionName(CombatState.ComboStep);
	++CombatState.ComboStep;

	BeginCombatStep();

	Animation->Montage_JumpToSection(NextSection, Montage);
	Animation->Montage_Resume(Montage);
}

void AVampsCharacter::ClearCombatTimers()
{
	AbilitySystem->CancelAbilityHandle(DefendHandle);
	GetWorldTimerManager().ClearTimer(NARecoveryTimer);
	GetWorldTimerManager().ClearTimer(CombatReactionTimer);
	ReactionPlaybackInstanceID = INDEX_NONE;
}

float AVampsCharacter::GetCombatReactionTimeRemaining() const
{
	return FMath::Max(0.f, GetWorldTimerManager().GetTimerRemaining(CombatReactionTimer));
}

float AVampsCharacter::GetNARecoveryTimeRemaining() const
{
	return FMath::Max(0.f, GetWorldTimerManager().GetTimerRemaining(NARecoveryTimer));
}

void AVampsCharacter::CancelCombatOffense(bool bCancelAbility)
{
	ClearAttackPlayback();
	CombatState.ComboStep = 0;

	SetCombatPhase(CombatState.Phase == EVampsActionPhase::NARecovery ? EVampsActionPhase::NARecovery
	                                                                  : EVampsActionPhase::Idle);

	if (bCancelAbility)
	{
		AbilitySystem->CancelAbilityHandle(AttackHandle);
	}
}

void AVampsCharacter::CancelCombatAction()
{
	CancelCombatOffense();
	AbilitySystem->CancelAbilityHandle(DefendHandle);
}

void AVampsCharacter::FinishCombatReaction()
{
	GetWorldTimerManager().ClearTimer(CombatReactionTimer);

	if (CombatState.Reaction == EVampsCombatReaction::Stun)
	{
		AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetGuardLoadAttribute(), 0.f);
		GuardRecoveryStartsAt = GetWorld()->GetTimeSeconds();
	}

	StopCombatReactionMontage();

	CombatState.Reaction = EVampsCombatReaction::None;
	AbilitySystem->CancelAbilityHandle(DefendHandle);

	RefreshCombatPermissions();
}

void AVampsCharacter::HandleGuardMontageEnded(UAnimMontage*, bool, int32 PlaybackInstanceID)
{
	if (ReactionPlaybackInstanceID != PlaybackInstanceID)
	{
		return;
	}

	ReactionPlaybackInstanceID = INDEX_NONE;
}

void AVampsCharacter::StopCombatReactionMontage()
{
	if (ReactionPlaybackInstanceID == INDEX_NONE)
	{
		return;
	}

	UAnimInstance* Animation = GetMesh()->GetAnimInstance();
	const FAnimMontageInstance* Playback =
	    Animation ? Animation->GetMontageInstanceForID(ReactionPlaybackInstanceID) : nullptr;
	ReactionPlaybackInstanceID = INDEX_NONE;

	if (Playback)
	{
		Animation->Montage_Stop(0.05f, Playback->Montage);
	}
}

void AVampsCharacter::FinishNARecovery()
{
	CombatState.ComboStep = 0;
	SetCombatPhase(EVampsActionPhase::Idle);
}

void AVampsCharacter::ResetCombatState()
{
	ClearMobilityIntent();
	ClearCombatTimers();

	bHasMobilityInput = false;
	MobilityRecoveryStartsAt = 0;

	if (CombatSettings)
	{
		AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetMaxMobilityEnergyAttribute(),
		                                       CombatSettings->MobilityEnergyCapacity);
		AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetMobilityEnergyAttribute(),
		                                       CombatSettings->MobilityEnergyCapacity);
	}

	CombatState.MontageInstanceID = INDEX_NONE;

	AbilitySystem->CancelAllAbilities();

	GuardRecoveryStartsAt = 0;

	AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetGuardLoadAttribute(), 0.f);

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

	if (UVampsCombatResolver* Resolver = GetWorld()->GetSubsystem<UVampsCombatResolver>())
	{
		Resolver->DiscardPendingCombatFor(*this);
	}
	OnCombatReset.Broadcast();
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

	const bool bInReaction = CombatState.Reaction != EVampsCombatReaction::None;

	const bool bCombatAllowed = CombatState.bIsAlive && IsMatchCombatAllowed();

	const bool bCanRespond = bCombatAllowed && AttackHandle.IsValid() && !bInNA;

	CombatState.bOffenseRestricted = bInReaction;

	CombatState.bCanMove = bCombatAllowed && !bInReaction && (Phase == EVampsActionPhase::Idle || bInNA);

	CombatState.bCanRequestCombat = bCanRespond && !bInReaction && (!bInRecovery || CombatState.bComboWindowOpen);

	CombatState.bCanStartAction = CombatState.bCanRequestCombat && Phase == EVampsActionPhase::Idle;

	CombatState.bCanDealDamage = bCombatAllowed && Phase == EVampsActionPhase::Contacts && !bInReaction;

	CombatState.bCanDefend =
	    bCanRespond && CombatState.Reaction != EVampsCombatReaction::Stun && CombatState.bDefenseRequestArmed;

	if (bWasDamageEnabled != CombatState.bCanDealDamage)
	{
		SetCombatWeaponTracing(CombatState.bCanDealDamage);
	}
}

void AVampsCharacter::SetCombatWeaponTracing(bool bEnabled)
{
	const bool bEnableOnAuthority = bEnabled && HasAuthority();
	for (AVampsWeapon* EquippedWeapon : Weapons)
	{
		if (IsValid(EquippedWeapon))
		{
			EquippedWeapon->SetTracingEnabled(bEnableOnAuthority);
			continue;
		}
		if (bEnableOnAuthority)
		{
			CancelCombatOffense();
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
		CombatState.Phase = EVampsActionPhase::Idle;

		ClearMobilityIntent();
		CancelCombatOffense();

		static_cast<UVampsCombatMovementComponent*>(GetCharacterMovement())->ResetCombatMotion();
	}
	OnCombatAliveChanged.Broadcast();
}

void AVampsCharacter::ReceiveCombatHit(const FVampsPendingContact& Contact, AVampsCharacter& Attacker)
{
	FGameplayEffectContextHandle EffectContext = Attacker.AbilitySystem->MakeEffectContext();
	EffectContext.AddSourceObject(&Attacker);

	FGameplayEffectSpecHandle DamageSpec =
	    Attacker.AbilitySystem->MakeOutgoingSpec(UGE_VampsDamage::StaticClass(), 1, EffectContext);

	float DamageAmount = Contact.ContactDamage;
	if (Contact.bBlocked)
	{
		DamageAmount = FMath::Max(0.f, DamageAmount - Contact.BlockReduction);
		AbilitySystem->SetNumericAttributeBase(
		    UVampsAttributeSet::GetGuardLoadAttribute(),
		    FMath::Min(Attributes->GetMaxGuardLoad(), Attributes->GetGuardLoad() + Contact.GuardLoad));
		GuardRecoveryStartsAt = GetWorld()->GetTimeSeconds() + CombatSettings->GuardRecoveryDelay;
	}

	DamageSpec.Data->SetSetByCallerMagnitude(VampsTags::Damage, -DamageAmount);
	Attacker.AbilitySystem->ApplyGameplayEffectSpecToTarget(*DamageSpec.Data, AbilitySystem);

	if (CombatState.bIsAlive)
	{
		return;
	}

	PresentCombatDeath(Contact.bBlocked);

	if (AVampsMatchGameMode* MatchMode = GetWorld()->GetAuthGameMode<AVampsMatchGameMode>())
	{
		MatchMode->RegisterCombatDeath(this, &Attacker);
	}
}

void AVampsCharacter::ApplyCombatReaction(const FVampsPendingContact& Contact)
{
	ClearMobilityIntent();

	if (CombatState.Reaction == EVampsCombatReaction::Stun)
	{
		return;
	}
	float Duration = Contact.StepSettings.ContactReactionDuration;
	EVampsCombatReaction NewReaction = EVampsCombatReaction::Hit;
	if (Contact.bGuardBreak)
	{
		Duration = CombatSettings->StunDuration;
		NewReaction = EVampsCombatReaction::Stun;
	}
	else if (Contact.bBlocked)
	{
		NewReaction = EVampsCombatReaction::Guard;
	}

	if (!FMath::IsFinite(Duration) || Duration <= 0.f)
	{
		return;
	}

	// The contact refreshes the action lock even when the existing guard montage keeps playing.
	GetWorldTimerManager().SetTimer(CombatReactionTimer, this, &AVampsCharacter::FinishCombatReaction, Duration);
	if (CombatState.Reaction == EVampsCombatReaction::Guard && NewReaction == EVampsCombatReaction::Guard &&
	    ReactionPlaybackInstanceID != INDEX_NONE)
	{
		return;
	}

	StopCombatReactionMontage();
	CombatState.Reaction = NewReaction;

	if (NewReaction != EVampsCombatReaction::Guard)
	{
		AbilitySystem->CancelAbilityHandle(DefendHandle);
	}

	CancelCombatOffense();
	ConsumeMovementInputVector();
	GetCharacterMovement()->StopMovementImmediately();

	if (NewReaction == EVampsCombatReaction::Stun)
	{
		return;
	}

	UAnimInstance* Animation = GetMesh()->GetAnimInstance();
	if (NewReaction == EVampsCombatReaction::Hit)
	{
		PlayCombatReactionMontage(Animation, CombatSettings->HitReactionMontage.Get(), Duration);
		return;
	}

	UAnimMontage* GuardMontage = CombatSettings->GuardReactionMontage.Get();
	if (!GuardMontage || !GuardMontage->bEnableAutoBlendOut ||
	    !PlayCombatReactionMontage(Animation, GuardMontage, Duration))
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s' could not play guard montage '%s' with Auto Blend Out enabled."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(GuardMontage));
		return;
	}

	FOnMontageEnded GuardEnded;
	GuardEnded.BindUObject(this, &AVampsCharacter::HandleGuardMontageEnded, ReactionPlaybackInstanceID);
	Animation->Montage_SetEndDelegate(GuardEnded, GuardMontage);
}

bool AVampsCharacter::PlayCombatReactionMontage(UAnimInstance* Animation, UAnimMontage* ReactionMontage, float Duration)
{
	if (!Animation || !ReactionMontage)
	{
		return false;
	}

	const float MontageRate = ReactionMontage->RateScale;
	if (!FMath::IsFinite(MontageRate) || MontageRate <= 0.f)
	{
		return false;
	}

	const float PlaybackDuration =
	    Animation->Montage_Play(ReactionMontage, ReactionMontage->GetPlayLength() / (Duration * MontageRate));

	const FAnimMontageInstance* Playback = Animation->GetActiveInstanceForMontage(ReactionMontage);
	if (PlaybackDuration <= 0.f || !Playback)
	{
		return false;
	}

	ReactionPlaybackInstanceID = Playback->GetInstanceID();
	return true;
}
