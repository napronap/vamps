#include "Characters/VampsDuelCharacter.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"
#include "AI/DuelAIController.h"
#include "Combat/DuelCombatSubsystem.h"
#include "Combat/CombatTags.h"
#include "Combat/Attributes/VampsAttributeSet.h"
#include "Combat/Abilities/GA_Attack.h"
#include "Combat/Abilities/GA_Guard.h"
#include "Combat/Abilities/GA_NA.h"
#include "Combat/Abilities/GA_HitReaction.h"
#include "Combat/Effects/GE_DuelDamage.h"

DEFINE_LOG_CATEGORY_STATIC(LogDuelAnimation, Log, All);

AVampsDuelCharacter::AVampsDuelCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(34.f, 88.f);
	GetMesh()->SetRelativeLocation(FVector(0, 0, -88));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
	AbilitySystem->SetIsReplicated(true);
	Attributes = CreateDefaultSubobject<UVampsAttributeSet>(TEXT("Attributes"));

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 550;
	CameraBoom->TargetOffset = FVector(0, 0, 65);
	CameraBoom->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	AIControllerClass = ADuelAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

UAbilitySystemComponent* AVampsDuelCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

const UCombatConfig& AVampsDuelCharacter::Config() const
{
	return CombatConfig ? *CombatConfig : *GetDefault<UCombatConfig>();
}

double AVampsDuelCharacter::CombatTime() const
{
	return GetWorld()->GetSubsystem<UDuelCombatSubsystem>()->Now();
}

void AVampsDuelCharacter::BeginPlay()
{
	Super::BeginPlay();
	InitializeCombat();
}

void AVampsDuelCharacter::InitializeCombat()
{
	if (bInitialized || !HasAuthority())
	{
		return;
	}

	bInitialized = true;
	AbilitySystem->InitAbilityActorInfo(this, this);
	AbilitySystem->AddAttributeSetSubobject(Attributes.Get());

	auto Grant = [this](TSubclassOf<UGameplayAbility> Class)
	{
		const FGameplayAbilitySpecHandle Handle = AbilitySystem->GiveAbility(FGameplayAbilitySpec(Class, 1));
		return AbilitySystem->FindAbilitySpecFromHandle(Handle)->GetPrimaryInstance();
	};

	Attack = Cast<UGA_Attack>(Grant(UGA_Attack::StaticClass()));
	Guard = Cast<UGA_Guard>(Grant(UGA_Guard::StaticClass()));
	NA = Cast<UGA_NA>(Grant(UGA_NA::StaticClass()));
	HitReaction = Cast<UGA_HitReaction>(Grant(UGA_HitReaction::StaticClass()));

	GetWorld()->GetSubsystem<UDuelCombatSubsystem>()->Register(this);
	ResetFighter(GetActorTransform());
}

void AVampsDuelCharacter::EndPlay(const EEndPlayReason::Type Reason)
{
	if (UDuelCombatSubsystem* Combat = GetWorld()->GetSubsystem<UDuelCombatSubsystem>())
	{
		Combat->Unregister(this);
	}

	AbilitySystem->CancelAllAbilities();
	Super::EndPlay(Reason);
}

bool AVampsDuelCharacter::IsDefeated() const
{
	return Attributes->GetHealth() <= 0;
}

float AVampsDuelCharacter::GetHealth() const
{
	return Attributes->GetHealth();
}

float AVampsDuelCharacter::GetMaxHealth() const
{
	return Attributes->GetMaxHealth();
}

float AVampsDuelCharacter::GetNARemaining() const
{
	return NA && NA->IsActive() ? float(NA->Clock.Remaining) : 0.f;
}

float AVampsDuelCharacter::GetNAPause() const
{
	return NA && NA->IsActive() ? float(FMath::Max(0., NA->Clock.ResumeAt - CombatTime())) : 0.f;
}

