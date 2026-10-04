#include "UI/DuelHUD.h"
#include "Characters/VampsDuelCharacter.h"
#include "GameModes/DuelGameMode.h"
#include "AI/DuelAIController.h"
#include "Player/DuelPlayerController.h"
#include "Combat/Abilities/GA_Attack.h"
#include "Combat/Abilities/GA_Guard.h"
#include "Engine/Canvas.h"
#include "Engine/World.h"

void ADuelHUD::DrawFighter(AVampsDuelCharacter* Fighter, const FString& Label, float X, float Width)
{
	if (!Fighter)
	{
		return;
	}

	DrawRect(FLinearColor(0.025f, 0.025f, 0.04f, 0.86f), X, 20, Width, 130);
	DrawText(FString::Printf(TEXT("%s   %.0f / %.0f"), *Label, Fighter->GetHealth(), Fighter->GetMaxHealth()),
	         FLinearColor::White,
	         X + 12,
	         29);
	DrawRect(FLinearColor(0.15f, 0.15f, 0.18f), X + 12, 52, Width - 24, 10);
	DrawRect(FLinearColor(0.25f, 0.8f, 0.65f),
	         X + 12,
	         52,
	         (Width - 24) * FMath::Clamp(Fighter->GetHealth() / FMath::Max(1.f, Fighter->GetMaxHealth()), 0.f, 1.f),
	         10);
	FString State = StaticEnum<EDuelPhase>()->GetNameStringByValue(int64(Fighter->GetPhase()));

	if (Fighter->Attack && Fighter->Attack->IsActive())
	{
		State += FString::Printf(TEXT("  %d/3"), Fighter->Attack->StrikeIndex + 1);
	}

	if (Fighter->Guard && Fighter->Guard->IsActive())
	{
		State += Fighter->Guard->bAwaitingPress ? TEXT(" - press to continue") : TEXT(" - guard active");
	}

	DrawText(State, FLinearColor::White, X + 12, 72);
	DrawText(FString::Printf(TEXT("NA %.2f   Pause %.2f"), Fighter->GetNARemaining(), Fighter->GetNAPause()),
	         FLinearColor(1, 0.7f, 0.3f),
	         X + 12,
	         95);
	DrawText(Fighter->LastEvent, FLinearColor(0.7f, 0.8f, 0.9f), X + 12, 118);
}

void ADuelHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Canvas || !PlayerOwner)
	{
		return;
	}

	ADuelGameMode* GameMode = GetWorld()->GetAuthGameMode<ADuelGameMode>();
	AVampsDuelCharacter* Player = Cast<AVampsDuelCharacter>(PlayerOwner->GetPawn());
	const float Width = FMath::Min(380.f, Canvas->SizeX * 0.43f);
	DrawFighter(Player, TEXT("PLAYER"), 20, Width);

	if (GameMode)
	{
		DrawFighter(GameMode->Opponent, TEXT("OPPONENT"), Canvas->SizeX - Width - 20, Width);
	}

	DrawText(TEXT("WASD move | Mouse free camera | Click attack/guard | Tab lock | R reset | F1 AI mode"),
	         FLinearColor::White,
	         20,
	         Canvas->SizeY - 50);

	if (GameMode && GameMode->Opponent)
	{
		if (ADuelAIController* AI = Cast<ADuelAIController>(GameMode->Opponent->GetController()))
		{
			DrawText(TEXT("AI: ") + StaticEnum<EDuelAIMode>()->GetNameStringByValue(int64(AI->Mode)),
			         FLinearColor::White,
			         20,
			         Canvas->SizeY - 75);
		}

		if (Player && (Player->IsDefeated() || GameMode->Opponent->IsDefeated()))
		{
			DrawText(Player->IsDefeated() ? TEXT("DEFEAT - R to reset") : TEXT("VICTORY - R to reset"),
			         FLinearColor::Yellow,
			         Canvas->SizeX * 0.35f,
			         Canvas->SizeY * 0.4f,
			         nullptr,
			         1.8f);
		}
	}

	if (const ADuelPlayerController* DuelController = Cast<ADuelPlayerController>(PlayerOwner))
	{
		if (!DuelController->DuelMapping)
		{
			DrawText(TEXT("Assign IMC_Duel in BP_DuelPlayerController"), FLinearColor::Red, 20, 165);
		}
	}
}
