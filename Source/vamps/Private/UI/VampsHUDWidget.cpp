#include "UI/VampsHUDWidget.h"
#include "UI/VampsCharacterOptionWidget.h"
#include "UI/VampsEnemyWidget.h"
#include "UI/VampsFighterWidget.h"
#include "AI/VampsAIController.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Characters/VampsCharacter.h"
#include "Characters/VampsLockOnComponent.h"
#include "Combat/VampsCombatLog.h"
#include "Combat/Data/VampsCombatSettings.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ScaleBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameModes/VampsCombatTestGameMode.h"
#include "GameModes/VampsMatchGameMode.h"

namespace
{
	void ShowWidget(UWidget* Widget, bool bShow)
	{
		Widget->SetVisibility(bShow ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	}

	FString FormatRatio(int32 Kills, int32 Deaths)
	{
		return Deaths > 0 ? FString::Printf(TEXT("%.2f"), double(Kills) / Deaths)
		                  : FString(Kills > 0 ? TEXT("INF") : TEXT("0.00"));
	}
} // namespace

void UVampsHUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (!Cast<UBorder>(NoticeText->GetParent()) || !Cast<UBorder>(TeamText->GetParent()) ||
	    !Cast<UBorder>(RespawnText->GetParent()))
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s' requires Border parents for NoticeText, TeamText and RespawnText."),
		       __FUNCTION__,
		       *GetPathName());
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	MatchMode = Cast<AVampsMatchGameMode>(GetWorld()->GetAuthGameMode());
	StartButton->OnClicked.AddDynamic(this, &UVampsHUDWidget::StartRound);
	RestartButton->OnClicked.AddDynamic(this, &UVampsHUDWidget::ReturnToSelection);
	if (!EnemyWidgetClass || !FighterWidgetClass || !CharacterOptionWidgetClass)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s' requires EnemyWidgetClass, FighterWidgetClass and CharacterOptionWidgetClass."),
		       __FUNCTION__,
		       *GetPathName());
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	if (MatchMode.IsValid())
	{
		for (UVampsCombatSettings* CharacterSettings : MatchMode->AvailableCharacters)
		{
			UVampsCharacterOptionWidget* CharacterOption =
			    CreateWidget<UVampsCharacterOptionWidget>(GetOwningPlayer(), CharacterOptionWidgetClass);
			if (!CharacterOption)
			{
				UE_LOG(LogVampsCombat,
				       Error,
				       TEXT("%hs: '%s' could not create a character option for '%s'."),
				       __FUNCTION__,
				       *GetPathName(),
				       *GetPathNameSafe(CharacterSettings));
				SetVisibility(ESlateVisibility::Collapsed);
				return;
			}
			if (!CharacterOption->ConfigureCharacter(CharacterSettings, MatchMode.Get()))
			{
				SetVisibility(ESlateVisibility::Collapsed);
				return;
			}
			CharacterOptions->AddChildToVerticalBox(CharacterOption)->SetPadding(FMargin(0.f, 0.f, 0.f, 8.f));
		}
	}
}

void UVampsHUDWidget::StartRound()
{
	if (MatchMode.IsValid())
	{
		MatchMode->StartLocalRound();
	}
}

void UVampsHUDWidget::ReturnToSelection()
{
	if (MatchMode.IsValid())
	{
		MatchMode->OpenCharacterSelection();
	}
}

void UVampsHUDWidget::ToggleCombatDebug()
{
#if !UE_BUILD_SHIPPING
	if (GetWorld()->WorldType != EWorldType::PIE)
	{
		return;
	}
	bShowCombatDebug = !bShowCombatDebug;
	UE_LOG(LogVampsCombat,
	       Display,
	       TEXT("%s: %s HUD (F9)."),
	       *GetPathName(),
	       bShowCombatDebug ? TEXT("debug") : TEXT("combat"));
#endif
}

void UVampsHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	const FVector2D ViewSize = MyGeometry.GetLocalSize();
	if (ViewSize.X <= 0.f || ViewSize.Y <= 0.f)
	{
		return;
	}
	const float ViewScale = FMath::Min(ViewSize.X / 1280.f, ViewSize.Y / 720.f);
	LayoutScale->SetUserSpecifiedScale(ViewScale);
	const float DpiScale = UWidgetLayoutLibrary::GetViewportScale(this);
	const FVector2D DebugSize = ViewSize * DpiScale;
	DebugScale->SetUserSpecifiedScale(1.f / (ViewScale * DpiScale));
	const float DebugWidth = FMath::Min(380.f, DebugSize.X * 0.43f);
	Cast<UCanvasPanelSlot>(DebugPlayerPanel->Slot)->SetSize(FVector2D(DebugWidth, 425.f));
	UCanvasPanelSlot* OpponentsSlot = Cast<UCanvasPanelSlot>(DebugOpponents->Slot);
	OpponentsSlot->SetPosition(FVector2D(-DebugWidth - 20.f, 20.f));
	OpponentsSlot->SetSize(FVector2D(DebugWidth, DebugOpponents->GetChildrenCount() * 445.f));
	const bool bSelecting = MatchMode.IsValid() && MatchMode->RoundPhase == EVampsMatchPhase::Selecting;
	const bool bUnavailable = MatchMode.IsValid() && MatchMode->RoundPhase == EVampsMatchPhase::Unavailable;
	const bool bPreparing = MatchMode.IsValid() && MatchMode->RoundPhase == EVampsMatchPhase::Preparing;
	ShowWidget(SelectionView, bSelecting);
	ShowWidget(CombatView, !bSelecting && !bUnavailable && !bShowCombatDebug);
	ShowWidget(DebugView, !bSelecting && !bUnavailable && bShowCombatDebug);
	ShowWidget(MatchView, MatchMode.IsValid() && !bSelecting && !bUnavailable);
	ShowWidget(ResultView, MatchMode.IsValid() && MatchMode->RoundPhase == EVampsMatchPhase::Finished);
	ShowWidget(NoticeText->GetParent(), bPreparing || bUnavailable);
#if !UE_BUILD_SHIPPING
	ShowWidget(DebugHint, GetWorld()->WorldType == EWorldType::PIE && !bSelecting && !bUnavailable);
	DebugHint->SetText(FText::FromString(bShowCombatDebug ? TEXT("F9 COMBAT HUD") : TEXT("F9 DEBUG HUD")));
#else
	ShowWidget(DebugHint, false);
#endif
	if (bSelecting)
	{
		const bool bCharacterSelected = MatchMode->SelectedCharacter != nullptr;
		StartButton->SetIsEnabled(bCharacterSelected);
		StartButton->SetBackgroundColor(bCharacterSelected ? FLinearColor(0.15f, 0.35f, 0.45f)
		                                                   : FLinearColor(0.08f, 0.08f, 0.1f));
		for (int32 CharacterIndex = 0; CharacterIndex < CharacterOptions->GetChildrenCount(); ++CharacterIndex)
		{
			Cast<UVampsCharacterOptionWidget>(CharacterOptions->GetChildAt(CharacterIndex))
			    ->RefreshSelection(MatchMode->SelectedCharacter);
		}
		return;
	}
	if (bUnavailable)
	{
		NoticeText->SetText(FText::FromString(TEXT("MATCH UNAVAILABLE - See LogVampsCombat")));
		NoticeText->SetColorAndOpacity(FLinearColor::Red);
		return;
	}
	RefreshFighters(ViewSize.Y / ViewScale);
	if (MatchMode.IsValid())
	{
		RefreshMatch();
	}
}

