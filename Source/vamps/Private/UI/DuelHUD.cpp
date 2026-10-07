#include "UI/DuelHUD.h"
#include "AI/DuelAIController.h"
#include "Characters/VampsDuelCharacter.h"
#include "GameModes/DuelGameMode.h"
#include "Engine/Canvas.h"
#include "Engine/World.h"

void ADuelHUD::DrawFighter(AVampsDuelCharacter* Fighter, const FString& Label, float X, float Width)
{
	if (!Fighter)
	{
		return;
	}

	DrawRect(FLinearColor(0.025f, 0.025f, 0.04f, 0.86f), X, 20, Width, 90);
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

	const FString State = Fighter->IsGuarding()    ? TEXT("GUARD")
	                      : Fighter->IsAttacking() ? TEXT("ATTACK")
	                                               : TEXT("IDLE");
	DrawText(State, FLinearColor::White, X + 12, 72);
}

void ADuelHUD::DrawHUD()
{
	Super::DrawHUD();
	if (!Canvas || !PlayerOwner)
	{
		return;
	}

	const float Width = FMath::Min(380.f, Canvas->SizeX * 0.43f);
	DrawFighter(Cast<AVampsDuelCharacter>(PlayerOwner->GetPawn()), TEXT("PLAYER"), 20, Width);
	if (ADuelGameMode* GameMode = GetWorld()->GetAuthGameMode<ADuelGameMode>())
	{
		DrawFighter(GameMode->Opponent, TEXT("OPPONENT"), Canvas->SizeX - Width - 20, Width);
		if (GameMode->Opponent)
		{
			const ADuelAIController* AI = Cast<ADuelAIController>(GameMode->Opponent->GetController());
			const FString Mode =
			    AI ? StaticEnum<EDuelAIMode>()->GetNameStringByValue(int64(AI->Mode)) : TEXT("Unavailable");
			DrawText(TEXT("AI: ") + Mode, FLinearColor::White, 20, Canvas->SizeY - 75);
		}
	}
	DrawText(TEXT("WASD move | LMB attack | Hold RMB guard | Tab lock | R reset | F1 AI mode"),
	         FLinearColor::White,
	         20,
	         Canvas->SizeY - 50);
}
