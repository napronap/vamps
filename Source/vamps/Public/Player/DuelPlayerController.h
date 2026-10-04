#pragma once

#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "DuelPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;

UCLASS()
class VAMPS_API ADuelPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	ADuelPlayerController();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void PlayerTick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DuelMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> SharedAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LockAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ResetAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> AIModeAction;

	UFUNCTION(BlueprintCallable, Category = "Duel")
	void ResetDuel();

	UFUNCTION(BlueprintCallable, Category = "Duel")
	void ToggleLock();

	UFUNCTION(BlueprintCallable, Category = "Duel")
	void CycleAI();

private:

	FVector2D MoveAxes = FVector2D::ZeroVector;

	void Move(const FInputActionValue& Value);
	void StopMove(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Shared();
};
