#pragma once

#include "GameFramework/HUD.h"
#include "VampsCombatTestHUD.generated.h"

class AVampsCharacter;

UCLASS()
class VAMPS_API AVampsCombatTestHUD : public AHUD
{
	GENERATED_BODY()

public:

	virtual void DrawHUD() override;

private:

	void DrawFighter(AVampsCharacter* Fighter, const FString& Label, float X, float Y, float Width);
};
