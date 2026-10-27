#include "UI/VampsCombatTestHUD.h"
#include "AI/VampsAIController.h"
#include "Characters/VampsCharacter.h"
#include "Characters/VampsLockOnComponent.h"
#include "Combat/Attributes/VampsAttributeSet.h"
#include "Combat/VampsCombatMovementComponent.h"
#include "Combat/VampsCombatLog.h"
#include "Engine/Canvas.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameModes/VampsCombatTestGameMode.h"

#if !UE_BUILD_SHIPPING
void AVampsCombatTestHUD::ToggleCombatHUD()
{
	bShowCombatDebug = !bShowCombatDebug;
	UE_LOG(LogVampsCombat,
	       Display,
	       TEXT("%s: %s HUD (F9)."),
	       *GetPathName(),
	       bShowCombatDebug ? TEXT("debug") : TEXT("combat"));
}
#endif

void AVampsCombatTestHUD::DrawCombatMeter(const TCHAR* MeterLabel,
                                          float MeterValue,
                                          float MeterCapacity,
                                          FLinearColor MeterColor,
                                          float MeterX,
                                          float MeterY,
                                          float ViewScale)
{
	if (!FMath::IsFinite(MeterValue) || !FMath::IsFinite(MeterCapacity) || MeterCapacity <= 0.f)
	{
		DrawText(FString::Printf(TEXT("%hs: %s requires finite values and positive capacity; %g / %g."),
		                         __FUNCTION__,
		                         MeterLabel,
		                         MeterValue,
		                         MeterCapacity),
		         FLinearColor::Red,
		         MeterX,
		         MeterY,
		         nullptr,
		         ViewScale);
		return;
	}
	DrawText(FString::Printf(TEXT("%s %.0f / %.0f"), MeterLabel, MeterValue, MeterCapacity),
	         FLinearColor::White,
	         MeterX,
	         MeterY,
	         nullptr,
	         ViewScale);
	DrawRect(FLinearColor(0.15f, 0.15f, 0.18f), MeterX, MeterY + 17.f * ViewScale, 276.f * ViewScale, 6.f * ViewScale);
	DrawRect(MeterColor,
	         MeterX,
	         MeterY + 17.f * ViewScale,
	         276.f * ViewScale * FMath::Clamp(MeterValue / MeterCapacity, 0.f, 1.f),
	         6.f * ViewScale);
}

