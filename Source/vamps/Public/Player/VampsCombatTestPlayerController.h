#pragma once

#include "Player/VampsPlayerController.h"
#include "VampsCombatTestPlayerController.generated.h"

UCLASS()
class VAMPS_API AVampsCombatTestPlayerController : public AVampsPlayerController
{
	GENERATED_BODY()

protected:

	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, Category = "Combat Test")
	TObjectPtr<UInputAction> CycleAIModeAction;

	UPROPERTY(EditDefaultsOnly, Category = "Combat Test")
	TObjectPtr<UInputAction> ResetCombatAction;

private:

	void CycleAIMode();
	void ResetCombat();

#if !UE_BUILD_SHIPPING
	void ToggleLockOnDebug(FKey Key, FInputActionValue Value);
	void ToggleCombatHUD(FKey Key, FInputActionValue Value);
#endif
};
