#pragma once

#include "GameFramework/HUD.h"
#include "VampsCombatTestHUD.generated.h"

class AVampsCharacter;
class AVampsMatchGameMode;

UCLASS()
class VAMPS_API AVampsCombatTestHUD : public AHUD
{
	GENERATED_BODY()

public:

	virtual void DrawHUD() override;
	virtual void NotifyHitBoxClick(FName BoxName) override;

#if !UE_BUILD_SHIPPING
	void ToggleCombatHUD();
#endif

private:

	bool bShowCombatDebug = false;
	void DrawMatchHUD(const AVampsMatchGameMode* MatchMode, float ViewScale);

	void DrawCombatHUD(AVampsCharacter* ViewingFighter, float ViewScale);
	void DrawCombatPanel(
	    AVampsCharacter* Fighter, const FString& PanelTitle, float PanelX, float PanelY, float ViewScale);
	void DrawCombatMeter(const TCHAR* MeterLabel,
	                     float MeterValue,
	                     float MeterCapacity,
	                     FLinearColor MeterColor,
	                     float MeterX,
	                     float MeterY,
	                     float ViewScale);
	void DrawCombatEnemies(AVampsCharacter* ViewingFighter, float ListX, float ListY, float ViewScale);
	void DrawFighter(AVampsCharacter* Fighter, const FString& Label, float X, float Y, float Width, bool bSelected);
};