void AVampsCombatTestHUD::DrawCombatPanel(
    AVampsCharacter* Fighter, const FString& PanelTitle, float PanelX, float PanelY, float ViewScale)
{
	DrawRect(FLinearColor(0.025f, 0.025f, 0.04f, 0.86f), PanelX, PanelY, 300.f * ViewScale, 160.f * ViewScale);
	const float ContentX = PanelX + 12.f * ViewScale;
	DrawText(PanelTitle, FLinearColor::White, ContentX, PanelY + 8.f * ViewScale, nullptr, ViewScale);
	if (!Fighter->Attributes)
	{
		DrawText(FString::Printf(TEXT("%hs: '%s' requires combat attributes."), __FUNCTION__, *Fighter->GetName()),
		         FLinearColor::Red,
		         ContentX,
		         PanelY + 30.f * ViewScale,
		         nullptr,
		         ViewScale);
		return;
	}
	const FVampsCombatState& FighterState = Fighter->CombatState;
	DrawCombatMeter(TEXT("HP"),
	                Fighter->Attributes->GetHealth(),
	                Fighter->Attributes->GetMaxHealth(),
	                FLinearColor(0.25f, 0.8f, 0.65f),
	                ContentX,
	                PanelY + 30.f * ViewScale,
	                ViewScale);
	DrawCombatMeter(TEXT("GUARD"),
	                Fighter->Attributes->GetGuardLoad(),
	                Fighter->Attributes->GetMaxGuardLoad(),
	                FighterState.Reaction == EVampsCombatReaction::Stun ? FLinearColor::Red
	                                                                    : FLinearColor(0.3f, 0.6f, 1.f),
	                ContentX,
	                PanelY + 56.f * ViewScale,
	                ViewScale);
	DrawCombatMeter(TEXT("ENERGY"),
	                Fighter->Attributes->GetMobilityEnergy(),
	                Fighter->Attributes->GetMaxMobilityEnergy(),
	                FLinearColor(1.f, 0.75f, 0.15f),
	                ContentX,
	                PanelY + 82.f * ViewScale,
	                ViewScale);
	const double Now = GetWorld()->GetTimeSeconds();
	FString StatusText;
	if (!FighterState.bIsAlive)
	{
		StatusText = TEXT("DOWN");
	}
	else if (FighterState.Reaction != EVampsCombatReaction::None)
	{
		StatusText =
		    FString::Printf(TEXT("%s %.1fs"),
			                *StaticEnum<EVampsCombatReaction>()->GetNameStringByValue(int64(FighterState.Reaction)),
			                FMath::Max(0.0, FighterState.ReactionEndsAt - Now));
	}
	else if (FighterState.bIsDashing || FighterState.bIsSprinting)
	{
		StatusText = FighterState.bIsDashing ? TEXT("DASH") : TEXT("SPRINT");
	}
	else if (FighterState.Phase == EVampsActionPhase::Preparation || FighterState.Phase == EVampsActionPhase::Contacts)
	{
		StatusText = FString::Printf(TEXT("ATTACK %d"), FighterState.ComboStep);
	}
	else if (FighterState.Phase == EVampsActionPhase::Recovery)
	{
		StatusText = TEXT("RECOVERY");
	}
	if (FighterState.bIsAlive &&
	    (FighterState.Phase == EVampsActionPhase::NARecovery || FighterState.NARecoveryEndsAt > Now))
	{
		if (!StatusText.IsEmpty())
		{
			StatusText += TEXT(" | ");
		}
		StatusText += FString::Printf(TEXT("NA %.1fs"), FMath::Max(0.0, FighterState.NARecoveryEndsAt - Now));
	}
	if (StatusText.IsEmpty())
	{
		StatusText = TEXT("READY");
	}
	DrawText(StatusText,
	         FighterState.bIsAlive ? FLinearColor::White : FLinearColor::Red,
	         ContentX,
	         PanelY + 111.f * ViewScale,
	         nullptr,
	         ViewScale);
	if (FighterState.LastIncomingContact != EVampsContactResult::None)
	{
		const TCHAR* ResultText =
		    FighterState.LastIncomingContact == EVampsContactResult::Blocked
		        ? TEXT("BLOCK")
		        : (FighterState.LastIncomingContact == EVampsContactResult::GuardBreak ? TEXT("GUARD BREAK")
		                                                                               : TEXT("HIT"));
		DrawText(FString::Printf(TEXT("LAST: %s -%.1f%s"),
		                         ResultText,
		                         FighterState.LastDamageReceived,
		                         FighterState.bLastIncomingCritical ? TEXT(" CRITICAL") : TEXT("")),
		         FighterState.bLastIncomingCritical ? FLinearColor::Yellow : FLinearColor::White,
		         ContentX,
		         PanelY + 135.f * ViewScale,
		         nullptr,
		         ViewScale);
	}
}

