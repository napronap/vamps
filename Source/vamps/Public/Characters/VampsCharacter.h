// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpecHandle.h"
#include "InputActionValue.h"
#include "VampsCharacter.generated.h"

class UAbilitySystemComponent;
class UVampsAttributeSet;
class UFistHitboxComponent;
class UAnimMontage;
class UInputAction;

UCLASS(abstract)
class AVampsCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AVampsCharacter();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndplayReason::Type Reason) override; // CHECK
	virtual void Tick(float DeltaSeconds) override;
	// for now there's no reason to keep the ASC in any other place than the character
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> AbilitySystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UVampsAttributeSet> Attributes;

	// CHECK: probably move this to another AC
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UFistHitboxComponent> FistHitbox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AttackDamage = 12.f;

	UFUNCTION(BlueprintCallable)
	void RequestAttack();

	UFUNCTION(BlueprintCallable)
	void RequestGuard();

	void CancelAttack();
	void ReceiveHit(AVampsCharacter* Attacker);
	void ResetFighter(const FTransform& SpawnTransform);

	UFUNCTION(BlueprintPure)
	bool IsAttacking() const;

	UFUNCTION(BlueprintPure)
	bool IsGuarding() const;

	UFUNCTION(BlueprintPure)
	bool GetHealth() const;

	UFUNCTION(BlueprintPure)
	bool GetMaxHealth() const;

	UFUNCTION(BlueprintCallable)
	bool RequestMove(FVector WorldDirection);

	UFUNCTION(BlueprintCallable)
	void SetTarget(AVampsCharacter* NewTarget);

	UFUNCTION(BlueprintPure)
	AVampsCharacter* GetTarget();


	UFUNCTION(BlueprintCallable, Category="Input")
	void DoMove(const FInputActionValue& Value);

protected:

	/** Input actions */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

	// CHECK
	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	FGameplayAbilitySpecHandle AttackHandle;
	FGameplayAbilitySpecHandle GuardHandle;
	TWeakObjectPtr<AVampsCharacter> Target;
};