EDuelPhase AVampsDuelCharacter::GetPhase() const
{
	if (IsDefeated())
	{
		return EDuelPhase::Defeated;
	}

	if (NA && NA->IsActive())
	{
		return EDuelPhase::NA;
	}

	if (Attack && Attack->IsActive())
	{
		return Attack->Phase(CombatTime());
	}

	if (Guard && Guard->IsActive())
	{
		return EDuelPhase::Guard;
	}

	if (HitReaction && HitReaction->IsActive())
	{
		return EDuelPhase::HitReaction;
	}

	return EDuelPhase::Default;
}

void AVampsDuelCharacter::ResetFighter(const FTransform& SpawnTransform)
{
	AbilitySystem->CancelAllAbilities();
	AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetMaxHealthAttribute(),
	                                       FMath::Max(1.f, Config().MaxHealth));
	AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetHealthAttribute(),
	                                       FMath::Max(1.f, Config().MaxHealth));

	ClearInput();
	MoveIntent = FVector::ZeroVector;
	LastResult = EDuelResult::None;
	LastEvent = TEXT("Ready");
	ContactsResolved = 0;

	StopCombatAnimation();
	SetActorTransform(SpawnTransform, false, nullptr, ETeleportType::TeleportPhysics);
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	GetCharacterMovement()->MaxWalkSpeed = FMath::Max(0.f, Config().MoveSpeed);
}

void AVampsDuelCharacter::SetTarget(AVampsDuelCharacter* NewTarget)
{
	Target = NewTarget != this ? NewTarget : nullptr;
}

AVampsDuelCharacter* AVampsDuelCharacter::GetTarget() const
{
	return Target.IsValid() && !Target->IsDefeated() ? Target.Get() : nullptr;
}

void AVampsDuelCharacter::PressShared()
{
	if (!bInitialized || IsDefeated())
	{
		return;
	}

	GetWorld()->GetSubsystem<UDuelCombatSubsystem>()->Synchronize();
	const double Time = CombatTime();
	EDuelInputIntent Intent = EDuelInputIntent::StartAttack;

	if (Guard->IsActive() || HitReaction->IsActive())
	{
		Intent = EDuelInputIntent::Guard;
	}
	else if (Attack->IsActive())
	{
		Intent = EDuelInputIntent::ContinueAttack;
	}

	Input.Store(Intent, Time, Config().InputBuffer);
	ConsumeInput(Time);
}

bool AVampsDuelCharacter::TryInput(EDuelInputIntent Intent, double Time)
{
	if (IsDefeated() || NA->IsActive())
	{
		return false;
	}

	if (Intent == EDuelInputIntent::ContinueAttack)
	{
		return Attack->Continue(Time);
	}

	if (Intent == EDuelInputIntent::Guard)
	{
		if (Guard->IsActive())
		{
			return Guard->Press(Time);
		}

		if (HitReaction->CanRespond(Time))
		{
			HitReaction->Finish();
			StartGuard(Time, false);
			return Guard->IsActive();
		}

		return false;
	}

	if (Intent != EDuelInputIntent::StartAttack || Attack->IsActive() || Guard->IsActive() || HitReaction->IsActive())
	{
		return false;
	}

	if (!AbilitySystem->TryActivateAbilityByClass(UGA_Attack::StaticClass()))
	{
		return false;
	}

	GetCharacterMovement()->StopMovementImmediately();
	Attack->Start(Time);
	return true;
}

void AVampsDuelCharacter::ConsumeInput(double Time)
{
	Input.Expire(Time);

	if (Input.Intent != EDuelInputIntent::None && TryInput(Input.Intent, Time))
	{
		Input.Clear();
	}
}

void AVampsDuelCharacter::StartNA(double Time, bool bOffensive)
{
	if (NA->IsActive() || IsDefeated())
	{
		return;
	}

	if (AbilitySystem->TryActivateAbilityByClass(UGA_NA::StaticClass()))
	{
		NA->Start(Time, bOffensive ? Config().OffensiveNA : Config().DefensiveNA);
	}
}

void AVampsDuelCharacter::StartGuard(double Time, bool bAlreadyBlocked)
{
	if (NA->IsActive() || IsDefeated())
	{
		return;
	}

	Attack->Finish(true);
	ClearInput();

	if (AbilitySystem->TryActivateAbilityByClass(UGA_Guard::StaticClass()))
	{
		Guard->Start(Time, bAlreadyBlocked);
	}
}

