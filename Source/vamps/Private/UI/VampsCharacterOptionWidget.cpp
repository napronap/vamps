#include "UI/VampsCharacterOptionWidget.h"
#include "Combat/Data/VampsCombatSettings.h"
#include "Combat/VampsCombatLog.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameModes/VampsMatchGameMode.h"

void UVampsCharacterOptionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	CharacterButton->OnClicked.AddDynamic(this, &UVampsCharacterOptionWidget::ChooseCharacter);
}

bool UVampsCharacterOptionWidget::ConfigureCharacter(UVampsCombatSettings* CharacterSettings,
                                                     AVampsMatchGameMode* CharacterMatchMode)
{
	if (!CharacterSettings || !CharacterMatchMode)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s' requires character settings and a match GameMode; settings='%s', mode='%s'."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(CharacterSettings),
		       *GetPathNameSafe(CharacterMatchMode));
		return false;
	}
	CharacterData = CharacterSettings;
	MatchMode = CharacterMatchMode;
	CharacterLabel->SetText(CharacterData->CharacterName);
	RefreshSelection(CharacterMatchMode->SelectedCharacter);
	return true;
}

void UVampsCharacterOptionWidget::RefreshSelection(const UVampsCombatSettings* SelectedCharacterSettings)
{
	CharacterButton->SetBackgroundColor(CharacterData.Get() == SelectedCharacterSettings
	                                        ? FLinearColor(0.15f, 0.4f, 0.5f)
	                                        : FLinearColor(0.12f, 0.12f, 0.18f));
}

void UVampsCharacterOptionWidget::ChooseCharacter()
{
	if (!MatchMode.IsValid() || !CharacterData)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s' cannot select a character; settings='%s', mode='%s'."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(CharacterData),
		       *GetPathNameSafe(MatchMode.Get()));
		return;
	}
	MatchMode->SelectCharacter(CharacterData);
}
