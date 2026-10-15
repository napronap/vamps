#include "UI/VampsCombatTestHUD.h"
#include "AI/VampsAIController.h"
#include "Characters/VampsCharacter.h"
#include "Characters/VampsLockOnComponent.h"
#include "Combat/Attributes/VampsAttributeSet.h"
#include "Engine/Canvas.h"
#include "GameModes/VampsCombatTestGameMode.h"

void AVampsCombatTestHUD::DrawFighter(
    AVampsCharacter* Fighter, const FString& Label, float X, float Y, float Width, bool bSelected)
{
	if (!Fighter)
	{
		return;
	}

	const float Health = Fighter->Attributes->GetHealth();
	const float MaxHealth = Fighter->Attributes->GetMaxHealth();
	const FVampsCombatState& FighterState = Fighter->CombatState;
	DrawRect(FLinearColor(0.025f, 0.025f, 0.04f, 0.86f), X, Y, Width, 200);
	if (bSelected)
	{
		DrawRect(FLinearColor(1.f, 0.75f, 0.15f), X, Y, 4, 200);
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
			TargetY += 220;
		}
	}
	const AVampsCombatTestGameMode* TestGameMode = CastChecked<AVampsCombatTestGameMode>(GetWorld()->GetAuthGameMode());
	check(TestGameMode->OpponentAI);
	const FString ModeName = StaticEnum<EVampsAIMode>()->GetNameStringByValue(int64(TestGameMode->OpponentAI->Mode));
	DrawText(TEXT("AI: ") + ModeName + TEXT(" | F6 next mode | R reset"), FLinearColor::White, 20, Canvas->SizeY - 75);
	DrawText(TEXT("WASD move | Auto camera | RMB combat (click) | F next target"),
	         FLinearColor::White,
	         20,
	         Canvas->SizeY - 50);
}
