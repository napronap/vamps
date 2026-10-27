#include "Player/VampsPlayerController.h"
#include "Player/VampsPlayerCameraManager.h"
#include "Characters/VampsCharacter.h"
#include "Characters/VampsLockOnComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedPlayerInput.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Combat/VampsCombatLog.h"
#include "InputCoreTypes.h"
#include "UI/VampsCombatTestHUD.h"

AVampsPlayerController::AVampsPlayerController()
{
	PlayerCameraManagerClass = AVampsPlayerCameraManager::StaticClass();
}

void AVampsPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (!IsLocalPlayerController())
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
	    ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!DefaultInputMappingContext || !InputSubsystem)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: input setup requires DefaultInputMappingContext and EnhancedInputLocalPlayerSubsystem; "
		            "mapping='%s', subsystem='%s'."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(DefaultInputMappingContext),
		       *GetPathNameSafe(InputSubsystem));
		return;
	}
	InputSubsystem->AddMappingContext(DefaultInputMappingContext, 0);
	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;
}

void AVampsPlayerController::EndPlay(const EEndPlayReason::Type Reason)
{
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
		    LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if (InputSubsystem)
		{
			InputSubsystem->RemoveMappingContext(DefaultInputMappingContext);
		}
		else
		{
			UE_LOG(LogVampsCombat,
			       Warning,
			       TEXT("%hs: %s: cannot remove input mapping '%s'; EnhancedInput subsystem is unavailable during "
			            "shutdown."),
			       __FUNCTION__,
			       *GetPathName(),
			       *GetPathNameSafe(DefaultInputMappingContext));
		}
	}
	Super::EndPlay(Reason);
}

void AVampsPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent);
	if (!Input || !MoveAction || !CombatAction || !CycleTargetAction || !MobilityAction)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: input bindings require EnhancedInputComponent and all actions; component='%s', "
		            "Move='%s', Combat='%s', CycleTarget='%s', Mobility='%s'."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(InputComponent),
		       *GetPathNameSafe(MoveAction),
		       *GetPathNameSafe(CombatAction),
		       *GetPathNameSafe(CycleTargetAction),
		       *GetPathNameSafe(MobilityAction));
		return;
	}
	if (MoveAction->ValueType != EInputActionValueType::Axis2D ||
	    CombatAction->ValueType != EInputActionValueType::Boolean ||
	    CycleTargetAction->ValueType != EInputActionValueType::Boolean ||
	    MobilityAction->ValueType != EInputActionValueType::Boolean)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: %s: input action types are invalid; '%s'=%d (expected Axis2D), '%s'=%d and '%s'=%d (expected "
		            "Boolean)."),
		       __FUNCTION__,
		       *GetPathName(),
		       *MoveAction->GetPathName(),
		       int32(MoveAction->ValueType),
		       *CombatAction->GetPathName(),
		       int32(CombatAction->ValueType),
		       *CycleTargetAction->GetPathName(),
		       int32(CycleTargetAction->ValueType));
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: Mobility='%s' has type %d; expected Boolean."),
		       __FUNCTION__,
		       *MobilityAction->GetPathName(),
		       int32(MobilityAction->ValueType));
		return;
	}

	Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AVampsPlayerController::Move);
	Input->BindAction(MoveAction, ETriggerEvent::Completed, this, &AVampsPlayerController::Move);
	Input->BindAction(MoveAction, ETriggerEvent::Canceled, this, &AVampsPlayerController::Move);
	Input->BindAction(CombatAction, ETriggerEvent::Started, this, &AVampsPlayerController::RequestCombat);
	Input->BindAction(CycleTargetAction, ETriggerEvent::Started, this, &AVampsPlayerController::CycleTarget);
	Input->BindAction(MobilityAction, ETriggerEvent::Started, this, &AVampsPlayerController::MobilityPressed);
	Input->BindAction(MobilityAction, ETriggerEvent::Completed, this, &AVampsPlayerController::MobilityReleased);
	Input->BindAction(MobilityAction, ETriggerEvent::Canceled, this, &AVampsPlayerController::MobilityReleased);