void AVampsCombatTestHUD::DrawCombatEnemies(AVampsCharacter* ViewingFighter, float ListX, float ListY, float ViewScale)
{
	TArray<AVampsCharacter*> LivingEnemies;
	for (const TWeakObjectPtr<AVampsCharacter>& Candidate : ViewingFighter->LockOn->GetCandidates())
	{
		AVampsCharacter* EnemyFighter = Candidate.Get();
		if (IsValid(EnemyFighter) && EnemyFighter->HasActorBegunPlay() && !EnemyFighter->IsActorBeingDestroyed() &&
		    EnemyFighter->CombatState.bIsAlive)
		{
			LivingEnemies.Add(EnemyFighter);
		}
	}
	AVampsCharacter* SelectedFighter = ViewingFighter->LockOn->GetCurrentTarget();
	const int32 VisibleRows =
	    FMath::Max(1, FMath::FloorToInt((Canvas->SizeY - ListY - 60.f * ViewScale) / (26.f * ViewScale)));
	const int32 SelectedRow = LivingEnemies.IndexOfByKey(SelectedFighter);
	const int32 FirstRow = SelectedRow == INDEX_NONE ? 0 : (SelectedRow / VisibleRows) * VisibleRows;
	const int32 EndRow = FMath::Min(FirstRow + VisibleRows, LivingEnemies.Num());
	DrawText(FString::Printf(TEXT("ENEMIES %d-%d / %d | F next"),
	                         LivingEnemies.IsEmpty() ? 0 : FirstRow + 1,
	                         EndRow,
	                         LivingEnemies.Num()),
	         FLinearColor::White,
	         ListX,
	         ListY,
	         nullptr,
	         ViewScale);
	const FRotationMatrix CameraAxes(FRotator(0.f, PlayerOwner->GetControlRotation().Yaw, 0.f));
	const auto DrawDirection =
	    [this, ViewScale](const FVector2D& ArrowOrigin, const FVector2D& ArrowDirection, FLinearColor ArrowColor)
	{
		if (ArrowDirection.IsNearlyZero())
		{
			return;
		}
		const FVector2D ArrowTip = ArrowOrigin + ArrowDirection * 8.f * ViewScale;
		const FVector2D ArrowBase = ArrowOrigin - ArrowDirection * 6.f * ViewScale;
		const FVector2D ArrowWing(-ArrowDirection.Y * 5.f * ViewScale, ArrowDirection.X * 5.f * ViewScale);
		DrawLine(
		    ArrowTip.X, ArrowTip.Y, ArrowBase.X + ArrowWing.X, ArrowBase.Y + ArrowWing.Y, ArrowColor, 2.f * ViewScale);
		DrawLine(
		    ArrowTip.X, ArrowTip.Y, ArrowBase.X - ArrowWing.X, ArrowBase.Y - ArrowWing.Y, ArrowColor, 2.f * ViewScale);
	};
	for (int32 EnemyIndex = 0; EnemyIndex < LivingEnemies.Num(); ++EnemyIndex)
	{
		const AVampsCharacter* EnemyFighter = LivingEnemies[EnemyIndex];
		const FVampsCombatState& EnemyState = EnemyFighter->CombatState;
		const bool bIncomingAttack =
		    ViewingFighter->CombatState.bIsAlive && EnemyFighter->LockOn &&
		    EnemyFighter->LockOn->GetCurrentTarget() == ViewingFighter &&
		    (EnemyState.Phase == EVampsActionPhase::Preparation || EnemyState.Phase == EVampsActionPhase::Contacts);
		const FVector OpponentOffset = EnemyFighter->GetActorLocation() - ViewingFighter->GetActorLocation();
		const FVector2D ArrowDirection =
		    FVector2D(FVector::DotProduct(OpponentOffset, CameraAxes.GetUnitAxis(EAxis::Y)),
			          -FVector::DotProduct(OpponentOffset, CameraAxes.GetUnitAxis(EAxis::X)))
		        .GetSafeNormal();
		if (bIncomingAttack)
		{
			const FVector2D ThreatOrigin =
			    FVector2D(Canvas->SizeX * 0.5f, Canvas->SizeY * 0.5f) + ArrowDirection * 100.f * ViewScale;
			DrawDirection(ThreatOrigin, ArrowDirection, FLinearColor::Red);
			DrawText(TEXT("!"),
			         FLinearColor::Red,
			         ThreatOrigin.X - 3.f * ViewScale,
			         ThreatOrigin.Y + 10.f * ViewScale,
			         nullptr,
			         ViewScale);
		}
		if (EnemyIndex < FirstRow || EnemyIndex >= EndRow)
		{
			continue;
		}
		const double DistanceMeters = OpponentOffset.Size2D() / 100.0;
		const bool bSelectedEnemy = EnemyFighter == SelectedFighter;
		const FLinearColor RowColor =
		    bIncomingAttack ? FLinearColor::Red : (bSelectedEnemy ? FLinearColor::Yellow : FLinearColor::White);
		const float RowY = ListY + (24.f + (EnemyIndex - FirstRow) * 26.f) * ViewScale;
		DrawText(FString::Printf(TEXT("%s %d | %.1fm%s%s"),
		                         bSelectedEnemy ? TEXT(">") : TEXT(" "),
		                         EnemyIndex + 1,
		                         DistanceMeters,
		                         DistanceMeters <= 15.0 ? TEXT(" NEAR") : TEXT(""),
		                         bIncomingAttack ? TEXT(" ! INCOMING") : TEXT("")),
		         RowColor,
		         ListX,
		         RowY,
		         nullptr,
		         ViewScale);
		DrawDirection(FVector2D(ListX + 284.f * ViewScale, RowY + 8.f * ViewScale), ArrowDirection, RowColor);
	}
}

