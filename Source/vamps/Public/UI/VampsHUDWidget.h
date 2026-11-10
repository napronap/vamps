#pragma once

#include "Blueprint/UserWidget.h"
#include "VampsHUDWidget.generated.h"

class APawn;
class AVampsCharacter;
class AVampsMatchGameMode;
class UButton;
class UCanvasPanel;
class UScaleBox;
class UTextBlock;
class UVerticalBox;
class UVampsCharacterOptionWidget;
class UVampsCombatSettings;
class UVampsEnemyWidget;
class UVampsFighterWidget;
class UVampsLockOnComponent;

UCLASS(Abstract)
class VAMPS_API UVampsHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void ToggleCombatDebug();

protected:

	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UVampsEnemyWidget> EnemyWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UVampsFighterWidget> FighterWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UVampsCharacterOptionWidget> CharacterOptionWidgetClass;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScaleBox> LayoutScale;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScaleBox> DebugScale;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> SelectionView;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> CombatView;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> DebugView;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> MatchView;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> ResultView;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> CharacterOptions;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StartButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RestartButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ScoreText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NoticeText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TeamText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RespawnText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ProtectionText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ResultText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ParticipantText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TeamResultsText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EnemyHeading;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NoTargetText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DebugHint;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TestControlsText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVampsFighterWidget> PlayerPanel;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVampsFighterWidget> TargetPanel;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVampsFighterWidget> DebugPlayerPanel;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> EnemyIndicators;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> DebugOpponents;

	TWeakObjectPtr<AVampsMatchGameMode> MatchMode;
	TWeakObjectPtr<UVampsLockOnComponent> ObservedLockOn;
	bool bHUDInitialized = false;
	bool bShowCombatDebug = false;

	static void ShowWidget(UWidget* Widget, bool bShow);
	static FString FormatRatio(int32 Kills, int32 Deaths);
	void RefreshMatchViews();
	void RefreshCharacterSelection(UVampsCombatSettings* CharacterSettings);
	void RefreshTargetPanel(AVampsCharacter* TargetFighter);
	void RefreshMatch();
	void RefreshFighters(float LayoutHeight);
	void RefreshResults();

	UFUNCTION()
	void HandleObservedPawnChanged(APawn* OldPawn, APawn* NewPawn);
	UFUNCTION()
	void StartRound();
	UFUNCTION()
	void ReturnToSelection();
};
