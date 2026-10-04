#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Combat/Data/CombatConfig.h"
#include "Combat/CombatRules.h"
#include "VampsDuelCharacter.generated.h"

class UAbilitySystemComponent;
class UVampsAttributeSet;
class UGA_Attack;
class UGA_Guard;
class UGA_NA;
class UGA_HitReaction;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class VAMPS_API AVampsDuelCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	AVampsDuelCharacter();

	// Actor lifecycle and ability system access.
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Shared configuration and native components.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Duel")
	TObjectPtr<UCombatConfig> CombatConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Duel")
	TObjectPtr<UAbilitySystemComponent> AbilitySystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Duel")
	TObjectPtr<UVampsAttributeSet> Attributes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;

	// Results exposed to presentation and diagnostics.
	UPROPERTY(BlueprintReadOnly, Category = "Duel")
	EDuelResult LastResult = EDuelResult::None;

	UPROPERTY(BlueprintReadOnly, Category = "Duel")
	FString LastEvent;

	UPROPERTY(BlueprintReadOnly, Category = "Duel")
	int32 ContactsResolved = 0;

	// Instanced abilities own the combat state.
	UPROPERTY()
	TObjectPtr<UGA_Attack> Attack;

	UPROPERTY()
	TObjectPtr<UGA_Guard> Guard;

	UPROPERTY()
	TObjectPtr<UGA_NA> NA;

	UPROPERTY()
	TObjectPtr<UGA_HitReaction> HitReaction;

	// Initialization and access to the common combat clock.
	void InitializeCombat();
	const UCombatConfig& Config() const;
	double CombatTime() const;

	// Requests shared by player input and AI.
	UFUNCTION(BlueprintCallable, Category = "Duel")
	void PressShared();

	UFUNCTION(BlueprintCallable, Category = "Duel")
	void SetMoveIntent(FVector WorldDirection);

	UFUNCTION(BlueprintCallable, Category = "Duel")
	void SetTarget(AVampsDuelCharacter* NewTarget);

	// Read-only state for Blueprints, animation and HUD.
	UFUNCTION(BlueprintPure, Category = "Duel")
	AVampsDuelCharacter* GetTarget() const;

	UFUNCTION(BlueprintPure, Category = "Duel")
	bool IsDefeated() const;

	UFUNCTION(BlueprintPure, Category = "Duel")
	EDuelPhase GetPhase() const;

	UFUNCTION(BlueprintPure, Category = "Duel")
	float GetHealth() const;

	UFUNCTION(BlueprintPure, Category = "Duel")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintPure, Category = "Duel")
	float GetNARemaining() const;

	UFUNCTION(BlueprintPure, Category = "Duel")
	float GetNAPause() const;

	// Action transitions and ordered contact resolution.
	void ResetFighter(const FTransform& SpawnTransform);
	void AdvanceActions(double Now);
	void ConsumeInput(double Now);

	void ClearInput()
	{
		Input.Clear();
	}

	void StartNA(double Now, bool bOffensive);
	void StartGuard(double Now, bool bAlreadyBlocked);
	EDuelResult ReceiveContact(AVampsDuelCharacter* Attacker, const FDuelStrike& Strike, double Now);

	// Movement availability is independent of visual recovery.
	bool CanMove(double Now) const;
	void MoveSwept(FVector Delta);

	// Presentation follows gameplay decisions.
	void PlayAttackSection(const FDuelStrike& Strike, FName Section, double Duration);
	void PlayReaction(bool bGuard);
	void StopCombatAnimation();
	void DrawCombat() const;

private:

	FDuelInputBuffer Input;
	TWeakObjectPtr<AVampsDuelCharacter> Target;
	FVector MoveIntent = FVector::ZeroVector;
	bool bInitialized = false;

	bool TryInput(EDuelInputIntent Intent, double Now);
	void ResetOnParry();
};
