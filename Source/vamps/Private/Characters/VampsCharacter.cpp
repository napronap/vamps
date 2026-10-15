#include "Characters/VampsCharacter.h"
#include "Characters/VampsLockOnComponent.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Combat/Abilities/GA_Attack.h"
#include "Combat/Attributes/VampsAttributeSet.h"
#include "Combat/VampsCombatTags.h"
#include "Combat/Effects/GE_VampsDamage.h"
#include "Combat/FistHitboxComponent.h"
#include "Combat/VampsCombatResolver.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
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
	LockOn = CreateDefaultSubobject<UVampsLockOnComponent>(TEXT("LockOn"));
}

void AVampsCharacter::BeginPlay()
{
	AbilitySystem->InitAbilityActorInfo(this, this);

	if (HasAuthority())
	{
		AttackHandle = AbilitySystem->GiveAbility(FGameplayAbilitySpec(UGA_Attack::StaticClass(), 1));
	}
	AbilitySystem->GetGameplayAttributeValueChangeDelegate(UVampsAttributeSet::GetHealthAttribute())
	    .AddUObject(this, &AVampsCharacter::HandleCombatHealthChanged);
	SetCombatPhase(EVampsActionPhase::Idle);
	Super::BeginPlay();
}

void AVampsCharacter::EndPlay(const EEndPlayReason::Type Reason)
{
	AbilitySystem->GetGameplayAttributeValueChangeDelegate(UVampsAttributeSet::GetHealthAttribute()).RemoveAll(this);
	ResetCombatState();
	Super::EndPlay(Reason);
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

void AVampsCharacter::RequestCombatAction()
{
	if (!HasAuthority())
	{
		return;
	}

	++CombatState.RequestCount;
	CombatState.LastRequest = EVampsCombatRequest::Rejected;
	if (!CombatState.bCanRequestCombat)
	{
		return;
	}

	if (CombatState.bCanStartAction)
	{
		if (AbilitySystem->TryActivateAbility(AttackHandle))
		{
			CombatState.LastRequest = EVampsCombatRequest::ActionStarted;
		}
		return;
	}

	CombatState.bCanDefend = true;
	CombatState.LastRequest = EVampsCombatRequest::DefenseRearmed;
}

void AVampsCharacter::BeginCombatAction(int32 PlaybackInstanceID)
{
	check(CombatState.bCanStartAction && PlaybackInstanceID != INDEX_NONE);
	++CombatState.ActionNumber;
	CombatState.MontageInstanceID = PlaybackInstanceID;
	CombatState.bCanDefend = true;
	CombatState.bActionConnected = false;
	CombatState.LastOutgoingContact = EVampsContactResult::None;
	CombatState.LastOutgoingContactFrame = 0;
	FistHitbox->ResetHits();
	SetCombatPhase(EVampsActionPhase::Preparation);
}

void AVampsCharacter::FinishCombatAction()
{
	if (CombatState.Phase != EVampsActionPhase::Idle && !CombatState.bActionConnected)
	{
		CombatState.LastOutgoingContact = EVampsContactResult::Miss;
		CombatState.LastOutgoingContactFrame = GFrameCounter;
	}
	CombatState.MontageInstanceID = INDEX_NONE;
	SetCombatPhase(EVampsActionPhase::Idle);
}

void AVampsCharacter::SetCombatContacts(const FBranchingPointNotifyPayload& Payload, bool bEnabled)
{
	if (!HasAuthority() || Payload.SkelMeshComponent != GetMesh() ||
	    Payload.MontageInstanceID != CombatState.MontageInstanceID)
	{
		return;
	}
	UAnimInstance* Animation = GetMesh()->GetAnimInstance();
	const FAnimMontageInstance* Playback =
	    Animation ? Animation->GetMontageInstanceForID(CombatState.MontageInstanceID) : nullptr;
	if (!Playback || Playback->Montage != AttackMontage)
	{
		return;
	}

	if (bEnabled)
	{
		checkf(CombatState.Phase == EVampsActionPhase::Preparation, TEXT("Combat contacts must follow preparation."));
		SetCombatPhase(EVampsActionPhase::Contacts);
		return;
	}
	checkf(CombatState.Phase == EVampsActionPhase::Contacts, TEXT("Combat recovery must follow contacts."));
	SetCombatPhase(EVampsActionPhase::Recovery);
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
	FinishCombatAction();
	GetWorld()->GetSubsystem<UVampsCombatResolver>()->DiscardContactsFor(this);
	FistHitbox->ResetHits();
	CombatState = FVampsCombatState();
	SetCombatPhase(EVampsActionPhase::Idle);
}

void AVampsCharacter::SetCombatPhase(EVampsActionPhase NewPhase)
{
	const bool bWasDamageEnabled = CombatState.bCanDealDamage;
	CombatState.Phase = NewPhase;
	CombatState.bIsAlive = Attributes->GetHealth() > 0.f;
	CombatState.bIsRecovering = NewPhase == EVampsActionPhase::Recovery;
	CombatState.bCanRequestCombat = CombatState.bIsAlive && !CombatState.bIsRecovering;
	CombatState.bCanStartAction = CombatState.bIsAlive && NewPhase == EVampsActionPhase::Idle;
	CombatState.bCanDealDamage = CombatState.bIsAlive && NewPhase == EVampsActionPhase::Contacts;
	CombatState.bCanDefend = CombatState.bCanDefend && CombatState.bIsAlive &&
	                         (NewPhase == EVampsActionPhase::Preparation || NewPhase == EVampsActionPhase::Contacts);

	if (bWasDamageEnabled == CombatState.bCanDealDamage)
	{
		return;
	}
	if (CombatState.bCanDealDamage)
	{
		FistHitbox->StartTracing();
		return;
	}
	FistHitbox->StopTracing();
}

void AVampsCharacter::HandleCombatHealthChanged(const FOnAttributeChangeData&)
{
	SetCombatPhase(CombatState.Phase);
}

void AVampsCharacter::ReceiveCombatHit(AVampsCharacter* Attacker, bool bBlocked)
{
	check(Attacker && Attacker != this);
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
	check(Damage.IsValid());
	Damage.Data->SetSetByCallerMagnitude(VampsTags::Damage, -FMath::Max(0.f, Attacker->AttackDamage));
	Attacker->AbilitySystem->ApplyGameplayEffectSpecToTarget(*Damage.Data, AbilitySystem);
}
