#include "Player/DuelPlayerController.h"
#include "Characters/VampsDuelCharacter.h"
#include "GameModes/DuelGameMode.h"
#include "AI/DuelAIController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"

ADuelPlayerController::ADuelPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADuelPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* Local = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Input = Local->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (DuelMapping)
			{
				Input->AddMappingContext(DuelMapping, 0);
			}
		}
	}

	SetControlRotation(FRotator(-18, 0, 0));
	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;
}

void ADuelPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent);

	if (!Input)
	{
		UE_LOG(LogTemp, Error, TEXT("Duel requires EnhancedInputComponent in Input settings."));
		return;
	}

	if (MoveAction)
	{
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADuelPlayerController::Move);
		Input->BindAction(MoveAction, ETriggerEvent::Completed, this, &ADuelPlayerController::StopMove);
		Input->BindAction(MoveAction, ETriggerEvent::Canceled, this, &ADuelPlayerController::StopMove);
	}

	if (LookAction)
	{
		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADuelPlayerController::Look);
	}

	if (SharedAction)
	{
		Input->BindAction(SharedAction, ETriggerEvent::Started, this, &ADuelPlayerController::Shared);
	}

	if (LockAction)
	{
		Input->BindAction(LockAction, ETriggerEvent::Started, this, &ADuelPlayerController::ToggleLock);
	}

	if (ResetAction)
	{
		Input->BindAction(ResetAction, ETriggerEvent::Started, this, &ADuelPlayerController::ResetDuel);
	}

	if (AIModeAction)
	{
		Input->BindAction(AIModeAction, ETriggerEvent::Started, this, &ADuelPlayerController::CycleAI);
	}
}

void ADuelPlayerController::Move(const FInputActionValue& Value)
{
	MoveAxes = Value.Get<FVector2D>();
}

void ADuelPlayerController::StopMove(const FInputActionValue& Value)
{
	MoveAxes = FVector2D::ZeroVector;
}

void ADuelPlayerController::Look(const FInputActionValue& Value)
{
	AVampsDuelCharacter* Fighter = Cast<AVampsDuelCharacter>(GetPawn());

	if (!Fighter || Fighter->GetTarget())
	{
		return;
	}

	const FVector2D Delta = Value.Get<FVector2D>();
	FRotator Rotation = GetControlRotation();
	Rotation.Yaw += Delta.X;
	Rotation.Pitch = FMath::Clamp(FRotator::NormalizeAxis(Rotation.Pitch) - Delta.Y, -65.f, 25.f);
	SetControlRotation(Rotation);
}

void ADuelPlayerController::Shared()
{
	if (AVampsDuelCharacter* Fighter = Cast<AVampsDuelCharacter>(GetPawn()))
	{
		Fighter->PressShared();
	}
}

void ADuelPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	AVampsDuelCharacter* Fighter = Cast<AVampsDuelCharacter>(GetPawn());

	if (!Fighter)
	{
		return;
	}

	if (AVampsDuelCharacter* Target = Fighter->GetTarget())
	{
		FRotator Desired = (Target->GetActorLocation() - Fighter->GetActorLocation()).Rotation();
		Desired.Pitch = -18;
		Desired.Roll = 0;
		SetControlRotation(FMath::RInterpTo(GetControlRotation(), Desired, DeltaTime, 8));
	}

	const FRotator Yaw(0, GetControlRotation().Yaw, 0);
	Fighter->SetMoveIntent(Yaw.Vector() * MoveAxes.Y + FRotationMatrix(Yaw).GetUnitAxis(EAxis::Y) * MoveAxes.X);
}

void ADuelPlayerController::ResetDuel()
{
	MoveAxes = FVector2D::ZeroVector;

	if (ADuelGameMode* GameMode = GetWorld()->GetAuthGameMode<ADuelGameMode>())
	{
		GameMode->ResetDuel();
	}
}

void ADuelPlayerController::ToggleLock()
{
	AVampsDuelCharacter* Fighter = Cast<AVampsDuelCharacter>(GetPawn());
	ADuelGameMode* GameMode = GetWorld()->GetAuthGameMode<ADuelGameMode>();

	if (Fighter && GameMode)
	{
		Fighter->SetTarget(Fighter->GetTarget() ? nullptr : GameMode->Opponent);
	}
}

void ADuelPlayerController::CycleAI()
{
	if (ADuelGameMode* GameMode = GetWorld()->GetAuthGameMode<ADuelGameMode>())
	{
		if (GameMode->Opponent)
		{
			if (ADuelAIController* AI = Cast<ADuelAIController>(GameMode->Opponent->GetController()))
			{
				AI->CycleMode();
			}
		}
	}
}
