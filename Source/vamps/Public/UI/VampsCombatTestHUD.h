#pragma once

#include "GameFramework/HUD.h"
#include "VampsCombatTestHUD.generated.h"

class UVampsHUDWidget;

UCLASS()
class VAMPS_API AVampsCombatTestHUD : public AHUD
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;

private:

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UVampsHUDWidget> HUDWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UVampsHUDWidget> HUDWidget;
};
