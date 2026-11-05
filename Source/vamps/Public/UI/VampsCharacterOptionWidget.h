#pragma once

#include "Blueprint/UserWidget.h"
#include "VampsCharacterOptionWidget.generated.h"

class AVampsMatchGameMode;
class UButton;
class UTextBlock;
class UVampsCombatSettings;

UCLASS(Abstract)
class VAMPS_API UVampsCharacterOptionWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	bool ConfigureCharacter(UVampsCombatSettings* CharacterSettings, AVampsMatchGameMode* CharacterMatchMode);
	void RefreshSelection(const UVampsCombatSettings* SelectedCharacterSettings);

protected:

	virtual void NativeOnInitialized() override;

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CharacterButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CharacterLabel;

	UPROPERTY()
	TObjectPtr<UVampsCombatSettings> CharacterData;
	TWeakObjectPtr<AVampsMatchGameMode> MatchMode;

	UFUNCTION()
	void ChooseCharacter();
};
