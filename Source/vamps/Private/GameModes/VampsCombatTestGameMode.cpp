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
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	Opponent = GetWorld()->SpawnActor<AVampsCharacter>(OpponentClass, OpponentSpawn, SpawnParameters);
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
	OpponentAI = GetWorld()->SpawnActor<AVampsAIController>(OpponentControllerClass);
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
	if (!IsValid(OpponentAI) || OpponentAI->Mode > EVampsAIMode::Mixed)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: cannot cycle AI mode; OpponentAI '%s' is missing or has an invalid mode."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(OpponentAI));
		return;
	}
	OpponentAI->SetMode(EVampsAIMode((int32(OpponentAI->Mode) + 1) % 5));
}

void AVampsCombatTestGameMode::ResetCombat()
{
	APlayerController* TestPlayerController = GetWorld()->GetFirstPlayerController();
	AVampsCharacter* PlayerCharacter =
	    TestPlayerController ? Cast<AVampsCharacter>(TestPlayerController->GetPawn()) : nullptr;
	if (!IsValid(PlayerCharacter) || !IsValid(Opponent) || !IsValid(OpponentAI))
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: reset requires a VampsCharacter player, opponent and AI; player='%s', opponent='%s', "
		            "AI='%s'."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(PlayerCharacter),
		       *GetPathNameSafe(Opponent),
		       *GetPathNameSafe(OpponentAI));
		return;
	}
	for (AVampsCharacter* Fighter : {PlayerCharacter, Opponent.Get()})
	{
		if (!Fighter->AbilitySystem || !Fighter->Attributes || !Fighter->GetCharacterMovement())
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: %s: reset cancelled; fighter '%s' requires AbilitySystem, Attributes and movement."),
			       __FUNCTION__,
			       *GetPathName(),
			       *Fighter->GetPathName());
			return;
		}
	}
	for (AVampsCharacter* Fighter : {PlayerCharacter, Opponent.Get()})
	{
		const bool bWasDead = !Fighter->CombatState.bIsAlive;
		Fighter->ResetCombatState();
		Fighter->AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetHealthAttribute(),
		                                                Fighter->Attributes->GetMaxHealth());
		Fighter->SetActorTransform(
		    Fighter == PlayerCharacter ? PlayerSpawn : OpponentSpawn, false, nullptr, ETeleportType::TeleportPhysics);
		if (bWasDead)
		{
			Fighter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			Fighter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}
		Fighter->ConsumeMovementInputVector();
		Fighter->GetCharacterMovement()->StopMovementImmediately();
	}
	TestPlayerController->SetControlRotation(PlayerSpawn.Rotator());
	OpponentAI->SetFocus(PlayerCharacter);
	OpponentAI->SetMode(OpponentAI->Mode);
}
