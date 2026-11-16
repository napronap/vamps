#pragma once

#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Delegates/Delegate.h"
#include "GameplayAbilitySpecHandle.h"
#include "Combat/VampsCombatState.h"
#include "TimerManager.h"
#include "VampsCharacter.generated.h"

class UAbilitySystemComponent;
class UVampsAttributeSet;
class AVampsWeapon;
class UVampsLockOnComponent;
class UVampsCombatSettings;
class UWidgetComponent;
class UAnimMontage;
class UAnimInstance;
struct FBranchingPointNotifyPayload;
struct FOnAttributeChangeData;
struct FVampsCombatResolution;
struct FVampsPendingContact;

UCLASS(Abstract)
class VAMPS_API AVampsCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnCombatFramingRequested, AVampsCharacter*);
	DECLARE_MULTICAST_DELEGATE(FOnCombatAliveChanged);
	DECLARE_MULTICAST_DELEGATE(FOnCombatReset);

	AVampsCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
	{
		return AbilitySystem.Get();
	}
	virtual void AddMovementInput(FVector WorldDirection, float ScaleValue = 1.f, bool bForce = false) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAbilitySystemComponent> AbilitySystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UVampsAttributeSet> Attributes;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Transient, Category = "Combat")
	TArray<TObjectPtr<AVampsWeapon>> Weapons;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lock On")
	TObjectPtr<UVampsLockOnComponent> LockOn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> CharacterIndicator;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UVampsCombatSettings> CombatSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float StartingMaxHealth = 100.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
	FVampsCombatState CombatState;

	// broadcasted on attack start and from an incoming attacker on a confirmed block
	FOnCombatFramingRequested OnCombatFramingRequested;
	FOnCombatAliveChanged OnCombatAliveChanged;
	FOnCombatReset OnCombatReset;

	// lockon, AI and HUD can track an enemy even while spawn protection prevents hits
	bool CanEngageEnemy(const AVampsCharacter* OtherFighter) const;
	bool CanHitCombatTarget(const AVampsCharacter* OtherFighter) const;

	// The weapon calls this after confirming an unobstructed hit on a valid target.
	void QueueWeaponContact(AVampsCharacter& Recipient, const FVector& ImpactLocation);

	// the match mode stops the round. Both game modes can reset a fighter for reuse
	void StopMatchCombat();
	void ResetCombatState();

	// player input and AI request an attack, defense or combo continuation here
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void RequestCombatAction();
	void RequestCombatDefense();
	void CancelCombatAction();
	bool CanRequestCombatContinuation() const;

	// player input supplies movement intent and the shared dash/sprint button state
	void SetMobilityInput(bool bHasInput);
	void SetMobilityHeld(bool bHeld, const FVector& DashDirection = FVector::ZeroVector);

	// consumed by HUD
	float GetCombatReactionTimeRemaining() const;
	float GetNARecoveryTimeRemaining() const;

	// GA_Attack registers its playback and cleans up any attack that ends early
	bool BeginCombatAction(int32 PlaybackInstanceID);
	void CancelCombatOffense(bool bCancelAbility = true);

	// montage callbacks are scoped to the current playback instance and combo section
	void EndCombatRecovery(const FBranchingPointNotifyPayload& Payload);
	void SetCombatContacts(const FBranchingPointNotifyPayload& Payload, bool bEnabled);
	void ResetCombatWeaponHits(const FBranchingPointNotifyPayload& Payload, const TArray<int32>& WeaponIndices);
	void SetCombatComboWindow(const FBranchingPointNotifyPayload& Payload, bool bEnabled);

	void ConfirmCombatDefense(const FVampsPendingContact& Contact);
	void CreditCombatContact(const FVampsPendingContact& Contact);
	void ReactToCombatResolution(const FVampsCombatResolution& Resolution);

private:

	// TODO: give GA_Dash and GA_Defend a public entry point for movement state
	friend class UGA_Dash;
	friend class UGA_Defend;

	FGameplayAbilitySpecHandle AttackHandle;
	FGameplayAbilitySpecHandle DashHandle;
	FGameplayAbilitySpecHandle DefendHandle;
	FTimerHandle NARecoveryTimer;
	FTimerHandle CombatReactionTimer;
	FTimerHandle HitstopTimer;
	double GuardRecoveryStartsAt = 0;

	int32 ReactionPlaybackInstanceID = INDEX_NONE;
	bool bHasMobilityInput = false;
	bool bMobilityHeld = false;
	double MobilityRecoveryStartsAt = 0;
	FVector RequestedDashDirection = FVector::ZeroVector;

	void StartRequestedDash();
	bool CanStartCombatDash() const;
	void UpdateMobilityEnergy(float DeltaSeconds);
	void ClearMobilityIntent();

	bool InitializeCombatWeapons();
	void DestroyCombatWeapons();
	void SetCombatWeaponTracing(bool bEnabled);

	void BeginCombatStep();
	void ClearAttackPlayback();
	bool IsCurrentCombatNotify(const FBranchingPointNotifyPayload& Payload) const;

	void TryContinueCombatCombo(int32 RequestedActionNumber);
	void FinishAttackRecovery(int32 RecoveryActionNumber);
	void FinishNARecovery();
	void SetCombatPhase(EVampsActionPhase NewPhase);
	bool IsMatchCombatAllowed() const;
	void RefreshCombatPermissions();

	void ClearCombatTimers();

	void ReceiveCombatHit(const FVampsPendingContact& Contact, AVampsCharacter& Attacker);
	void PresentCombatImpact(const FVampsPendingContact& Contact, AVampsCharacter& Attacker);
	void BeginCombatHitstop(float Duration);
	void EndCombatHitstop();
	void ApplyCombatReaction(const FVampsPendingContact& Contact);
	bool PlayCombatReactionMontage(UAnimInstance* Animation, UAnimMontage* ReactionMontage, float Duration);
	void HandleGuardMontageEnded(UAnimMontage* Montage, bool bInterrupted, int32 PlaybackInstanceID);
	void FinishCombatReaction();
	void StopCombatReactionMontage();
	void HandleCombatHealthChanged(const FOnAttributeChangeData& AttributeChange);
	void PresentCombatDeath(bool bBlockedDeath);
};
