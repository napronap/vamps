#pragma once

#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpecHandle.h"
#include "VampsDuelCharacter.generated.h"

class UAbilitySystemComponent;
class UVampsAttributeSet;
class UFistHitboxComponent;
class USpringArmComponent;
class UCameraComponent;
class UAnimMontage;

UCLASS()
class VAMPS_API AVampsDuelCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	AVampsDuelCharacter();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAbilitySystemComponent> AbilitySystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UVampsAttributeSet> Attributes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UFistHitboxComponent> FistHitbox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "0"))
	float AttackDamage = 12.f;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void RequestAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void RequestGuard(bool bPressed);

	void CancelAttack();
	void ReceiveHit(AVampsDuelCharacter* Attacker);
	void ResetFighter(const FTransform& SpawnTransform);

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsAttacking() const;

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsGuarding() const;

	UFUNCTION(BlueprintPure, Category = "Combat")
	float GetHealth() const;

	UFUNCTION(BlueprintPure, Category = "Combat")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void RequestMove(FVector WorldDirection);

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void SetTarget(AVampsDuelCharacter* NewTarget);

	UFUNCTION(BlueprintPure, Category = "Camera")
	AVampsDuelCharacter* GetTarget() const;

private:

	FGameplayAbilitySpecHandle AttackHandle;
	FGameplayAbilitySpecHandle GuardHandle;
	TWeakObjectPtr<AVampsDuelCharacter> Target;
};
