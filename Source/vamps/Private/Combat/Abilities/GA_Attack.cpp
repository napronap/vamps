#include "Combat/Abilities/GA_Attack.h"
#include "Combat/CombatTags.h"
#include "Characters/VampsDuelCharacter.h"
#include "Combat/DuelCombatSubsystem.h"

UGA_Attack::UGA_Attack()
{
	ActivationOwnedTags.AddTag(DuelTags::Attack);
}

double UGA_Attack::ActiveAt() const
{
	return StartedAt + (StrikeIndex == 0 ? FMath::Max(0.f, Fighter()->Config().InitialParry) : 0.f) +
	       FMath::Max(0.01f, Fighter()->Config().Strike(StrikeIndex).Buildup);
}

double UGA_Attack::RecoveryAt() const
{
	return ActiveAt() + FMath::Max(0.01f, Fighter()->Config().Strike(StrikeIndex).Active);
}

double UGA_Attack::EndAt() const
{
	return RecoveryAt() + FMath::Max(0.01f, Fighter()->Config().Strike(StrikeIndex).Continuation);
}

EDuelPhase UGA_Attack::Phase(double Now) const
{
	if (!IsActive())
	{
		return EDuelPhase::Default;
	}

	if (StrikeIndex == 0 && Now < StartedAt + Fighter()->Config().InitialParry)
	{
		return EDuelPhase::Parry;
	}

	if (Now < ActiveAt())
	{
		return EDuelPhase::Buildup;
	}

	if (Now < RecoveryAt())
	{
		return EDuelPhase::Active;
	}

	return EDuelPhase::Recovery;
}

double UGA_Attack::NextBoundary(double Now) const
{
	const double Boundaries[] = {StartedAt + Fighter()->Config().InitialParry, ActiveAt(), RecoveryAt(), EndAt()};

	for (double Boundary : Boundaries)
	{
		if (Boundary > Now + 1.e-8)
		{
			return Boundary;
		}
	}

	return TNumericLimits<double>::Max();
}

void UGA_Attack::Start(double Now)
{
	StrikeIndex = 0;
	AVampsDuelCharacter* Owner = Fighter();
	Direction = Owner->GetActorForwardVector();
	ApproachDistance = ApproachApplied = 0;

	if (AVampsDuelCharacter* Target = Owner->GetTarget())
	{
		const FVector Offset = Target->GetActorLocation() - Owner->GetActorLocation();
		const float Delta = FMath::FindDeltaAngleDegrees(Direction.Rotation().Yaw, Offset.Rotation().Yaw);
		const float Yaw = Direction.Rotation().Yaw +
		                  FMath::Clamp(Delta, -Owner->Config().InitialTurnLimit, Owner->Config().InitialTurnLimit);
		Direction = FRotator(0, Yaw, 0).Vector();
		ApproachDistance = FMath::Min(FMath::Max(0.f, Owner->Config().InitialApproach),
		                              FMath::Max(0.f, FVector::DotProduct(Offset, Direction) - 130.f));
	}

	Owner->SetActorRotation(Direction.Rotation());
	StartStrike(Now);
}

void UGA_Attack::StartStrike(double Now)
{
	StartedAt = Now;
	HitActors.Reset();
	Serial = Fighter()->GetWorld()->GetSubsystem<UDuelCombatSubsystem>()->NextSerial();
	PresentedPhase = EDuelPhase::Buildup;
	const FDuelStrike& Strike = Fighter()->Config().Strike(StrikeIndex);
	Fighter()->PlayAttackSection(Strike, Strike.PrepareSection, ActiveAt() - Now);
}

void UGA_Attack::Advance(double Now)
{
	if (!IsActive())
	{
		return;
	}

	if (StrikeIndex == 0 && ApproachApplied < ApproachDistance)
	{
		const float Desired =
		    ApproachDistance * FMath::Clamp(float((Now - StartedAt) / (ActiveAt() - StartedAt)), 0.f, 1.f);
		Fighter()->MoveSwept(Direction * (Desired - ApproachApplied));
		ApproachApplied = Desired;
	}

	const EDuelPhase Current = Phase(Now);

	if (Current != PresentedPhase && (Current == EDuelPhase::Active || Current == EDuelPhase::Recovery))
	{
		const FDuelStrike& Strike = Fighter()->Config().Strike(StrikeIndex);
		Fighter()->PlayAttackSection(Strike,
		                             Current == EDuelPhase::Active ? Strike.ActiveSection : Strike.RecoverySection,
		                             Current == EDuelPhase::Active ? Strike.Active : Strike.Continuation);
		PresentedPhase = Current;
	}

	if ((StrikeIndex == 2 && Now >= RecoveryAt()) || Now >= EndAt())
	{
		Leave(Now);
	}
}

bool UGA_Attack::Continue(double Now)
{
	if (!IsActive() || StrikeIndex >= 2 || Phase(Now) != EDuelPhase::Recovery || Now >= EndAt())
	{
		return false;
	}
	++StrikeIndex;
	StartStrike(Now);
	return true;
}

void UGA_Attack::Leave(double Now)
{
	if (!IsActive())
	{
		return;
	}

	Finish();
	Fighter()->ClearInput();
	Fighter()->StartNA(Now, true);
}