void UVampsHUDWidget::RefreshFighters(float LayoutHeight)
{
	AVampsCharacter* Viewer = Cast<AVampsCharacter>(GetOwningPlayerPawn());
	if (!Viewer || !Viewer->LockOn)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s' requires a VampsCharacter with LockOn; pawn='%s'."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetNameSafe(GetOwningPlayerPawn()));
		ShowWidget(CombatView, false);
		ShowWidget(DebugView, false);
		return;
	}
	AVampsCharacter* SelectedFighter = Viewer->LockOn->GetCurrentTarget();
	PlayerPanel->ObserveFighter(Viewer, TEXT("PLAYER"));
	TargetPanel->ObserveFighter(SelectedFighter, TEXT("TARGET"));
	ShowWidget(NoTargetText, !SelectedFighter);
	if (bShowCombatDebug)
	{
		DebugPlayerPanel->ObserveFighter(Viewer, TEXT("PLAYER"), true);
		int32 PanelIndex = 0;
		for (const TWeakObjectPtr<AVampsCharacter>& Candidate : Viewer->LockOn->GetCandidates())
		{
			AVampsCharacter* Fighter = Candidate.Get();
			if (!Fighter)
			{
				continue;
			}
			if (PanelIndex == DebugOpponents->GetChildrenCount())
			{
				UVampsFighterWidget* NewPanel =
				    CreateWidget<UVampsFighterWidget>(GetOwningPlayer(), FighterWidgetClass);
				if (!NewPanel)
				{
					UE_LOG(LogVampsCombat,
					       Error,
					       TEXT("%hs: '%s' could not create a debug fighter panel."),
					       __FUNCTION__,
					       *GetPathName());
					return;
				}
				DebugOpponents->AddChildToVerticalBox(NewPanel)->SetPadding(FMargin(0.f, 0.f, 0.f, 20.f));
			}
			Cast<UVampsFighterWidget>(DebugOpponents->GetChildAt(PanelIndex++))
			    ->ObserveFighter(Fighter, Fighter->GetName(), true, Fighter == SelectedFighter);
		}
		while (DebugOpponents->GetChildrenCount() > PanelIndex)
		{
			DebugOpponents->RemoveChildAt(DebugOpponents->GetChildrenCount() - 1);
		}
		const AVampsCombatTestGameMode* TestMode = Cast<AVampsCombatTestGameMode>(GetWorld()->GetAuthGameMode());
		if (TestMode && TestMode->OpponentAI)
		{
			TestControlsText->SetText(FText::FromString(
			    TEXT("AI: ") + StaticEnum<EVampsAIMode>()->GetNameStringByValue(int64(TestMode->OpponentAI->Mode)) +
			    TEXT(" | F6 next mode | R reset\nWASD move | Shift dash/hold sprint | RMB combat (click) | F next "
			         "target")));
		}
		ShowWidget(TestControlsText, TestMode != nullptr);
		return;
	}
	TArray<AVampsCharacter*> Enemies;
	for (const TWeakObjectPtr<AVampsCharacter>& Candidate : Viewer->LockOn->GetCandidates())
	{
		if (Viewer->CanEngageEnemy(Candidate.Get()))
		{
			Enemies.Add(Candidate.Get());
		}
	}
	const int32 VisibleRows = FMath::Max(1, FMath::FloorToInt((LayoutHeight - 260.f) / 26.f));
	const int32 SelectedRow = Enemies.IndexOfByKey(SelectedFighter);
	const int32 FirstRow = SelectedRow == INDEX_NONE ? 0 : SelectedRow / VisibleRows * VisibleRows;
	const int32 EndRow = FMath::Min(FirstRow + VisibleRows, Enemies.Num());
	EnemyHeading->SetText(FText::FromString(FString::Printf(
	    TEXT("ENEMIES %d-%d / %d | F next"), Enemies.IsEmpty() ? 0 : FirstRow + 1, EndRow, Enemies.Num())));
	for (int32 EnemyIndex = 0; EnemyIndex < Enemies.Num(); ++EnemyIndex)
	{
		if (EnemyIndex == EnemyIndicators->GetChildrenCount())
		{
			UVampsEnemyWidget* NewIndicator = CreateWidget<UVampsEnemyWidget>(GetOwningPlayer(), EnemyWidgetClass);
			if (!NewIndicator)
			{
				UE_LOG(LogVampsCombat,
				       Error,
				       TEXT("%hs: '%s' could not create an enemy indicator."),
				       __FUNCTION__,
				       *GetPathName());
				return;
			}
			UCanvasPanelSlot* IndicatorSlot = EnemyIndicators->AddChildToCanvas(NewIndicator);
			IndicatorSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
			IndicatorSlot->SetOffsets(FMargin(0.f));
		}
		Cast<UVampsEnemyWidget>(EnemyIndicators->GetChildAt(EnemyIndex))
		    ->UpdateEnemy(Viewer,
		                  Enemies[EnemyIndex],
		                  EnemyIndex,
		                  EnemyIndex >= FirstRow && EnemyIndex < EndRow ? EnemyIndex - FirstRow : INDEX_NONE,
		                  Enemies[EnemyIndex] == SelectedFighter);
	}
	while (EnemyIndicators->GetChildrenCount() > Enemies.Num())
	{
		EnemyIndicators->RemoveChildAt(EnemyIndicators->GetChildrenCount() - 1);
	}
}

