#include "UI/VampsFighterWidget.h"
#include "Characters/VampsCharacter.h"
#include "Characters/VampsLockOnComponent.h"
#include "Combat/Attributes/VampsAttributeSet.h"
#include "Combat/Data/VampsCombatSettings.h"
#include "Combat/VampsCombatMovementComponent.h"
#include "Combat/VampsCombatLog.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Engine/World.h"

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

void UVampsFighterWidget::ObserveFighter(AVampsCharacter* NewFighter,
                                         const FString& PanelLabel,
                                         bool bDebug,
                                         bool bSelected)
{
	FighterLabel = PanelLabel;
	bDebugView = bDebug;
	SetMinimumDesiredSize(bDebug ? FVector2D(380.f, 425.f) : FVector2D(300.f, 160.f));
	CombatView->SetVisibility(bDebug ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
	DebugView->SetVisibility(bDebug ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	SelectionMark->SetVisibility(bSelected ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
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
	if (!NewFighter->Attributes || !NewFighter->AbilitySystem || !NewFighter->LockOn ||
	    !Cast<UVampsCombatMovementComponent>(NewFighter->GetCharacterMovement()))
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s' cannot observe '%s'; combat components are missing."),
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
	DebugHealthBar->SetPercent(HealthBar->GetPercent());
	DebugTitle->SetText(FText::FromString(
	    FString::Printf(TEXT("%s   %.0f / %.0f"), *FighterLabel, Values->GetHealth(), Values->GetMaxHealth())));
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
		const double RemainingStun = State.ReactionEndsAt - GetWorld()->GetTimeSeconds();
		GuardBar->SetPercent(FMath::Clamp(float(RemainingStun / Settings->StunDuration), 0.f, 1.f));
	}
	else
	{
		const UVampsAttributeSet* Values = Fighter.Attributes;
		UpdateMeter(GuardBar, GuardText, TEXT("GUARD"), Values->GetGuardLoad(), Values->GetMaxGuardLoad());
	}
	const FLinearColor GuardColor = bStunned ? FLinearColor::Red : FLinearColor(0.3f, 0.6f, 1.f);
	GuardBar->SetFillColorAndOpacity(GuardColor);
	DebugGuardBar->SetPercent(GuardBar->GetPercent());
	DebugGuardBar->SetFillColorAndOpacity(GuardColor);
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
	if (bDebugView)
	{
		RefreshDebugDetails();
	}
}

void UVampsFighterWidget::RefreshDebugDetails()
{
	const AVampsCharacter* Fighter = ObservedFighter.Get();
	const FVampsCombatState& State = Fighter->CombatState;
	const UVampsCombatMovementComponent* Movement =
	    Cast<UVampsCombatMovementComponent>(Fighter->GetCharacterMovement());
	const double Now = GetWorld()->GetTimeSeconds();
	FString Contact;
	if (State.LastIncomingContact != EVampsContactResult::None)
	{
		const TCHAR* Result =
		    State.LastIncomingContact == EVampsContactResult::Blocked
		        ? TEXT("BLOCK")
		        : (State.LastIncomingContact == EVampsContactResult::GuardBreak ? TEXT("GUARD BREAK") : TEXT("HIT"));
		Contact = FString::Printf(TEXT("LAST: %s -%.1f%s"),
		                          Result,
		                          State.LastDamageReceived,
		                          State.bLastIncomingCritical ? TEXT(" CRITICAL") : TEXT(""));
	}
	ContactText->SetText(FText::FromString(Contact));
	ContactText->SetColorAndOpacity(State.bLastIncomingCritical ? FLinearColor::Yellow : FLinearColor::White);
	DebugDetails->SetText(FText::FromString(FString::Printf(
	    TEXT("ACTION %d: %s\nSTART: %s | HIT: %s | GUARD: %s\nREQUEST #%d: %s\nOUT: %s | FRAME: %llu\nIN: %s | FRAME: "
	         "%llu\n")
	        TEXT("LOCK ON: %s\nSTEP %d | WINDOW: %s | CONTACT: %s\nCONTINUATION: %s\nREACTION TIME: %.2f | NA: %.2f\n")
	            TEXT("MOTION: %s | WALL: %s\nREACTION: %s | MOVE: %s\nGUARD LOAD: %.0f / %.0f"),
	    State.ActionNumber,
	    *StaticEnum<EVampsActionPhase>()->GetNameStringByValue(int64(State.Phase)),
	    State.bCanStartAction ? TEXT("YES") : TEXT("NO"),
	    State.bCanDealDamage ? TEXT("ON") : TEXT("OFF"),
	    State.bCanDefend ? TEXT("ARMED") : TEXT("UNARMED"),
	    State.RequestCount,
	    *StaticEnum<EVampsCombatRequest>()->GetNameStringByValue(int64(State.LastRequest)),
	    *StaticEnum<EVampsContactResult>()->GetNameStringByValue(int64(State.LastOutgoingContact)),
	    State.LastOutgoingContactFrame,
	    *StaticEnum<EVampsContactResult>()->GetNameStringByValue(int64(State.LastIncomingContact)),
	    State.LastIncomingContactFrame,
	    *GetNameSafe(Fighter->LockOn->GetCurrentTarget()),
	    State.ComboStep,
	    State.bComboWindowOpen ? TEXT("OPEN") : TEXT("CLOSED"),
	    State.bActionConnected ? TEXT("YES") : TEXT("NO"),
	    State.bContinuationRequested ? TEXT("PENDING") : TEXT("NONE"),
	    FMath::Max(0.0, State.ReactionEndsAt - Now),
	    FMath::Max(0.0, State.NARecoveryEndsAt - Now),
	    *StaticEnum<EVampsCombatMotion>()->GetNameStringByValue(int64(Movement->CombatMotion)),
	    Movement->bCombatMotionBlocked ? TEXT("YES") : TEXT("NO"),
	    *StaticEnum<EVampsCombatReaction>()->GetNameStringByValue(int64(State.Reaction)),
	    State.bCanMove ? TEXT("YES") : TEXT("NO"),
	    Fighter->Attributes->GetGuardLoad(),
	    Fighter->Attributes->GetMaxGuardLoad())));
	DebugContact->SetText(
	    FText::FromString(FString::Printf(TEXT("DEFENSE: %s #%d\nMOBILITY ENERGY: %.0f / "
	                                           "%.0f | SPRINT: %s\nDASH: %s | ACTIVE: %s | PENDING: %s"),
	                                      *GetNameSafe(State.DefendedAttacker.Get()),
	                                      State.DefendedActionNumber,
	                                      Fighter->Attributes->GetMobilityEnergy(),
	                                      Fighter->Attributes->GetMaxMobilityEnergy(),
	                                      State.bIsSprinting ? TEXT("ON") : TEXT("OFF"),
	                                      State.bCanDash ? TEXT("READY") : TEXT("NO"),
	                                      State.bIsDashing ? TEXT("YES") : TEXT("NO"),
	                                      State.bDashRequested ? TEXT("YES") : TEXT("NO"))));
}
