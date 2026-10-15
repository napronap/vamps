#include "GameModes/VampsCombatTestGameMode.h"
#include "AbilitySystemComponent.h"
#include "Characters/VampsCharacter.h"
#include "Characters/VampsCharacter_Player.h"
#include "Combat/Attributes/VampsAttributeSet.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/VampsCombatTestPlayerController.h"
#include "UI/VampsCombatTestHUD.h"

AVampsCombatTestGameMode::AVampsCombatTestGameMode()
{
	DefaultPawnClass = AVampsCharacter_Player::StaticClass();
	PlayerControllerClass = AVampsCombatTestPlayerController::StaticClass();
	HUDClass = AVampsCombatTestHUD::StaticClass();
	OpponentControllerClass = AVampsAIController::StaticClass();
	PlayerSpawn = FTransform(FRotator::ZeroRotator, FVector(-220.f, 0.f, 100.f));
	OpponentSpawn = FTransform(FRotator(0.f, 180.f, 0.f), FVector(220.f, 0.f, 100.f));
}

void AVampsCombatTestGameMode::StartPlay()
{
	Super::StartPlay();
	checkf(OpponentClass && !OpponentClass->HasAnyClassFlags(CLASS_Abstract),
	       TEXT("OpponentClass must be a concrete VampsCharacter class."));
	check(OpponentControllerClass);
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	Opponent = GetWorld()->SpawnActor<AVampsCharacter>(OpponentClass, OpponentSpawn, SpawnParameters);
	check(Opponent);
	OpponentAI = GetWorld()->SpawnActor<AVampsAIController>(OpponentControllerClass);
	check(OpponentAI);
	OpponentAI->Mode = InitialAIMode;
	OpponentAI->Possess(Opponent);
	ResetCombat();
}

void AVampsCombatTestGameMode::CycleAIMode()
{
	check(OpponentAI);
	OpponentAI->SetMode(EVampsAIMode((int32(OpponentAI->Mode) + 1) % 5));
}

void AVampsCombatTestGameMode::ResetCombat()
{
	APlayerController* TestPlayerController = GetWorld()->GetFirstPlayerController();
	check(TestPlayerController && Opponent && OpponentAI);
	AVampsCharacter* PlayerCharacter = CastChecked<AVampsCharacter>(TestPlayerController->GetPawn());
	for (AVampsCharacter* Fighter : {PlayerCharacter, Opponent.Get()})
	{
		Fighter->ResetCombatState();
		Fighter->AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetHealthAttribute(),
		                                                Fighter->Attributes->GetMaxHealth());
		Fighter->SetActorTransform(
		    Fighter == PlayerCharacter ? PlayerSpawn : OpponentSpawn, false, nullptr, ETeleportType::TeleportPhysics);
		Fighter->ConsumeMovementInputVector();
		Fighter->GetCharacterMovement()->StopMovementImmediately();
	}
	TestPlayerController->SetControlRotation(PlayerSpawn.Rotator());
	OpponentAI->SetFocus(PlayerCharacter);
	OpponentAI->SetMode(OpponentAI->Mode);
}
