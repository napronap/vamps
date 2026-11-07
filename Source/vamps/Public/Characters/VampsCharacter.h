#pragma once

#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
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

	bool CanEngageEnemy(const AVampsCharacter* OtherFighter) const;
	bool CanHitCombatTarget(const AVampsCharacter* OtherFighter) const;
	void StopMatchCombat();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void RequestCombatAction();

	void SetMobilityInput(bool bHasInput);
	void SetMobilityHeld(bool bHeld, const FVector& DashDirection = FVector::ZeroVector);
	void ResolveCombatDash();

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
	FVampsCombatState CombatState;

	uint32 CombatGeneration = 0;

	bool BeginCombatAction(int32 PlaybackInstanceID);
	void FinishCombatAction();
	void EndCombatRecovery(const FBranchingPointNotifyPayload& Payload);
	void ResolveCombatRecoveryEnd(int32 RecoveryActionNumber);
	void SetCombatContacts(const FBranchingPointNotifyPayload& Payload, bool bEnabled);
	void ResetCombatWeaponHits(const FBranchingPointNotifyPayload& Payload, const TArray<int32>& WeaponIndices);
	void SetCombatComboWindow(const FBranchingPointNotifyPayload& Payload, bool bEnabled);
	void ResolveCombatContinuation();
	bool MatchesDefendedContact(const FVampsPendingContact& Contact) const;
	void RememberDefendedContact(const FVampsPendingContact& Contact);
	void RegisterCombatContact(bool bBlocked);
	bool ReceiveCombatHit(const FVampsPendingContact& Contact);
	void ApplyCombatReaction(const FVampsPendingContact& Contact);
	void ResetCombatState();

private:

	friend class UGA_Dash;

	FGameplayAbilitySpecHandle AttackHandle;
	FGameplayAbilitySpecHandle DashHandle;
	FTimerHandle NARecoveryTimer;
	double GuardRecoveryStartsAt = 0;
	uint32 DefendedGeneration = 0;
	int32 ReactionPlaybackInstanceID = INDEX_NONE;
	bool bHasMobilityInput = false;
	bool bMobilityHeld = false;
	double MobilityRecoveryStartsAt = 0;
	int32 DashRequestActionNumber = INDEX_NONE;
	FVector RequestedDashDirection = FVector::ZeroVector;

	void UpdateMobilityEnergy(float DeltaSeconds);
	void ClearMobilityIntent();

	bool InitializeCombatWeapons();
	void DestroyCombatWeapons();
	bool BeginCombatStep();
	bool IsCurrentCombatNotify(const FBranchingPointNotifyPayload& Payload) const;
	void FinishNARecovery();
	void BeginNARecovery();
	void CancelCombatOffense();
	void FinishCombatReaction();
	void ArmDefenseRequest();
	void SetCombatPhase(EVampsActionPhase NewPhase);
	void HandleCombatHealthChanged(const FOnAttributeChangeData& AttributeChange);
	void PresentCombatDeath(bool bBlockedDeath);
};
