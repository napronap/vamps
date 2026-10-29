#include "UI/VampsCharacterIndicatorWidget.h"
#include "Characters/VampsCharacter.h"
#include "Characters/VampsLockOnComponent.h"
#include "Combat/Data/VampsCombatSettings.h"
#include "Combat/VampsCombatLog.h"
#include "Components/ProgressBar.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameModes/VampsMatchGameMode.h"

void UVampsCharacterIndicatorWidget::ObserveFighter(AVampsCharacter* Fighter)
{
	ObservedFighter = Fighter;
}

void UVampsCharacterIndicatorWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	const AVampsCharacter* Fighter = ObservedFighter.Get();
	const APlayerController* ViewingController = GetWorld()->GetFirstPlayerController();
	const AVampsCharacter* Viewer = ViewingController && ViewingController->IsLocalController()
	                                    ? Cast<AVampsCharacter>(ViewingController->GetPawn())
	                                    : nullptr;
	const AVampsMatchGameMode* MatchMode = Cast<AVampsMatchGameMode>(GetWorld()->GetAuthGameMode());
	const bool bPlaying = !MatchMode || MatchMode->RoundPhase == EVampsMatchPhase::Preparing ||
	                      MatchMode->RoundPhase == EVampsMatchPhase::Active;
	const bool bShowIndicators =
	    bPlaying && Fighter && Viewer && Fighter->CombatState.bIsAlive && Viewer->CombatState.bIsAlive;
	const bool bSelected =
	    bShowIndicators && Fighter != Viewer && Viewer->LockOn && Viewer->LockOn->GetCurrentTarget() == Fighter;
	LockOnMarker->SetVisibility(bSelected ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	const double Now = GetWorld()->GetTimeSeconds();
	const bool bShowNA = bShowIndicators && Fighter == Viewer && Fighter->CombatState.NARecoveryEndsAt > Now;
	NAView->SetVisibility(bShowNA ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	if (!bShowNA)
	{
		NAProgress->SetPercent(0.f);
		return;
	}
	const UVampsCombatSettings* Settings = Fighter->CombatSettings;
	if (!Settings || !FMath::IsFinite(Settings->NARecoveryDuration) || Settings->NARecoveryDuration <= 0.f)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s' requires a finite, positive NARecoveryDuration in '%s'."),
		       __FUNCTION__,
		       *Fighter->GetPathName(),
		       *GetPathNameSafe(Settings));
		NAView->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	NAProgress->SetPercent(
	    FMath::Clamp(float((Fighter->CombatState.NARecoveryEndsAt - Now) / Settings->NARecoveryDuration), 0.f, 1.f));
}