bool AVampsDuelCharacter::CanMove(double Time) const
{
	if (IsDefeated())
	{
		return false;
	}

	if (HitReaction && HitReaction->IsActive() && !HitReaction->CanRespond(Time))
	{
		return false;
	}

	return !Attack || !Attack->IsActive() || Attack->Phase(Time) == EDuelPhase::Recovery;
}

void AVampsDuelCharacter::SetMoveIntent(FVector WorldDirection)
{
	MoveIntent = WorldDirection.GetClampedToMaxSize(1.f);
	MoveIntent.Z = 0;
}

void AVampsDuelCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bInitialized)
	{
		return;
	}

	const double Time = CombatTime();

	if (!MoveIntent.IsNearlyZero() && CanMove(Time))
	{
		if (Attack->IsActive())
		{
			Attack->Leave(Time);
		}

		if (Guard->IsActive())
		{
			Guard->Leave(Time);
		}

		if (HitReaction->CanRespond(Time))
		{
			HitReaction->Finish();
			ClearInput();
		}

		AddMovementInput(MoveIntent, 1.f);
	}

	if (!Attack->IsActive() && CanMove(Time))
	{
		FVector Facing = GetTarget() ? GetTarget()->GetActorLocation() - GetActorLocation() : MoveIntent;

		if (!Facing.IsNearlyZero())
		{
			SetActorRotation(FRotator(0, Facing.Rotation().Yaw, 0));
		}
	}
}

void AVampsDuelCharacter::AdvanceActions(double Time)
{
	if (!bInitialized || IsDefeated())
	{
		return;
	}

	NA->Advance(Time);
	HitReaction->Advance(Time);
	Attack->Advance(Time);
	Guard->Advance(Time);
	ConsumeInput(Time);
}

void AVampsDuelCharacter::ResetOnParry()
{
	AbilitySystem->CancelAllAbilities();
	ClearInput();
	StopCombatAnimation();
}

EDuelResult AVampsDuelCharacter::ReceiveContact(AVampsDuelCharacter* Attacker, const FDuelStrike& Strike, double Time)
{
	if (!Attacker || IsDefeated())
	{
		return EDuelResult::None;
	}

	const bool bCovered = DuelRules::InsideDefense(
	    GetActorForwardVector(), Attacker->GetActorLocation() - GetActorLocation(), Config().DefenseAngle);
	EDuelResult Result = EDuelResult::Hit;

	if (!NA->IsActive() && bCovered)
	{
		if (Attack->IsActive() && Attack->Phase(Time) == EDuelPhase::Parry)
		{
			Result = EDuelResult::Parry;
			ResetOnParry();
		}
		else if (Attack->IsActive() && Attack->Phase(Time) == EDuelPhase::Active)
		{
			Result = EDuelResult::Block;
			StartGuard(Time, true);
		}
		else if (Guard->CanBlock(Time))
		{
			Result = EDuelResult::Block;
			Guard->Blocked(Time);
		}
	}

	if (Result == EDuelResult::Hit)
	{
		// Damage does not create NA: only pause it when it was already active.
		NA->OnDamage(Time);
		Attack->Finish(true);
		Guard->Finish(true);
		ClearInput();
		StopCombatAnimation();
		FGameplayEffectContextHandle Context = Attacker->AbilitySystem->MakeEffectContext();
		Context.AddSourceObject(Attacker);
		FGameplayEffectSpecHandle Spec =
		    Attacker->AbilitySystem->MakeOutgoingSpec(UGE_DuelDamage::StaticClass(), 1, Context);

		if (Spec.IsValid())
		{
			Spec.Data->SetSetByCallerMagnitude(DuelTags::Damage, -FMath::Max(0.f, Strike.Damage));
			Attacker->AbilitySystem->ApplyGameplayEffectSpecToTarget(*Spec.Data, AbilitySystem);
		}

		GetCharacterMovement()->StopMovementImmediately();

		if (!IsDefeated())
		{
			if (!HitReaction->IsActive())
			{
				AbilitySystem->TryActivateAbilityByClass(UGA_HitReaction::StaticClass());
			}

			HitReaction->Start(Time);
		}
		else
		{
			AbilitySystem->CancelAllAbilities();
			MoveIntent = FVector::ZeroVector;
			GetCharacterMovement()->DisableMovement();
		}
	}

	const FVector Away = (GetActorLocation() - Attacker->GetActorLocation()).GetSafeNormal2D();
	MoveSwept(Away * (Result == EDuelResult::Hit     ? Config().HitPushback
	                  : Result == EDuelResult::Block ? Config().BlockPushback
	                                                 : Config().ParryPushback));
	LastResult = Result;
	LastEvent =
	    FString::Printf(TEXT("%s @ %.3f"), *StaticEnum<EDuelResult>()->GetNameStringByValue(int64(Result)), Time);
	++ContactsResolved;
	return Result;
}

