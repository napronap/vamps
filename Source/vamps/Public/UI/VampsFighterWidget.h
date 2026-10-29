#pragma once

#include "Blueprint/UserWidget.h"
#include "VampsFighterWidget.generated.h"

class AVampsCharacter;
class UAbilitySystemComponent;
class UProgressBar;
class UTextBlock;
struct FOnAttributeChangeData;

UCLASS(Abstract)
class VAMPS_API UVampsFighterWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void ObserveFighter(AVampsCharacter* NewFighter,
	                    const FString& PanelLabel,
	                    bool bDebug = false,
	                    bool bSelected = false);

protected:

	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> CombatView;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> DebugView;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PanelTitle;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HealthText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> GuardText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EnergyText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> GuardBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> EnergyBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> StatusText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ContactText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DebugTitle;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DebugDetails;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DebugContact;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> DebugHealthBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> DebugGuardBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> SelectionMark;

	TWeakObjectPtr<AVampsCharacter> ObservedFighter;
	TWeakObjectPtr<UAbilitySystemComponent> ObservedAbilitySystem;
	FString FighterLabel;
	bool bDebugView = false;
	TArray<FDelegateHandle> AttributeHandles;

	void ReleaseAttributes();
	void RefreshMeters();
	void HandleAttributeChanged(const FOnAttributeChangeData& AttributeChange);
	void RefreshDebugDetails();
};
