#include "Player/VampsMatchPlayerController.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Combat/VampsCombatLog.h"
#include "EnhancedPlayerInput.h"
#include "Engine/World.h"
#include "GameModes/VampsMatchGameMode.h"

void AVampsMatchPlayerController::BeginRespawnCamera()
{
	if (RespawnCamera)
	{
		return;
	}
	FVector EyeLocation;
	FRotator EyeRotation;
	GetPlayerViewPoint(EyeLocation, EyeRotation);
	RespawnCamera = GetWorld()->SpawnActor<ACameraActor>(EyeLocation, EyeRotation);
	if (!RespawnCamera)
	{
		UE_LOG(
		    LogVampsCombat, Error, TEXT("%hs: '%s': failed to create respawn camera."), __FUNCTION__, *GetPathName());
		return;
	}
	SetViewTarget(RespawnCamera);
}

void AVampsMatchPlayerController::FinishRespawnCamera()
{
	if (RespawnCamera)
	{
		SetViewTarget(GetPawn());
		RespawnCamera->Destroy();
		RespawnCamera = nullptr;
	}
}

void AVampsMatchPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	const AVampsMatchGameMode* MatchMode = Cast<AVampsMatchGameMode>(GetWorld()->GetAuthGameMode());
	if (!RespawnCamera || !MatchMode || MatchMode->RoundPhase != EVampsMatchPhase::Active)
	{
		return;
	}
	const UEnhancedPlayerInput* MatchInput = Cast<UEnhancedPlayerInput>(PlayerInput);
	if (!MatchInput || !MoveAction)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': free camera requires EnhancedPlayerInput and MoveAction."),
		       __FUNCTION__,
		       *GetPathName());
		return;
	}
	float MouseHorizontal = 0.f;
	float MouseVertical = 0.f;
	GetInputMouseDelta(MouseHorizontal, MouseVertical);
	FRotator FreeRotation = RespawnCamera->GetActorRotation();
	FreeRotation.Yaw += MouseHorizontal * 1.5f;
	FreeRotation.Pitch = FMath::Clamp(FreeRotation.Pitch + MouseVertical * 1.5f, -85.f, 85.f);
	const FVector2D TravelInput = MatchInput->GetActionValue(MoveAction).Get<FVector2D>();
	const FVector TravelDirection =
	    FreeRotation.Vector() * TravelInput.Y + FRotationMatrix(FreeRotation).GetUnitAxis(EAxis::Y) * TravelInput.X;
	RespawnCamera->SetActorLocationAndRotation(
	    RespawnCamera->GetActorLocation() + TravelDirection.GetClampedToMaxSize(1.f) * 800.f * DeltaTime, FreeRotation);
}

void AVampsMatchPlayerController::EndPlay(const EEndPlayReason::Type Reason)
{
	FinishRespawnCamera();
	Super::EndPlay(Reason);
}
