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
struct FBranchingPointNotifyPayload;
struct FOnAttributeChangeData;
struct FVampsPendingContact;

UCLASS(Abstract)
class VAMPS_API AVampsCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnCombatFramingRequested, AVampsCharacter*);

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "1"))
	float StartingMaxHealth = 100.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
	FVampsCombatState CombatState;

	// TODO: maybe try to come up with a better way to do this to stop using magic numbers
	// CombatGeneration tracks the "version" of the current attack so that the combat resolver knows what to do when it requests any action
	// it goes up any time the state is reset (ResetCombatState) or teardown so contacts, defense and camera can discard stale combat state
	// (i.e number is different from the previous requested action)
	uint32 CombatGeneration = 0;

	// broadcasted on attack start and from an incoming attacker on a confirmed block
	FOnCombatFramingRequested OnCombatFramingRequested;

	// lockon, AI and HUD can track an enemy even while spawn protection prevents hits
	bool CanEngageEnemy(const AVampsCharacter* OtherFighter) const;
	bool CanHitCombatTarget(const AVampsCharacter* OtherFighter) const;

	// the match mode stops the round. Both game modes and the AI can reset a fighter for reuse
	void StopMatchCombat();
	void ResetCombatState();

	// player input and AI request an attack, defense or combo continuation here
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void RequestCombatAction();
	bool CanRequestCombatContinuation() const;

	// player input supplies movement intent and the shared dash/sprint button state
	void SetMobilityInput(bool bHasInput);
	void SetMobilityHeld(bool bHeld, const FVector& DashDirection = FVector::ZeroVector);

	// consumed by HUD
	bool CanStartCombatDash() const;
	float GetCombatReactionTimeRemaining() const;
	float GetNARecoveryTimeRemaining() const;

	// consumed by GA_Attack, registers the montage playback and closes the action when the ability ends
	bool BeginCombatAction(int32 PlaybackInstanceID);
	void FinishCombatAction();

	// montage callbacks are scoped to the current playback instance and combo section
	void EndCombatRecovery(const FBranchingPointNotifyPayload& Payload);
	void SetCombatContacts(const FBranchingPointNotifyPayload& Payload, bool bEnabled);
	void ResetCombatWeaponHits(const FBranchingPointNotifyPayload& Payload, const TArray<int32>& WeaponIndices);
	void SetCombatComboWindow(const FBranchingPointNotifyPayload& Payload, bool bEnabled);

private:

	friend class UGA_Dash;
	friend class UVampsCombatResolver;

	FGameplayAbilitySpecHandle AttackHandle;
	FGameplayAbilitySpecHandle DashHandle;
	FTimerHandle NARecoveryTimer;
	FTimerHandle CombatReactionTimer;
	FTimerHandle DefenseRequestTimer;
	double GuardRecoveryStartsAt = 0;
	uint32 DefendedGeneration = 0;
	int32 ReactionPlaybackInstanceID = INDEX_NONE;
	bool bHasMobilityInput = false;
	bool bMobilityHeld = false;
	double MobilityRecoveryStartsAt = 0;
	FVector RequestedDashDirection = FVector::ZeroVector;

	// combat resolver consumes dash intent after impacts, which may cancel it before energy is spent
	void ResolveCombatDash();
	void UpdateMobilityEnergy(float DeltaSeconds);
	void ClearMobilityIntent();

	bool InitializeCombatWeapons();
	void DestroyCombatWeapons();
	void SetCombatWeaponTracing(bool bEnabled);

	void BeginCombatStep();
	bool IsCurrentCombatNotify(const FBranchingPointNotifyPayload& Payload) const;

	// combat resolver tries a queued combo before ending Recovery for the same action number
	void ResolveCombatContinuation();
	void ResolveCombatRecoveryEnd(int32 RecoveryActionNumber);
	void BeginNARecovery();
	void FinishNARecovery();
	void CancelCombatOffense();
	void SetCombatPhase(EVampsActionPhase NewPhase);
	bool IsMatchCombatAllowed() const;
	void RefreshCombatPermissions();

	void ClearCombatTimers();
	void ArmDefenseRequest();
	void ClearDefenseRequest();
	void ExpireDefenseRequest();
	bool MatchesDefendedContact(const FVampsPendingContact& Contact) const;
	void RememberDefendedContact(const FVampsPendingContact& Contact);

	// combat resolver passes validated contacts here and selects one reaction per recipient
	void RegisterCombatContact(bool bBlocked);
	bool ReceiveCombatHit(const FVampsPendingContact& Contact, AVampsCharacter& Attacker);
	void ApplyCombatReaction(const FVampsPendingContact& Contact);
	void FinishCombatReaction();
	void StopCombatReactionMontage();
	void HandleCombatHealthChanged(const FOnAttributeChangeData& AttributeChange);
	void PresentCombatDeath(bool bBlockedDeath);
};
