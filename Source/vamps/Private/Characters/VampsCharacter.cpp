#include "Characters/VampsCharacter.h"
#include "AbilitySystemComponent.h"
#include "Combat/Abilities/GA_Attack.h"
#include "Combat/Abilities/GA_Guard.h"
#include "Combat/Attributes/VampsAttributeSet.h"
#include "Combat/CombatTags.h"
#include "Combat/Effects/GE_DuelDamage.h"
#include "Combat/FistHitboxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AVampsCharacter::AVampsCharacter()
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
	FistHitbox = CreateDefaultSubobject<UFistHitboxComponent>(TEXT("FistHitbox"));
	FistHitbox->SetupAttachment(GetMesh(), FistHitbox->SocketName);
}

void AVampsCharacter::BeginPlay()
{
	AbilitySystem->InitAbilityActorInfo(this, this);

	if (HasAuthority())
	{
		AttackHandle = AbilitySystem->GiveAbility(FGameplayAbilitySpec(UGA_Attack::StaticClass(), 1));
		GuardHandle = AbilitySystem->GiveAbility(FGameplayAbilitySpec(UGA_Guard::StaticClass(), 1));
	}
	Super::BeginPlay();
}

void AVampsCharacter::EndPlay(const EEndPlayReason::Type Reason)
{
	AbilitySystem->CancelAllAbilities();
	FistHitbox->StopTracing();
	Super::EndPlay(Reason);
}

void AVampsCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AbilitySystem->InitAbilityActorInfo(this, this);
}

void AVampsCharacter::UnPossessed()
{
	AbilitySystem->CancelAllAbilities();
	FistHitbox->StopTracing();
	Super::UnPossessed();
	AbilitySystem->InitAbilityActorInfo(this, this);
}

UAbilitySystemComponent* AVampsCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

void AVampsCharacter::RequestAttack()
{
	if (HasAuthority())
	{
		AbilitySystem->TryActivateAbility(AttackHandle);
	}
}

void AVampsCharacter::RequestGuard(bool bPressed)
{
	if (!HasAuthority())
	{
		return;
	}

	if (bPressed)
	{
		AbilitySystem->TryActivateAbility(GuardHandle);
		return;
	}
	AbilitySystem->CancelAbilityHandle(GuardHandle);
}

bool AVampsCharacter::IsAttacking() const
{
	return AbilitySystem->HasMatchingGameplayTag(DuelTags::Attack);
}

bool AVampsCharacter::IsGuarding() const
{
	return AbilitySystem->HasMatchingGameplayTag(DuelTags::Guard);
}

void AVampsCharacter::ReceiveHit(AVampsCharacter* Attacker)
{
	check(Attacker && Attacker != this);
	if (!HasAuthority())
	{
		return;
	}

	if (IsGuarding())
	{
		Attacker->AbilitySystem->CancelAbilityHandle(Attacker->AttackHandle);
		return;
	}

	FGameplayEffectContextHandle Context = Attacker->AbilitySystem->MakeEffectContext();
	Context.AddSourceObject(Attacker);
	FGameplayEffectSpecHandle Damage =
	    Attacker->AbilitySystem->MakeOutgoingSpec(UGE_DuelDamage::StaticClass(), 1, Context);
	check(Damage.IsValid());
	Damage.Data->SetSetByCallerMagnitude(DuelTags::Damage, -FMath::Max(0.f, Attacker->AttackDamage));
	Attacker->AbilitySystem->ApplyGameplayEffectSpecToTarget(*Damage.Data, AbilitySystem);
}
