#pragma once

#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpecHandle.h"
#include "Combat/VampsCombatState.h"
#include "VampsCharacter.generated.h"

class UAbilitySystemComponent;
class UVampsAttributeSet;
class UFistHitboxComponent;
class UVampsLockOnComponent;
class UAnimMontage;
struct FBranchingPointNotifyPayload;
struct FOnAttributeChangeData;

UCLASS(Abstract)
class VAMPS_API AVampsCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	AVampsCharacter();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAbilitySystemComponent> AbilitySystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UVampsAttributeSet> Attributes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UFistHitboxComponent> FistHitbox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lock On")
	TObjectPtr<UVampsLockOnComponent> LockOn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "0"))
	float AttackDamage = 12.f;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void RequestCombatAction();

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
	FVampsCombatState CombatState;

	void BeginCombatAction(int32 PlaybackInstanceID);
	void FinishCombatAction();
	void SetCombatContacts(const FBranchingPointNotifyPayload& Payload, bool bEnabled);
	void ConsumeDefenseRequest();
	void RegisterCombatContact(bool bBlocked);
	void ReceiveCombatHit(AVampsCharacter* Attacker, bool bBlocked);
	void ResetCombatState();

private:

	FGameplayAbilitySpecHandle AttackHandle;

	void SetCombatPhase(EVampsActionPhase NewPhase);
	void HandleCombatHealthChanged(const FOnAttributeChangeData& AttributeChange);
};