void AVampsDuelCharacter::MoveSwept(FVector Delta)
{
	FHitResult Hit;
	AddActorWorldOffset(Delta, true, &Hit);
}

void AVampsDuelCharacter::PlayAttackSection(const FDuelStrike& Strike, FName Section, double Duration)
{
	UAnimInstance* Anim = GetMesh()->GetAnimInstance();
	UAnimMontage* Montage = Config().AttackMontage;

	if (!Anim || !Montage)
	{
		return;
	}

	const int32 Index = Montage->GetSectionIndex(Section);

	if (Index == INDEX_NONE)
	{
		UE_LOG(LogDuelAnimation,
		       Warning,
		       TEXT("Cannot play attack on %s: montage %s is missing section %s."),
		       *GetName(),
		       *GetNameSafe(Montage),
		       *Section.ToString());
		return;
	}

	if (!Anim->Montage_IsPlaying(Montage) && Anim->Montage_Play(Montage) <= 0.f)
	{
		UE_LOG(LogDuelAnimation,
		       Warning,
		       TEXT("Cannot start attack montage %s on %s. Check the montage length, skeleton and slots."),
		       *GetNameSafe(Montage),
		       *GetName());
		return;
	}

	// Link the whole strike before advancing so only recovery can trigger automatic blend-out.
	Anim->Montage_SetNextSection(Strike.PrepareSection, Strike.ActiveSection, Montage);
	Anim->Montage_SetNextSection(Strike.ActiveSection, Strike.RecoverySection, Montage);
	Anim->Montage_SetNextSection(Strike.RecoverySection, NAME_None, Montage);
	Anim->Montage_JumpToSection(Section, Montage);
	Anim->Montage_SetPlayRate(Montage, Montage->GetSectionLength(Index) / FMath::Max(0.01f, float(Duration)));
}

void AVampsDuelCharacter::PlayReaction(bool bGuard)
{
	if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
	{
		Anim->Montage_Stop(0.05f);

		if (UAnimMontage* Montage = bGuard ? Config().GuardMontage.Get() : Config().HitMontage.Get())
		{
			Anim->Montage_Play(Montage);
		}
	}
}

void AVampsDuelCharacter::StopCombatAnimation()
{
	if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
	{
		Anim->Montage_Stop(0.05f);
	}
}

void AVampsDuelCharacter::DrawCombat() const
{
	if (!Config().bDrawCombat)
	{
		return;
	}

	const FColor Color = IsDefeated()                       ? FColor::Silver
	                     : GetPhase() == EDuelPhase::Parry  ? FColor::Cyan
	                     : GetPhase() == EDuelPhase::Active ? FColor::Red
	                     : GetPhase() == EDuelPhase::NA     ? FColor::Orange
	                                                        : FColor::Green;
	DrawDebugCapsule(GetWorld(), GetActorLocation(), 88, 34, GetActorQuat(), Color, false, -1, 0, 1.5f);

	if (Attack && Attack->IsActive())
	{
		const FVector End = GetActorLocation() + Attack->Direction * Config().Strike(Attack->StrikeIndex).Reach;
		DrawDebugLine(GetWorld(), GetActorLocation(), End, Color, false, -1, 0, 3);
	}
}
