#include "UI/VampsCombatTestHUD.h"
#include "AI/VampsAIController.h"
#include "Characters/VampsCharacter.h"
#include "Combat/Attributes/VampsAttributeSet.h"
#include "Engine/Canvas.h"
#include "EngineUtils.h"
#include "GameModes/VampsCombatTestGameMode.h"

void AVampsCombatTestHUD::DrawFighter(AVampsCharacter* Fighter, const FString& Label, float X, float Y, float Width)
{
	if (!Fighter)
	{
		return;
	}

	const float Health = Fighter->Attributes->GetHealth();
	const float MaxHealth = Fighter->Attributes->GetMaxHealth();
	DrawRect(FLinearColor(0.025f, 0.025f, 0.04f, 0.86f), X, Y, Width, 90);
	DrawText(FString::Printf(TEXT("%s   %.0f / %.0f"), *Label, Health, MaxHealth), FLinearColor::White, X + 12, Y + 9);
	DrawRect(FLinearColor(0.15f, 0.15f, 0.18f), X + 12, Y + 32, Width - 24, 10);
	DrawRect(FLinearColor(0.25f, 0.8f, 0.65f),
	         X + 12,
	         Y + 32,
	         (Width - 24) * FMath::Clamp(Health / MaxHealth, 0.f, 1.f),
	         10);

	const FString State = Fighter->IsGuarding()    ? TEXT("GUARD")
	                      : Fighter->IsAttacking() ? TEXT("ATTACK")
	                                               : TEXT("IDLE");
	DrawText(State, FLinearColor::White, X + 12, Y + 52);
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
	DrawFighter(PlayerCharacter, TEXT("PLAYER"), 20, 20, Width);
	float TargetY = 20;
	for (TActorIterator<AVampsCharacter> FighterIterator(GetWorld()); FighterIterator; ++FighterIterator)
	{
		AVampsCharacter* Fighter = *FighterIterator;
		if (Fighter == PlayerCharacter)
		{
			continue;
		}
		DrawFighter(Fighter, Fighter->GetName(), Canvas->SizeX - Width - 20, TargetY, Width);
		TargetY += 110;
	}
	const AVampsCombatTestGameMode* TestGameMode = CastChecked<AVampsCombatTestGameMode>(GetWorld()->GetAuthGameMode());
	check(TestGameMode->OpponentAI);
	const FString ModeName = StaticEnum<EVampsAIMode>()->GetNameStringByValue(int64(TestGameMode->OpponentAI->Mode));
	DrawText(TEXT("AI: ") + ModeName + TEXT(" | F6 next mode | R reset"), FLinearColor::White, 20, Canvas->SizeY - 75);
	DrawText(TEXT("WASD move | Mouse look | LMB attack | Hold RMB guard"), FLinearColor::White, 20, Canvas->SizeY - 50);
}
