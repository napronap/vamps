#include "GameModes/VampsCombatTestGameMode.h"
#include "AbilitySystemComponent.h"
#include "Characters/VampsCharacter.h"
#include "Characters/VampsCharacter_Player.h"
#include "Combat/Attributes/VampsAttributeSet.h"
#include "Combat/VampsCombatLog.h"
#include "Components/CapsuleComponent.h"
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

	if (!OpponentClass || OpponentClass->HasAnyClassFlags(CLASS_Abstract) || !OpponentControllerClass ||
	    OpponentControllerClass->HasAnyClassFlags(CLASS_Abstract))
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: opponent initialization requires concrete character and AI classes; character='%s', "
		            "AI='%s'."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(OpponentClass.Get()),
		       *GetPathNameSafe(OpponentControllerClass.Get()));
		return;
	}

	UWorld& CombatWorld = *GetWorld();
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	Opponent = CombatWorld.SpawnActor<AVampsCharacter>(OpponentClass, OpponentSpawn, SpawnParameters);

	if (!IsValid(Opponent))
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: failed to spawn opponent class '%s'."),
		       __FUNCTION__,
		       *GetPathName(),
		       *OpponentClass->GetPathName());
		return;
	}

	OpponentAI = CombatWorld.SpawnActor<AVampsAIController>(OpponentControllerClass);

	if (!IsValid(OpponentAI))
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: failed to spawn opponent AI class '%s'; opponent creation cancelled."),
		       __FUNCTION__,
		       *GetPathName(),
		       *OpponentControllerClass->GetPathName());
		Opponent->Destroy();
		Opponent = nullptr;
		return;
	}

	OpponentAI->Mode = InitialAIMode;
	OpponentAI->Possess(Opponent);
	ResetCombat();
}

void AVampsCombatTestGameMode::CycleAIMode()
{
	if (!IsValid(OpponentAI))
	{
		return;
	}

	const EVampsAIMode CurrentMode = OpponentAI->Mode;
	if (CurrentMode == EVampsAIMode::Mixed)
	{
		OpponentAI->SetMode(EVampsAIMode::Passive);
		return;
	}

	if (CurrentMode < EVampsAIMode::Mixed)
	{
		OpponentAI->SetMode(static_cast<EVampsAIMode>(static_cast<uint8>(CurrentMode) + 1));
	}
}

void AVampsCombatTestGameMode::ResetCombat()
{
	if (!IsValid(Opponent) || !IsValid(OpponentAI))
	{
		return;
	}

	APlayerController* TestPlayerController = GetWorld()->GetFirstPlayerController();
	if (!TestPlayerController)
	{
		return;
	}

	AVampsCharacter* PlayerCharacter = TestPlayerController->GetPawn<AVampsCharacter>();
	if (!IsValid(PlayerCharacter))
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: reset requires a VampsCharacter player pawn; controller='%s', pawn='%s'."),
		       __FUNCTION__,
		       *GetPathName(),
		       *TestPlayerController->GetPathName(),
		       *GetPathNameSafe(TestPlayerController->GetPawn()));
		return;
	}

	for (AVampsCharacter* Fighter : {PlayerCharacter, Opponent.Get()})
	{
		const bool bWasDead = !Fighter->CombatState.bIsAlive;
		const FTransform& SpawnTransform = Fighter == PlayerCharacter ? PlayerSpawn : OpponentSpawn;
		UCharacterMovementComponent* Movement = Fighter->GetCharacterMovement();

		Fighter->ResetCombatState();
		Fighter->AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetHealthAttribute(),
		                                                Fighter->Attributes->GetMaxHealth());
		Fighter->SetActorTransform(SpawnTransform, false, nullptr, ETeleportType::TeleportPhysics);

		if (bWasDead)
		{
			Fighter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			Movement->SetMovementMode(MOVE_Walking);
		}

		Fighter->ConsumeMovementInputVector();
		Movement->StopMovementImmediately();
	}

	TestPlayerController->SetControlRotation(PlayerSpawn.Rotator());
	OpponentAI->SetFocus(PlayerCharacter);
	OpponentAI->SetMode(OpponentAI->Mode);
}