void AVampsCombatTestHUD::DrawCombatHUD(AVampsCharacter* ViewingFighter, float ViewScale)
{
	if (!IsValid(ViewingFighter) || !ViewingFighter->LockOn)
	{
		DrawText(FString::Printf(TEXT("%hs: requires a VampsCharacter with LockOn; pawn='%s'."),
		                         __FUNCTION__,
		                         *GetNameSafe(PlayerOwner->GetPawn())),
		         FLinearColor::Red,
		         20.f * ViewScale,
		         50.f * ViewScale,
		         nullptr,
		         ViewScale);
		return;
	}
	DrawCombatPanel(ViewingFighter, TEXT("PLAYER"), 20.f * ViewScale, Canvas->SizeY - 186.f * ViewScale, ViewScale);
	const float TargetX = Canvas->SizeX - 320.f * ViewScale;
	AVampsCharacter* SelectedFighter = ViewingFighter->LockOn->GetCurrentTarget();
	if (SelectedFighter)
	{
		DrawCombatPanel(SelectedFighter, TEXT("TARGET"), TargetX, 20.f * ViewScale, ViewScale);
	}
	else
	{
		DrawText(TEXT("NO TARGET"), FLinearColor::White, TargetX, 20.f * ViewScale, nullptr, ViewScale);
	}
	DrawCombatEnemies(ViewingFighter, TargetX, 200.f * ViewScale, ViewScale);
}

