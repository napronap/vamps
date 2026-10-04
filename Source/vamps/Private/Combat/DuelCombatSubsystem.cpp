#include "Combat/DuelCombatSubsystem.h"
#include "Characters/VampsDuelCharacter.h"
#include "Combat/Abilities/GA_Attack.h"
#include "Combat/Abilities/GA_Guard.h"
#include "Combat/Abilities/GA_NA.h"
#include "Combat/Abilities/GA_HitReaction.h"
#include "AI/DuelAIController.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"

bool UDuelCombatSubsystem::DoesSupportWorldType(EWorldType::Type Type) const
{
	return Type == EWorldType::Game || Type == EWorldType::PIE;
}

TStatId UDuelCombatSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UDuelCombatSubsystem, STATGROUP_Tickables);
}

void UDuelCombatSubsystem::Register(AVampsDuelCharacter* Fighter)
{
	Fighters.AddUnique(Fighter);
}

void UDuelCombatSubsystem::Unregister(AVampsDuelCharacter* Fighter)
{
	Fighters.Remove(Fighter);
}

void UDuelCombatSubsystem::Synchronize()
{
	if (!bAdvancing)
	{
		AdvanceTo(GetWorld()->GetTimeSeconds());
	}
}

void UDuelCombatSubsystem::Tick(float DeltaTime)
{
	Synchronize();

	for (const auto& FighterReference : Fighters)
	{
		if (FighterReference.IsValid())
		{
			FighterReference->DrawCombat();
		}
	}
}

void UDuelCombatSubsystem::AdvanceTo(double RequestedTime)
{
	if (bAdvancing || RequestedTime <= Time)
	{
		return;
	}

	TGuardValue<bool> Advancing(bAdvancing, true);
	Fighters.RemoveAll(
	    [](const auto& FighterReference)
	    {
		    return !FighterReference.IsValid();
	    });

	if (Fighters.IsEmpty())
	{
		Time = RequestedTime;
		return;
	}
	while (Time + 1.e-8 < RequestedTime)
	{
		double NextTime = FMath::Min(RequestedTime, Time + 1. / 120.);
		auto ConsiderBoundary = [&](double BoundaryTime)
		{
			if (BoundaryTime > Time + 1.e-8)
			{
				NextTime = FMath::Min(NextTime, BoundaryTime);
			}
		};

		for (const auto& FighterReference : Fighters)
		{
			AVampsDuelCharacter* Fighter = FighterReference.Get();

			if (Fighter->Attack->IsActive())
			{
				ConsiderBoundary(Fighter->Attack->NextBoundary(Time));
			}

			if (Fighter->NA->IsActive())
			{
				ConsiderBoundary(Fighter->NA->Clock.ResumeAt);
				ConsiderBoundary(Fighter->NA->Clock.EndAt());
			}

			if (Fighter->Guard->IsActive())
			{
				ConsiderBoundary(Fighter->Guard->bAwaitingPress ? Fighter->Guard->ContinueUntil
				                                                : Fighter->Guard->ArmedUntil);
			}

			if (Fighter->HitReaction->IsActive())
			{
				ConsiderBoundary(Fighter->HitReaction->UnlockAt);
				ConsiderBoundary(Fighter->HitReaction->EndAt);
			}
		}

		Time = NextTime;

		for (const auto& FighterReference : Fighters)
		{
			FighterReference->AdvanceActions(Time);
		}

		for (const auto& FighterReference : Fighters)
		{
			if (ADuelAIController* AI = Cast<ADuelAIController>(FighterReference->GetController()))
			{
				AI->Advance(Time);
			}
		}

		ResolveContacts();
	}
}

void UDuelCombatSubsystem::ResolveContacts()
{
	struct FContact
	{
		AVampsDuelCharacter* Source;
		AVampsDuelCharacter* Target;
		uint64 Serial;
		float Distance;
	};

	TArray<FContact> Contacts;

	for (const auto& FighterReference : Fighters)
	{
		AVampsDuelCharacter* Source = FighterReference.Get();
		UGA_Attack* Attack = Source->Attack;

		if (!Attack->IsActive() || Attack->Phase(Time) != EDuelPhase::Active)
		{
			continue;
		}

		const FDuelStrike& Strike = Source->Config().Strike(Attack->StrikeIndex);
		const FVector Start = Source->GetActorLocation();
		const FVector End = Start + Attack->Direction * FMath::Max(1.f, Strike.Reach);
		TArray<FHitResult> Hits;
		FCollisionQueryParams Query(SCENE_QUERY_STAT(DuelContact), false, Source);
		GetWorld()->SweepMultiByObjectType(Hits,
		                                   Start,
		                                   End,
		                                   FQuat::Identity,
		                                   FCollisionObjectQueryParams(ECC_Pawn),
		                                   FCollisionShape::MakeSphere(FMath::Max(1.f, Strike.Radius)),
		                                   Query);

		for (const FHitResult& Hit : Hits)
		{
			AVampsDuelCharacter* Target = Cast<AVampsDuelCharacter>(Hit.GetActor());

			if (!Target || !Target->Attack || Target->IsDefeated() || Attack->HitActors.Contains(Target))
			{
				continue;
			}

			FCollisionQueryParams Occlusion(SCENE_QUERY_STAT(DuelOcclusion), false, Source);
			Occlusion.AddIgnoredActor(Target);
			FHitResult Wall;

			if (GetWorld()->LineTraceSingleByChannel(
			        Wall, Start, Target->GetActorLocation(), ECC_Visibility, Occlusion))
			{
				continue;
			}

			Contacts.Add({Source, Target, Attack->Serial, Hit.Distance});
		}
	}
	// Contacts are sampled on one clock. Exact ties use the action serial, with no tie window.
	Contacts.Sort(
	    [](const FContact& A, const FContact& B)
	    {
		    if (A.Serial != B.Serial)
		    {
			    return A.Serial < B.Serial;
		    }
		    if (A.Distance != B.Distance)
		    {
			    return A.Distance < B.Distance;
		    }
		    return A.Target->GetUniqueID() < B.Target->GetUniqueID();
	    });

	for (const FContact& Contact : Contacts)
	{
		UGA_Attack* Attack = Contact.Source->Attack;

		if (!Attack->IsActive() || Attack->Serial != Contact.Serial || Attack->HitActors.Contains(Contact.Target) ||
		    Contact.Source->IsDefeated())
		{
			continue;
		}

		Attack->HitActors.Add(Contact.Target);
		const EDuelResult Result =
		    Contact.Target->ReceiveContact(Contact.Source, Contact.Source->Config().Strike(Attack->StrikeIndex), Time);
		Contact.Source->LastEvent = FString::Printf(TEXT("Strike %d: %s @ %.3f"),
		                                            Attack->StrikeIndex + 1,
		                                            *StaticEnum<EDuelResult>()->GetNameStringByValue(int64(Result)),
		                                            Time);
	}
}
