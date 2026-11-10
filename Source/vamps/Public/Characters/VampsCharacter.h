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
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
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

	// A reset invalidates contacts captured for this fighter before the reset.
	uint32 CombatGeneration = 0;

	// The source is this fighter on attack start and the attacker on a confirmed block.
	FOnCombatFramingRequested OnCombatFramingRequested;

	bool CanEngageEnemy(const AVampsCharacter* OtherFighter) const;
	bool CanHitCombatTarget(const AVampsCharacter* OtherFighter) const;
	void StopMatchCombat();
	void ResetCombatState();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void RequestCombatAction();
	bool CanRequestCombatContinuation() const;

	void SetMobilityInput(bool bHasInput);
	void SetMobilityHeld(bool bHeld, const FVector& DashDirection = FVector::ZeroVector);

	bool BeginCombatAction(int32 PlaybackInstanceID);
	void FinishCombatAction();

	// Montage callbacks are scoped to the current playback instance and combo section.
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
	double GuardRecoveryStartsAt = 0;
	uint32 DefendedGeneration = 0;
	int32 ReactionPlaybackInstanceID = INDEX_NONE;
	bool bHasMobilityInput = false;
	bool bMobilityHeld = false;
	double MobilityRecoveryStartsAt = 0;
	FVector RequestedDashDirection = FVector::ZeroVector;

	void ResolveCombatDash();
	void UpdateMobilityEnergy(float DeltaSeconds);
	void ClearMobilityIntent();

	bool InitializeCombatWeapons();
	void DestroyCombatWeapons();
	void SetCombatWeaponTracing(bool bEnabled);

	bool BeginCombatStep();
	bool IsCurrentCombatNotify(const FBranchingPointNotifyPayload& Payload) const;
	void ResolveCombatContinuation();
	void ResolveCombatRecoveryEnd(int32 RecoveryActionNumber);
	void BeginNARecovery();
	void FinishNARecovery();
	void CancelCombatOffense();
	void SetCombatPhase(EVampsActionPhase NewPhase);
	void RefreshCombatPermissions();

	void ArmDefenseRequest();
	bool MatchesDefendedContact(const FVampsPendingContact& Contact) const;
	void RememberDefendedContact(const FVampsPendingContact& Contact);
	void RegisterCombatContact(bool bBlocked);
	bool ReceiveCombatHit(const FVampsPendingContact& Contact);
	void ApplyCombatReaction(const FVampsPendingContact& Contact);
	void FinishCombatReaction();
	void HandleCombatHealthChanged(const FOnAttributeChangeData& AttributeChange);
	void PresentCombatDeath(bool bBlockedDeath);
};