void AVampsCombatTestHUD::DrawFighter(
    AVampsCharacter* Fighter, const FString& Label, float X, float Y, float Width, bool bSelected)
{
	if (!Fighter)
	{
		return;
	}
	const UVampsCombatMovementComponent* CombatMovement =
	    Cast<UVampsCombatMovementComponent>(Fighter->GetCharacterMovement());
	if (!Fighter->Attributes || !Fighter->LockOn || !CombatMovement || !(Fighter->Attributes->GetMaxHealth() > 0.f))
	{
		DrawText(FString::Printf(TEXT("%hs: %s: missing combat attributes, LockOn, movement or positive MaxHealth."),
		                         __FUNCTION__,
		                         *Fighter->GetName()),
		         FLinearColor::Red,
		         X,
		         Y);
		return;
	}

	const float Health = Fighter->Attributes->GetHealth();
	const float MaxHealth = Fighter->Attributes->GetMaxHealth();
	const FVampsCombatState& FighterState = Fighter->CombatState;
	DrawRect(FLinearColor(0.025f, 0.025f, 0.04f, 0.86f), X, Y, Width, 425);
	if (bSelected)
	{
		DrawRect(FLinearColor(1.f, 0.75f, 0.15f), X, Y, 4, 425);
	}
	DrawText(FString::Printf(TEXT("%s   %.0f / %.0f"), *Label, Health, MaxHealth), FLinearColor::White, X + 12, Y + 9);
	DrawRect(FLinearColor(0.15f, 0.15f, 0.18f), X + 12, Y + 32, Width - 24, 10);
	DrawRect(FLinearColor(0.25f, 0.8f, 0.65f),
	         X + 12,
	         Y + 32,
	         (Width - 24) * FMath::Clamp(Health / MaxHealth, 0.f, 1.f),
	         10);

	const FString Phase = StaticEnum<EVampsActionPhase>()->GetNameStringByValue(int64(FighterState.Phase));
	DrawText(
	    FString::Printf(TEXT("ACTION %d: %s"), FighterState.ActionNumber, *Phase), FLinearColor::White, X + 12, Y + 52);
	const FString Availability = FString::Printf(TEXT("START: %s | HIT: %s | GUARD: %s"),
	                                             FighterState.bCanStartAction ? TEXT("YES") : TEXT("NO"),
	                                             FighterState.bCanDealDamage ? TEXT("ON") : TEXT("OFF"),
	                                             FighterState.bCanDefend ? TEXT("ARMED") : TEXT("UNARMED"));
	DrawText(Availability, FLinearColor::White, X + 12, Y + 74);
	const FString Request = StaticEnum<EVampsCombatRequest>()->GetNameStringByValue(int64(FighterState.LastRequest));
	DrawText(FString::Printf(TEXT("REQUEST #%d: %s"), FighterState.RequestCount, *Request),
	         FLinearColor::White,
	         X + 12,
	         Y + 96);
	const FString Outgoing =
	    StaticEnum<EVampsContactResult>()->GetNameStringByValue(int64(FighterState.LastOutgoingContact));
	const FString Incoming =
	    StaticEnum<EVampsContactResult>()->GetNameStringByValue(int64(FighterState.LastIncomingContact));
	DrawText(FString::Printf(TEXT("OUT: %s | FRAME: %llu"), *Outgoing, FighterState.LastOutgoingContactFrame),
	         FLinearColor::White,
	         X + 12,
	         Y + 118);
	DrawText(FString::Printf(TEXT("IN: %s | FRAME: %llu"), *Incoming, FighterState.LastIncomingContactFrame),
	         FLinearColor::White,
	         X + 12,
	         Y + 140);
	DrawText(
	    TEXT("LOCK ON: ") + GetNameSafe(Fighter->LockOn->GetCurrentTarget()), FLinearColor::White, X + 12, Y + 162);
	DrawText(FString::Printf(TEXT("STEP %d | WINDOW: %s | CONTACT: %s"),
	                         FighterState.ComboStep,
	                         FighterState.bComboWindowOpen ? TEXT("OPEN") : TEXT("CLOSED"),
	                         FighterState.bActionConnected ? TEXT("YES") : TEXT("NO")),
	         FLinearColor::White,
	         X + 12,
	         Y + 184);
	DrawText(
	    FString::Printf(TEXT("CONTINUATION: %s"), FighterState.bContinuationRequested ? TEXT("PENDING") : TEXT("NONE")),
	    FLinearColor::White,
	    X + 12,
	    Y + 206);
	const double Now = GetWorld()->GetTimeSeconds();
	DrawText(FString::Printf(TEXT("REACTION TIME: %.2f | NA: %.2f"),
	                         FMath::Max(0.0, FighterState.ReactionEndsAt - Now),
	                         FMath::Max(0.0, FighterState.NARecoveryEndsAt - Now)),
	         FLinearColor::White,
	         X + 12,
	         Y + 228);
	const FString MotionName =
	    StaticEnum<EVampsCombatMotion>()->GetNameStringByValue(int64(CombatMovement->CombatMotion));
	DrawText(FString::Printf(TEXT("MOTION: %s | WALL: %s | RELEASED: %s"),
	                         *MotionName,
	                         CombatMovement->bCombatMotionBlocked ? TEXT("YES") : TEXT("NO"),
	                         FighterState.bRecoveryVisualReleased ? TEXT("YES") : TEXT("NO")),
	         FLinearColor::White,
	         X + 12,
	         Y + 250);
	const FString ReactionName = StaticEnum<EVampsCombatReaction>()->GetNameStringByValue(int64(FighterState.Reaction));
	DrawText(FString::Printf(
	             TEXT("REACTION: %s | MOVE: %s"), *ReactionName, FighterState.bCanMove ? TEXT("YES") : TEXT("NO")),
	         FLinearColor::White,
	         X + 12,
	         Y + 272);
	DrawText(FString::Printf(TEXT("GUARD LOAD: %.0f / %.0f"),
	                         Fighter->Attributes->GetGuardLoad(),
	                         Fighter->Attributes->GetMaxGuardLoad()),
	         FLinearColor::White,
	         X + 12,
	         Y + 294);
	DrawRect(FLinearColor(0.15f, 0.15f, 0.18f), X + 12, Y + 316, Width - 24, 8);
	DrawRect(FighterState.Reaction == EVampsCombatReaction::Stun ? FLinearColor::Red : FLinearColor(0.3f, 0.6f, 1.f),
	         X + 12,
	         Y + 316,
	         (Width - 24) *
	             FMath::Clamp(Fighter->Attributes->GetGuardLoad() / Fighter->Attributes->GetMaxGuardLoad(), 0.f, 1.f),
	         8);
	DrawText(FString::Printf(TEXT("DAMAGE: %.1f | CRITICAL: %s"),
	                         FighterState.LastDamageReceived,
	                         FighterState.bLastIncomingCritical ? TEXT("YES") : TEXT("NO")),
	         FighterState.bLastIncomingCritical ? FLinearColor::Yellow : FLinearColor::White,
	         X + 12,
	         Y + 332);
	DrawText(FString::Printf(TEXT("DEFENSE: %s #%d"),
	                         *GetNameSafe(FighterState.DefendedAttacker.Get()),
	                         FighterState.DefendedActionNumber),
	         FLinearColor::White,
	         X + 12,
	         Y + 354);
	DrawText(FString::Printf(TEXT("MOBILITY ENERGY: %.0f / %.0f | SPRINT: %s"),
	                         Fighter->Attributes->GetMobilityEnergy(),
	                         Fighter->Attributes->GetMaxMobilityEnergy(),
	                         FighterState.bIsSprinting ? TEXT("ON") : TEXT("OFF")),
	         FLinearColor::White,
	         X + 12,
	         Y + 376);
	DrawText(FString::Printf(TEXT("DASH: %s | ACTIVE: %s | PENDING: %s"),
	                         FighterState.bCanDash ? TEXT("READY") : TEXT("NO"),
	                         FighterState.bIsDashing ? TEXT("YES") : TEXT("NO"),
	                         FighterState.bDashRequested ? TEXT("YES") : TEXT("NO")),
	         FLinearColor::White,
	         X + 12,
	         Y + 398);
}

