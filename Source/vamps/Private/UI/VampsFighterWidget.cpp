#include "UI/VampsFighterWidget.h"
#include "Characters/VampsCharacter.h"
#include "Combat/Attributes/VampsAttributeSet.h"
#include "Combat/Data/VampsCombatSettings.h"
#include "Combat/VampsCombatLog.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

const TArray<FGameplayAttribute>& UVampsFighterWidget::MeterAttributes()
{
	static const TArray<FGameplayAttribute> Attributes = {UVampsAttributeSet::GetHealthAttribute(),
	                                                      UVampsAttributeSet::GetMaxHealthAttribute(),
	                                                      UVampsAttributeSet::GetGuardLoadAttribute(),
	                                                      UVampsAttributeSet::GetMaxGuardLoadAttribute(),
	                                                      UVampsAttributeSet::GetMobilityEnergyAttribute(),
	                                                      UVampsAttributeSet::GetMaxMobilityEnergyAttribute()};
	return Attributes;
}

void UVampsFighterWidget::UpdateMeter(
    UProgressBar* Meter, UTextBlock* Label, const TCHAR* Caption, float Value, float Capacity)
{
	Meter->SetPercent(FMath::Clamp(Value / Capacity, 0.f, 1.f));
	Label->SetText(FText::FromString(FString::Printf(TEXT("%s %.0f / %.0f"), Caption, Value, Capacity)));
}

void UVampsFighterWidget::ObserveFighter(AVampsCharacter* NewFighter)
{
	SetMinimumDesiredSize(FVector2D(300.f, 160.f));
	if (NewFighter && ObservedFighter.Get() == NewFighter)
	{
		return;
	}
	ReleaseAttributes();
	ObservedFighter = NewFighter;
	SetVisibility(NewFighter ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	if (!NewFighter)
	{
		return;
	}
	if (!NewFighter->Attributes || !NewFighter->AbilitySystem)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s' cannot observe '%s'; attributes or ability system missing."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(NewFighter));
		SetVisibility(ESlateVisibility::Collapsed);
		ObservedFighter.Reset();
		return;
	}
	ObservedAbilitySystem = NewFighter->AbilitySystem;
	for (const FGameplayAttribute& MeterAttribute : MeterAttributes())
	{
		AttributeHandles.Add(NewFighter->AbilitySystem->GetGameplayAttributeValueChangeDelegate(MeterAttribute)
		                         .AddUObject(this, &UVampsFighterWidget::HandleAttributeChanged));
	}
	RefreshMeters();
}

void UVampsFighterWidget::ReleaseAttributes()
{
	if (UAbilitySystemComponent* PreviousAbilitySystem = ObservedAbilitySystem.Get())
	{
		const TArray<FGameplayAttribute>& MeterProperties = MeterAttributes();
		for (int32 AttributeIndex = 0; AttributeIndex < AttributeHandles.Num(); ++AttributeIndex)
		{
			PreviousAbilitySystem->GetGameplayAttributeValueChangeDelegate(MeterProperties[AttributeIndex])
			    .Remove(AttributeHandles[AttributeIndex]);
		}
	}
	AttributeHandles.Reset();
	ObservedAbilitySystem.Reset();
}

void UVampsFighterWidget::NativeDestruct()
{
	ReleaseAttributes();
	ObservedFighter.Reset();
	Super::NativeDestruct();
}

void UVampsFighterWidget::HandleAttributeChanged(const FOnAttributeChangeData& AttributeChange)
{
	RefreshMeters();
}

void UVampsFighterWidget::RefreshMeters()
{
	const AVampsCharacter* Fighter = ObservedFighter.Get();
	if (!Fighter)
	{
		return;
	}
	const UVampsAttributeSet* Values = Fighter->Attributes;
	for (const FGameplayAttribute& MeterAttribute : MeterAttributes())
	{
		const float MeterValue = Fighter->AbilitySystem->GetNumericAttribute(MeterAttribute);
		if (!FMath::IsFinite(MeterValue) || (MeterAttribute.GetName().StartsWith(TEXT("Max")) && MeterValue <= 0.f))
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: '%s': invalid %s=%g."),
			       __FUNCTION__,
			       *Fighter->GetPathName(),
			       *MeterAttribute.GetName(),
			       MeterValue);
			SetVisibility(ESlateVisibility::Collapsed);
			return;
		}
	}
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	UpdateMeter(HealthBar, HealthText, TEXT("HP"), Values->GetHealth(), Values->GetMaxHealth());
	RefreshGuardMeter(*Fighter, true);
	UpdateMeter(EnergyBar, EnergyText, TEXT("ENERGY"), Values->GetMobilityEnergy(), Values->GetMaxMobilityEnergy());
}

void UVampsFighterWidget::RefreshGuardMeter(const AVampsCharacter& Fighter, bool bAttributesChanged)
{
	const FVampsCombatState& State = Fighter.CombatState;
	const bool bStunned = State.bIsAlive && State.Reaction == EVampsCombatReaction::Stun;
	if (!bStunned && !bShowingStun && !bAttributesChanged)
	{
		return;
	}
	if (bStunned)
	{
		const UVampsCombatSettings* Settings = Fighter.CombatSettings;
		if (!Settings || !FMath::IsFinite(Settings->StunDuration) || Settings->StunDuration <= 0.f)
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: '%s' requires a finite, positive StunDuration in '%s'."),
			       __FUNCTION__,
			       *Fighter.GetPathName(),
			       *GetPathNameSafe(Settings));
			SetVisibility(ESlateVisibility::Collapsed);
			return;
		}
		if (!bShowingStun)
		{
			GuardText->SetText(FText::FromString(TEXT("GUARD")));
		}
		GuardBar->SetPercent(FMath::Clamp(Fighter.GetCombatReactionTimeRemaining() / Settings->StunDuration, 0.f, 1.f));
	}
	else
	{
		const UVampsAttributeSet* Values = Fighter.Attributes;
		UpdateMeter(GuardBar, GuardText, TEXT("GUARD"), Values->GetGuardLoad(), Values->GetMaxGuardLoad());
	}
	const FLinearColor GuardColor = bStunned ? FLinearColor::Red : FLinearColor(0.3f, 0.6f, 1.f);
	GuardBar->SetFillColorAndOpacity(GuardColor);
	bShowingStun = bStunned;
}

void UVampsFighterWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	const AVampsCharacter* Fighter = ObservedFighter.Get();
	if (!Fighter)
	{
		return;
	}
	RefreshGuardMeter(*Fighter, false);
}
