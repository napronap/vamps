#include "UI/VampsCombatTestHUD.h"
#include "AI/VampsAIController.h"
#include "Characters/VampsCharacter.h"
#include "Characters/VampsLockOnComponent.h"
#include "Combat/Attributes/VampsAttributeSet.h"
#include "Combat/VampsCombatMovementComponent.h"
#include "Engine/Canvas.h"
#include "Engine/World.h"
#include "GameModes/VampsCombatTestGameMode.h"

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
	if (!Canvas || !PlayerOwner)
	{
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
