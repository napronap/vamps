#pragma once

#include "GameFramework/HUD.h"
#include "DuelHUD.generated.h"

class AVampsDuelCharacter;

UCLASS()
class VAMPS_API ADuelHUD : public AHUD
{
	GENERATED_BODY()

public:

	virtual void DrawHUD() override;

private:

	void DrawFighter(AVampsDuelCharacter* Fighter, const FString& Label, float X, float Width);
};
