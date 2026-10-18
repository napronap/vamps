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
struct FBranchingPointNotifyPayload;
struct FOnAttributeChangeData;

UCLASS(Abstract)
class VAMPS_API AVampsCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	AVampsCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UVampsCombatSettings> CombatSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "0"))
	float AttackDamage = 12.f;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void RequestCombatAction();

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
	FVampsCombatState CombatState;

	bool BeginCombatAction(int32 PlaybackInstanceID);
	void FinishCombatAction();
	void SetCombatContacts(const FBranchingPointNotifyPayload& Payload, bool bEnabled);
	void ResetCombatWeaponHits(const FBranchingPointNotifyPayload& Payload, const TArray<int32>& WeaponIndices);
	void SetCombatComboWindow(const FBranchingPointNotifyPayload& Payload, bool bEnabled);
	void ResolveCombatContinuation();
	void ResolveCombatLocomotion();
	bool TryCalculateCombatSectionDuration(float& SectionDuration) const;
	void ApplyOffensiveRestriction(float RestrictionDuration);
	void ConsumeDefenseRequest();
	void RegisterCombatContact(bool bBlocked);
	void ReceiveCombatHit(AVampsCharacter* Attacker, bool bBlocked);
	void ResetCombatState();

private:

	FGameplayAbilitySpecHandle AttackHandle;
	FTimerHandle NARecoveryTimer;
	FTimerHandle ActionRecoveryTimer;
	FTimerHandle OffensiveRestrictionTimer;
	double CombatStepEndsAt = 0;

	bool InitializeCombatWeapons();
	void DestroyCombatWeapons();
	bool BeginCombatStep();
	bool IsCurrentCombatNotify(const FBranchingPointNotifyPayload& Payload) const;
	void FinishNARecovery();
	void BeginNARecovery();
	void FinishOffensiveRestriction();
	void SetCombatPhase(EVampsActionPhase NewPhase);
	void HandleCombatHealthChanged(const FOnAttributeChangeData& AttributeChange);
};
