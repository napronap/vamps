#include "AI/DuelAIController.h"
#include "Characters/VampsDuelCharacter.h"
#include "Combat/Abilities/GA_Attack.h"
#include "Combat/Abilities/GA_Guard.h"
#include "Combat/Abilities/GA_HitReaction.h"

void ADuelAIController::ResetSequence(double Now)
{
	Epoch = Now;
	NextDecision = Now + 0.75;
	NextPress = NextDecision;
}

void ADuelAIController::CycleMode()
{
	Mode = EDuelAIMode((int32(Mode) + 1) % 5);

	if (AVampsDuelCharacter* Fighter = Cast<AVampsDuelCharacter>(GetPawn()))
	{
		Fighter->SetMoveIntent(FVector::ZeroVector);
		ResetSequence(Fighter->CombatTime());
	}
}

void ADuelAIController::Advance(double Now)
{
	AVampsDuelCharacter* ControlledFighter = Cast<AVampsDuelCharacter>(GetPawn());

	if (!ControlledFighter || !ControlledFighter->Attack || Now < NextDecision)
	{
		return;
	}

	NextDecision = Now + 0.03;
	ControlledFighter->SetMoveIntent(FVector::ZeroVector);
	AVampsDuelCharacter* Opponent = ControlledFighter->GetTarget();

	if (ControlledFighter->IsDefeated() || !Opponent || Mode == EDuelAIMode::Passive)
	{
		return;
	}

	const FVector Offset = Opponent->GetActorLocation() - ControlledFighter->GetActorLocation();
	const float Distance = Offset.Size2D();
	EDuelAIMode Current = Mode;

	if (Mode == EDuelAIMode::Mixed)
	{
		Current = EDuelAIMode(1 + (int32((Now - Epoch) / 4.) % 3));
	}

	if (Current == EDuelAIMode::Sidestep)
	{
		if (Opponent->Attack->IsActive())
		{
			ControlledFighter->SetMoveIntent(FVector::CrossProduct(FVector::UpVector, Offset.GetSafeNormal2D()));
		}

		return;
	}

	if (Current == EDuelAIMode::Combo && Distance > 170.f && !ControlledFighter->Attack->IsActive())
	{
		ControlledFighter->SetMoveIntent(Offset.GetSafeNormal2D());
	}

	if (Now < NextPress)
	{
		return;
	}

	bool bPress = false;

	if (Current == EDuelAIMode::Combo)
	{
		bPress = Distance <= 250.f && ControlledFighter->GetPhase() != EDuelPhase::NA;
	}
	else if (Opponent->Attack->IsActive())
	{
		const double UntilContact = Opponent->Attack->ActiveAt() - Now;
		bPress = (UntilContact >= 0 && UntilContact <= 0.05) || ControlledFighter->Guard->IsActive() ||
		         ControlledFighter->HitReaction->CanRespond(Now);
	}

	if (bPress)
	{
		ControlledFighter->PressShared();
		NextPress = Now + 0.12;
	}
}