void UVampsHUDWidget::RefreshMatch()
{
	const double Now = GetWorld()->GetTimeSeconds();
	const int32 Seconds = FMath::CeilToInt(FMath::Max(0.0, MatchMode->PhaseEndsAt - Now));
	if (MatchMode->TeamScores.Num() != 2)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s' requires two team scores; received %d."),
		       __FUNCTION__,
		       *MatchMode->GetPathName(),
		       MatchMode->TeamScores.Num());
		return;
	}
	ScoreText->SetText(FText::FromString(FString::Printf(TEXT("A %d / %d  |  %02d:%02d  |  B %d / %d"),
	                                                     MatchMode->TeamScores[0],
	                                                     MatchMode->ScoreTarget,
	                                                     Seconds / 60,
	                                                     Seconds % 60,
	                                                     MatchMode->TeamScores[1],
	                                                     MatchMode->ScoreTarget)));
	NoticeText->SetColorAndOpacity(FLinearColor::Yellow);
	NoticeText->SetText(FText::FromString(FString::Printf(TEXT("MATCH STARTS IN %d"), Seconds)));
	TeamText->SetText(FText::GetEmpty());
	RespawnText->SetText(FText::GetEmpty());
	ProtectionText->SetText(FText::GetEmpty());
	for (const FVampsMatchParticipant& Participant : MatchMode->Participants)
	{
		if (Participant.Fighter != GetOwningPlayerPawn())
		{
			continue;
		}
		TeamText->SetText(FText::FromString(Participant.TeamIndex == 0 ? TEXT("TEAM A") : TEXT("TEAM B")));
		if (Participant.RespawnAt > Now)
		{
			RespawnText->SetText(FText::FromString(
			    FString::Printf(TEXT("RESPAWN IN %d"), FMath::CeilToInt(Participant.RespawnAt - Now))));
		}
		else if (Participant.ProtectionEndsAt > Now)
		{
			ProtectionText->SetText(
			    FText::FromString(FString::Printf(TEXT("SPAWN PROTECTION %.1fs"), Participant.ProtectionEndsAt - Now)));
		}
	}
	ShowWidget(TeamText->GetParent(), !TeamText->GetText().IsEmpty());
	ShowWidget(RespawnText->GetParent(), !RespawnText->GetText().IsEmpty());
	if (MatchMode->RoundPhase == EVampsMatchPhase::Finished)
	{
		RefreshResults();
	}
}

void UVampsHUDWidget::RefreshResults()
{
	ResultText->SetText(
	    FText::FromString(MatchMode->WinningTeamIndex == INDEX_NONE
	                          ? TEXT("DRAW")
	                          : (MatchMode->WinningTeamIndex == 0 ? TEXT("TEAM A WINS") : TEXT("TEAM B WINS"))));
	int32 TeamKills[2] = {};
	int32 TeamDeaths[2] = {};
	FString ParticipantRows;
	for (int32 ParticipantIndex = 0; ParticipantIndex < MatchMode->Participants.Num(); ++ParticipantIndex)
	{
		const FVampsMatchParticipant& Participant = MatchMode->Participants[ParticipantIndex];
		if (Participant.TeamIndex < 0 || Participant.TeamIndex > 1)
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: '%s' has invalid team %d."),
			       __FUNCTION__,
			       *GetNameSafe(Participant.Fighter),
			       Participant.TeamIndex);
			return;
		}
		TeamKills[Participant.TeamIndex] += Participant.Kills;
		TeamDeaths[Participant.TeamIndex] += Participant.Deaths;
		ParticipantRows += FString::Printf(TEXT("%s%s%d%s | K %d / D %d | K/D %s"),
		                                   ParticipantIndex == 0 ? TEXT("") : TEXT("\n"),
		                                   Participant.TeamIndex == 0 ? TEXT("A") : TEXT("B"),
		                                   ParticipantIndex % 2 + 1,
		                                   ParticipantIndex == 0 ? TEXT(" YOU") : TEXT(" BOT"),
		                                   Participant.Kills,
		                                   Participant.Deaths,
		                                   *FormatRatio(Participant.Kills, Participant.Deaths));
	}
	ParticipantText->SetText(FText::FromString(ParticipantRows));
	FString TeamRows;
	for (int32 TeamIndex = 0; TeamIndex < 2; ++TeamIndex)
	{
		TeamRows += FString::Printf(TEXT("%sTEAM %s | %d pts | K %d / D %d | K/D %s"),
		                            TeamIndex == 0 ? TEXT("") : TEXT("\n"),
		                            TeamIndex == 0 ? TEXT("A") : TEXT("B"),
		                            MatchMode->TeamScores[TeamIndex],
		                            TeamKills[TeamIndex],
		                            TeamDeaths[TeamIndex],
		                            *FormatRatio(TeamKills[TeamIndex], TeamDeaths[TeamIndex]));
	}
	TeamResultsText->SetText(FText::FromString(TeamRows));
}
