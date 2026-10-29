#pragma once

#include "Blueprint/UserWidget.h"
#include "VampsEnemyWidget.generated.h"

class AVampsCharacter;
class UBorder;
class UTextBlock;

UCLASS(Abstract)
class VAMPS_API UVampsEnemyWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void UpdateEnemy(const AVampsCharacter* Viewer,
	                 const AVampsCharacter* Enemy,
	                 int32 EnemyIndex,
	                 int32 VisibleRow,
	                 bool bSelected);

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> EnemyRow;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EnemyText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> DirectionArrow;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> ThreatMarker;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> ThreatArrow;
};