void AVampsCombatTestHUD::DrawHUD()
{
	Super::DrawHUD();
	if (!Canvas || !PlayerOwner || Canvas->SizeX <= 0 || Canvas->SizeY <= 0)
	{
		return;
	}

	const float ViewScale = FMath::Min(Canvas->SizeX / 1280.f, Canvas->SizeY / 720.f);
#if !UE_BUILD_SHIPPING
	DrawText(bShowCombatDebug ? TEXT("F9 COMBAT HUD") : TEXT("F9 DEBUG HUD"),
	         FLinearColor::White,
	         20.f * ViewScale,
	         Canvas->SizeY - 20.f * ViewScale,
	         nullptr,
	         ViewScale);
#endif
	if (!bShowCombatDebug)
	{
		DrawCombatHUD(Cast<AVampsCharacter>(PlayerOwner->GetPawn()), ViewScale);
		return;
	}

	const float Width = FMath::Min(380.f, Canvas->SizeX * 0.43f);
	AVampsCharacter* PlayerCharacter = Cast<AVampsCharacter>(PlayerOwner->GetPawn());
	DrawFighter(PlayerCharacter, TEXT("PLAYER"), 20, 20, Width, false);
	if (PlayerCharacter && !PlayerCharacter->LockOn)
	{
		return;
	}
	if (PlayerCharacter)
	{
		float TargetY = 20;
		for (const TWeakObjectPtr<AVampsCharacter>& Candidate : PlayerCharacter->LockOn->GetCandidates())
		{
			AVampsCharacter* Fighter = Candidate.Get();
			if (!IsValid(Fighter))
			{
				continue;
			}
			DrawFighter(Fighter,
			            Fighter->GetName(),
			            Canvas->SizeX - Width - 20,
			            TargetY,
			            Width,
			            Fighter == PlayerCharacter->LockOn->GetCurrentTarget());
			TargetY += 445;
		}
	}
	const AVampsCombatTestGameMode* TestGameMode = Cast<AVampsCombatTestGameMode>(GetWorld()->GetAuthGameMode());
	if (!TestGameMode || !IsValid(TestGameMode->OpponentAI))
	{
		DrawText(FString::Printf(TEXT("%hs: requires VampsCombatTestGameMode with a valid OpponentAI; got '%s'."),
		                         __FUNCTION__,
		                         *GetNameSafe(TestGameMode)),
		         FLinearColor::Red,
		         20,
		         Canvas->SizeY - 75);
		return;
	}
	const FString ModeName = StaticEnum<EVampsAIMode>()->GetNameStringByValue(int64(TestGameMode->OpponentAI->Mode));
	DrawText(TEXT("AI: ") + ModeName + TEXT(" | F6 next mode | R reset"), FLinearColor::White, 20, Canvas->SizeY - 75);
	DrawText(TEXT("WASD move | Shift dash/hold sprint | RMB combat (click) | F next target"),
	         FLinearColor::White,
	         20,
	         Canvas->SizeY - 50);
}
