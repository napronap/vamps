#include "Player/VampsPlayerController.h"
#include "Characters/VampsCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputAction.h"
#include "InputMappingContext.h"

void AVampsPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (!IsLocalPlayerController())
	{
		return;
	}

	checkf(DefaultInputMappingContext, TEXT("DefaultInputMappingContext must be assigned."));
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
	    ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(InputSubsystem);
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
		check(InputSubsystem);
		InputSubsystem->RemoveMappingContext(DefaultInputMappingContext);
	}
	Super::EndPlay(Reason);
}

void AVampsPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(InputComponent);
	checkf(MoveAction && LookAction && AttackAction && GuardAction, TEXT("All four input actions must be assigned."));
	check(MoveAction->ValueType == EInputActionValueType::Axis2D);
	check(LookAction->ValueType == EInputActionValueType::Axis2D);
	check(AttackAction->ValueType == EInputActionValueType::Boolean);
	check(GuardAction->ValueType == EInputActionValueType::Boolean);

	Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AVampsPlayerController::Move);
	Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &AVampsPlayerController::Look);
	Input->BindAction(AttackAction, ETriggerEvent::Started, this, &AVampsPlayerController::Attack);
	Input->BindAction(GuardAction, ETriggerEvent::Started, this, &AVampsPlayerController::Guard, true);
	Input->BindAction(GuardAction, ETriggerEvent::Completed, this, &AVampsPlayerController::Guard, false);
	Input->BindAction(GuardAction, ETriggerEvent::Canceled, this, &AVampsPlayerController::Guard, false);
}

void AVampsPlayerController::Move(const FInputActionValue& Value)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector2D Movement = Value.Get<FVector2D>();
		const FRotator Yaw(0.f, GetControlRotation().Yaw, 0.f);
		ControlledPawn->AddMovementInput(Yaw.Vector(), Movement.Y);
		ControlledPawn->AddMovementInput(FRotationMatrix(Yaw).GetUnitAxis(EAxis::Y), Movement.X);
	}
}

void AVampsPlayerController::Look(const FInputActionValue& Value)
{
	if (!GetPawn())
	{
		return;
	}
	const FVector2D LookInput = Value.Get<FVector2D>();
	AddYawInput(LookInput.X);
	AddPitchInput(LookInput.Y);
}

void AVampsPlayerController::Attack()
{
	if (APawn* ControlledPawn = GetPawn())
	{
		CastChecked<AVampsCharacter>(ControlledPawn)->RequestAttack();
	}
}

void AVampsPlayerController::Guard(bool bPressed)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		CastChecked<AVampsCharacter>(ControlledPawn)->RequestGuard(bPressed);
	}
}
