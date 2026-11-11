#pragma once

#include "Blueprint/UserWidget.h"
#include "VampsFighterWidget.generated.h"

class AVampsCharacter;
class UAbilitySystemComponent;
class UProgressBar;
class UTextBlock;
struct FGameplayAttribute;
struct FOnAttributeChangeData;

UCLASS(Abstract)
class VAMPS_API UVampsFighterWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void ObserveFighter(AVampsCharacter* NewFighter);

protected:

	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:

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

	TWeakObjectPtr<AVampsCharacter> ObservedFighter;
	TWeakObjectPtr<UAbilitySystemComponent> ObservedAbilitySystem;
	bool bShowingStun = false;
	TArray<FDelegateHandle> AttributeHandles;

	static const TArray<FGameplayAttribute>& MeterAttributes();
	static void UpdateMeter(UProgressBar* Meter, UTextBlock* Label, const TCHAR* Caption, float Value, float Capacity);
	void ReleaseAttributes();
	void RefreshMeters();
	void RefreshGuardMeter(const AVampsCharacter& Fighter, bool bAttributesChanged);
	void HandleAttributeChanged(const FOnAttributeChangeData& AttributeChange);
};
