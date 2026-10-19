#pragma once

#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "VampsPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;

UCLASS()
class VAMPS_API AVampsPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AVampsPlayerController();

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultInputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> CombatAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> CycleTargetAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MobilityAction;

private:

	void Move(const FInputActionValue& Value);
	void RequestCombat();
	void CycleTarget();
	void MobilityPressed();
	void MobilityReleased();
};
