#pragma once

#include "Blueprint/UserWidget.h"
#include "VampsCharacterIndicatorWidget.generated.h"

class AVampsCharacter;
class UProgressBar;

UCLASS(Abstract)
class VAMPS_API UVampsCharacterIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void ObserveFighter(AVampsCharacter* Fighter);

protected:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> LockOnMarker;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> NAView;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> NAProgress;

	TWeakObjectPtr<AVampsCharacter> ObservedFighter;
};
