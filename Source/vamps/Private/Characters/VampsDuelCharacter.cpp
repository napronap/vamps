#include "Characters/VampsDuelCharacter.h"
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Combat/Abilities/GA_Attack.h"
#include "Combat/Abilities/GA_Guard.h"
#include "Combat/Attributes/VampsAttributeSet.h"
#include "Combat/CombatTags.h"
#include "Combat/Effects/GE_DuelDamage.h"
#include "Combat/FistHitboxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

AVampsDuelCharacter::AVampsDuelCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::Disabled;
	GetCapsuleComponent()->InitCapsuleSize(34.f, 88.f);
	GetMesh()->SetRelativeLocation(FVector(0, 0, -88));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = 420.f;

	AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
	AbilitySystem->SetIsReplicated(true);
	Attributes = CreateDefaultSubobject<UVampsAttributeSet>(TEXT("Attributes"));

	FistHitbox = CreateDefaultSubobject<UFistHitboxComponent>(TEXT("FistHitbox"));
	FistHitbox->SetupAttachment(GetMesh(), FistHitbox->SocketName);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 550;
	CameraBoom->TargetOffset = FVector(0, 0, 65);
	CameraBoom->bUsePawnControlRotation = true;
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
}

UAbilitySystemComponent* AVampsDuelCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

void AVampsDuelCharacter::BeginPlay()
{
	Super::BeginPlay();
	AbilitySystem->InitAbilityActorInfo(this, this);

	if (HasAuthority())
	{
		AbilitySystem->AddAttributeSetSubobject(Attributes.Get());
		AttackHandle = AbilitySystem->GiveAbility(FGameplayAbilitySpec(UGA_Attack::StaticClass(), 1));
		GuardHandle = AbilitySystem->GiveAbility(FGameplayAbilitySpec(UGA_Guard::StaticClass(), 1));
		// The fist socket must follow the pose even when the mesh is off screen.
		GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
		GetMesh()->bEnableUpdateRateOptimizations = false;
	}
}

void AVampsDuelCharacter::EndPlay(const EEndPlayReason::Type Reason)
{
	AbilitySystem->CancelAllAbilities();
	FistHitbox->StopTracing();
	Super::EndPlay(Reason);
}

void AVampsDuelCharacter::RequestAttack()
{
	if (HasAuthority())
	{
		AbilitySystem->TryActivateAbility(AttackHandle);
	}
}

void AVampsDuelCharacter::RequestGuard(bool bPressed)
{
	if (!HasAuthority())
	{
		return;
	}

	if (bPressed)
	{
		AbilitySystem->TryActivateAbility(GuardHandle);
	}
	else
	{
		AbilitySystem->CancelAbilityHandle(GuardHandle);
	}
}

void AVampsDuelCharacter::CancelAttack()
{
	AbilitySystem->CancelAbilityHandle(AttackHandle);
}

bool AVampsDuelCharacter::IsAttacking() const
{
	return AbilitySystem->HasMatchingGameplayTag(DuelTags::Attack);
}

bool AVampsDuelCharacter::IsGuarding() const
{
	return AbilitySystem->HasMatchingGameplayTag(DuelTags::Guard);
}

void AVampsDuelCharacter::ReceiveHit(AVampsDuelCharacter* Attacker)
{
	if (!HasAuthority() || !Attacker || Attacker == this)
	{
		return;
	}

	if (IsGuarding())
	{
		Attacker->CancelAttack();
		return;
	}

	FGameplayEffectContextHandle Context = Attacker->AbilitySystem->MakeEffectContext();
	Context.AddSourceObject(Attacker);
	FGameplayEffectSpecHandle Damage =
	    Attacker->AbilitySystem->MakeOutgoingSpec(UGE_DuelDamage::StaticClass(), 1, Context);
	if (Damage.IsValid())
	{
		Damage.Data->SetSetByCallerMagnitude(DuelTags::Damage, -FMath::Max(0.f, Attacker->AttackDamage));
		Attacker->AbilitySystem->ApplyGameplayEffectSpecToTarget(*Damage.Data, AbilitySystem);
	}
}

float AVampsDuelCharacter::GetHealth() const
{
	return Attributes->GetHealth();
}

float AVampsDuelCharacter::GetMaxHealth() const
{
	return Attributes->GetMaxHealth();
}

void AVampsDuelCharacter::ResetFighter(const FTransform& SpawnTransform)
{
	AbilitySystem->CancelAllAbilities();
	FistHitbox->StopTracing();
	AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetHealthAttribute(), GetMaxHealth());
	SetActorTransform(SpawnTransform, false, nullptr, ETeleportType::TeleportPhysics);
	GetCharacterMovement()->StopMovementImmediately();
}

void AVampsDuelCharacter::RequestMove(FVector WorldDirection)
{
	WorldDirection.Z = 0;
	AddMovementInput(WorldDirection.GetClampedToMaxSize(1.f));
}

void AVampsDuelCharacter::SetTarget(AVampsDuelCharacter* NewTarget)
{
	Target = NewTarget != this ? NewTarget : nullptr;
}

AVampsDuelCharacter* AVampsDuelCharacter::GetTarget() const
{
	return Target.Get();
}

void AVampsDuelCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	AVampsDuelCharacter* CurrentTarget = GetTarget();
	GetCharacterMovement()->bOrientRotationToMovement = CurrentTarget == nullptr;
	if (CurrentTarget)
	{
		const FVector Direction = CurrentTarget->GetActorLocation() - GetActorLocation();
		SetActorRotation(FRotator(0, Direction.Rotation().Yaw, 0));
	}
}