#if !UE_BUILD_SHIPPING
	Input->BindDebugKey(FInputChord(EKeys::F9), IE_Pressed, this, &AVampsPlayerController::ToggleCombatHUD, false);
#endif
}

#if !UE_BUILD_SHIPPING
void AVampsPlayerController::ToggleCombatHUD(FKey, FInputActionValue)
{
	AVampsCombatTestHUD* CombatHUD = Cast<AVampsCombatTestHUD>(GetHUD());
	if (!CombatHUD)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': HUD toggle requires VampsCombatTestHUD; HUD='%s'."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(GetHUD()));
		return;
	}
	CombatHUD->ToggleCombatHUD();
}
#endif

void AVampsPlayerController::Move(const FInputActionValue& Value)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		if (Value.GetValueType() != EInputActionValueType::Axis2D)
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: %s: movement input has type %d; expected Axis2D."),
			       __FUNCTION__,
			       *GetPathName(),
			       int32(Value.GetValueType()));
			return;
		}
		const FVector2D Movement = Value.Get<FVector2D>();
		const FRotator Yaw(0.f, GetControlRotation().Yaw, 0.f);
		if (AVampsCharacter* ControlledFighter = Cast<AVampsCharacter>(ControlledPawn))
		{
			ControlledFighter->SetMobilityInput(!Movement.IsNearlyZero());
		}
		ControlledPawn->AddMovementInput(Yaw.Vector(), Movement.Y);
		ControlledPawn->AddMovementInput(FRotationMatrix(Yaw).GetUnitAxis(EAxis::Y), Movement.X);
	}
}

void AVampsPlayerController::RequestCombat()
{
	if (APawn* ControlledPawn = GetPawn())
	{
		AVampsCharacter* ControlledFighter = Cast<AVampsCharacter>(ControlledPawn);
		if (!ControlledFighter)
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: %s: combat input requires a VampsCharacter pawn; got '%s'."),
			       __FUNCTION__,
			       *GetPathName(),
			       *ControlledPawn->GetPathName());
			return;
		}
		ControlledFighter->RequestCombatAction();
	}
}

void AVampsPlayerController::CycleTarget()
{
	if (APawn* ControlledPawn = GetPawn())
	{
		AVampsCharacter* ControlledFighter = Cast<AVampsCharacter>(ControlledPawn);
		if (!ControlledFighter || !ControlledFighter->LockOn)
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: %s: target input requires a VampsCharacter pawn with LockOn; got '%s'."),
			       __FUNCTION__,
			       *GetPathName(),
			       *ControlledPawn->GetPathName());
			return;
		}
		ControlledFighter->LockOn->CycleTarget();
	}
}

void AVampsPlayerController::MobilityPressed()
{
	AVampsCharacter* ControlledFighter = Cast<AVampsCharacter>(GetPawn());
	const UEnhancedPlayerInput* EnhancedPlayerInput = Cast<UEnhancedPlayerInput>(PlayerInput);
	if (!ControlledFighter || !EnhancedPlayerInput || !MoveAction)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': mobility requires VampsCharacter, EnhancedInput and MoveAction."),
		       __FUNCTION__,
		       *GetPathName());
		return;
	}
	const FVector2D Direction = EnhancedPlayerInput->GetActionValue(MoveAction).Get<FVector2D>();
	const FRotator Yaw(0.f, GetControlRotation().Yaw, 0.f);
	const FVector DashDirection = Yaw.Vector() * Direction.Y + FRotationMatrix(Yaw).GetUnitAxis(EAxis::Y) * Direction.X;
	ControlledFighter->SetMobilityInput(!Direction.IsNearlyZero());
	ControlledFighter->SetMobilityHeld(true, DashDirection);
}

void AVampsPlayerController::MobilityReleased()
{
	if (AVampsCharacter* ControlledFighter = Cast<AVampsCharacter>(GetPawn()))
	{
		ControlledFighter->SetMobilityHeld(false);
	}
}
