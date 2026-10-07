#include "Player/DuelPlayerController.h"
#include "AI/DuelAIController.h"
#include "Characters/VampsDuelCharacter.h"
#include "GameModes/DuelGameMode.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "InputCoreTypes.h"
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

void ADuelPlayerController::EndPlay(const EEndPlayReason::Type Reason)
{
	if (ULocalPlayer* Local = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Input = Local->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (CombatMapping)
			{
				Input->RemoveMappingContext(CombatMapping);
			}
			if (DuelMapping)
			{
				Input->RemoveMappingContext(DuelMapping);
			}
		}
	}
	Super::EndPlay(Reason);
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
	}

	if (LookAction)
	{
		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADuelPlayerController::Look);
	}

	if (ULocalPlayer* Local = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = Local->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (CombatMapping)
			{
				Subsystem->RemoveMappingContext(CombatMapping);
			}
		}
	}
	AttackAction = NewObject<UInputAction>(this);
	GuardAction = NewObject<UInputAction>(this);
	AIModeAction = NewObject<UInputAction>(this);
	CombatMapping = NewObject<UInputMappingContext>(this);
	AttackAction->ValueType = EInputActionValueType::Boolean;
	GuardAction->ValueType = EInputActionValueType::Boolean;
	AIModeAction->ValueType = EInputActionValueType::Boolean;
	CombatMapping->MapKey(AttackAction, EKeys::LeftMouseButton);
	CombatMapping->MapKey(GuardAction, EKeys::RightMouseButton);
	CombatMapping->MapKey(AIModeAction, EKeys::F1);
	if (ULocalPlayer* Local = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = Local->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->AddMappingContext(CombatMapping, 1);
		}
	}
	Input->BindAction(AttackAction, ETriggerEvent::Started, this, &ADuelPlayerController::Attack);
	Input->BindAction(GuardAction, ETriggerEvent::Started, this, &ADuelPlayerController::Guard);
	Input->BindAction(GuardAction, ETriggerEvent::Completed, this, &ADuelPlayerController::StopGuard);
	Input->BindAction(GuardAction, ETriggerEvent::Canceled, this, &ADuelPlayerController::StopGuard);
	Input->BindAction(AIModeAction, ETriggerEvent::Started, this, &ADuelPlayerController::CycleAI);

	if (LockAction)
	{
		Input->BindAction(LockAction, ETriggerEvent::Started, this, &ADuelPlayerController::ToggleLock);
	}

	if (ResetAction)
	{
		Input->BindAction(ResetAction, ETriggerEvent::Started, this, &ADuelPlayerController::ResetDuel);
	}
}

void ADuelPlayerController::Move(const FInputActionValue& Value)
{
	if (AVampsDuelCharacter* Fighter = Cast<AVampsDuelCharacter>(GetPawn()))
	{
		const FVector2D Axes = Value.Get<FVector2D>();
		const FRotator Yaw(0, GetControlRotation().Yaw, 0);
		Fighter->RequestMove(Yaw.Vector() * Axes.Y + FRotationMatrix(Yaw).GetUnitAxis(EAxis::Y) * Axes.X);
	}
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

void ADuelPlayerController::Attack()
{
	if (AVampsDuelCharacter* Fighter = Cast<AVampsDuelCharacter>(GetPawn()))
	{
		Fighter->RequestAttack();
	}
}

void ADuelPlayerController::Guard()
{
	if (AVampsDuelCharacter* Fighter = Cast<AVampsDuelCharacter>(GetPawn()))
	{
		Fighter->RequestGuard(true);
	}
}

void ADuelPlayerController::StopGuard()
{
	if (AVampsDuelCharacter* Fighter = Cast<AVampsDuelCharacter>(GetPawn()))
	{
		Fighter->RequestGuard(false);
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
}

void ADuelPlayerController::ResetDuel()
{
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
	ADuelGameMode* GameMode = GetWorld()->GetAuthGameMode<ADuelGameMode>();
	if (GameMode && GameMode->Opponent)
	{
		if (ADuelAIController* AI = Cast<ADuelAIController>(GameMode->Opponent->GetController()))
		{
			AI->CycleMode();
		}
	}
}
