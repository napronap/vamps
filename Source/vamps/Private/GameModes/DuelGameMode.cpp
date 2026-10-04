#include "GameModes/DuelGameMode.h"
#include "Characters/VampsDuelCharacter.h"
#include "Player/DuelPlayerController.h"
#include "AI/DuelAIController.h"
#include "UI/DuelHUD.h"
#include "Engine/World.h"

ADuelGameMode::ADuelGameMode()
{
	DefaultPawnClass = AVampsDuelCharacter::StaticClass();
	OpponentClass = AVampsDuelCharacter::StaticClass();
	PlayerControllerClass = ADuelPlayerController::StaticClass();
	HUDClass = ADuelHUD::StaticClass();
	PlayerSpawn = FTransform(FRotator::ZeroRotator, FVector(-220, 0, 100));
	OpponentSpawn = FTransform(FRotator(0, 180, 0), FVector(220, 0, 100));
}

void ADuelGameMode::StartPlay()
{
	Super::StartPlay();
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	Opponent = GetWorld()->SpawnActor<AVampsDuelCharacter>(OpponentClass, OpponentSpawn, SpawnParameters);
	ResetDuel();
}

void ADuelGameMode::ResetDuel()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	AVampsDuelCharacter* Player = PlayerController ? Cast<AVampsDuelCharacter>(PlayerController->GetPawn()) : nullptr;

	if (!Player || !Opponent)
	{
		return;
	}

	for (AVampsDuelCharacter* Fighter : {Player, Opponent.Get()})
	{
		if (SharedConfig)
		{
			Fighter->CombatConfig = SharedConfig;
		}

		Fighter->ResetFighter(Fighter == Player ? PlayerSpawn : OpponentSpawn);
	}

	Player->SetTarget(Opponent);
	Opponent->SetTarget(Player);

	if (ADuelAIController* AI = Cast<ADuelAIController>(Opponent->GetController()))
	{
		AI->ResetSequence(Opponent->CombatTime());
	}

	PlayerController->SetControlRotation(FRotator(-18, 0, 0));
}
