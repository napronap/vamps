#include "Combat/VampsCombatResolver.h"
#include "Characters/VampsCharacter.h"
#include "Characters/VampsLockOnComponent.h"
#include "Engine/World.h"

void UVampsCombatResolver::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	PostActorTickHandle =
	    FWorldDelegates::OnWorldPostActorTick.AddUObject(this, &UVampsCombatResolver::ResolveCombatContacts);
}

void UVampsCombatResolver::Deinitialize()
{
	FWorldDelegates::OnWorldPostActorTick.Remove(PostActorTickHandle);
	PendingContacts.Reset();
	Super::Deinitialize();
}

bool UVampsCombatResolver::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void UVampsCombatResolver::QueueCombatContact(AVampsCharacter* Attacker, AVampsCharacter* Recipient)
{
	check(Attacker && Recipient && Attacker != Recipient);
	check(Attacker->HasAuthority() && Recipient->HasAuthority());
	check(Attacker->GetWorld() == GetWorld() && Recipient->GetWorld() == GetWorld());
	PendingContacts.Add({Attacker, Recipient, Attacker->CombatState.ActionNumber});
}

void UVampsCombatResolver::DiscardContactsFor(const AVampsCharacter* Fighter)
{
	PendingContacts.RemoveAll(
	    [Fighter](const FVampsPendingContact& Contact)
	    {
		    return Contact.Attacker.Get() == Fighter || Contact.Recipient.Get() == Fighter;
	    });
}

void UVampsCombatResolver::ResolveCombatContacts(UWorld* TickedWorld, ELevelTick TickType, float DeltaSeconds)
{
	if (TickedWorld != GetWorld() || PendingContacts.IsEmpty())
	{
		return;
	}

	TArray<FVampsPendingContact> Contacts = MoveTemp(PendingContacts);
	Contacts.RemoveAll(
	    [](const FVampsPendingContact& Contact)
	    {
		    return !Contact.Attacker.IsValid() || !Contact.Recipient.IsValid() ||
		           Contact.Attacker->IsActorBeingDestroyed() || Contact.Recipient->IsActorBeingDestroyed() ||
		           !Contact.Attacker->CombatState.bIsAlive || !Contact.Recipient->CombatState.bIsAlive;
	    });

	TMap<AVampsCharacter*, TArray<AVampsCharacter*>> BlockedAttackers;
	for (FVampsPendingContact& Contact : Contacts)
	{
		Contact.bBlocked = Contact.Recipient->CombatState.bCanDefend;
		if (Contact.bBlocked)
		{
			BlockedAttackers.FindOrAdd(Contact.Recipient.Get()).AddUnique(Contact.Attacker.Get());
		}
	}

	for (const auto& BlockedEntry : BlockedAttackers)
	{
		AVampsCharacter* Defender = BlockedEntry.Key;
		Defender->ConsumeDefenseRequest();
		AVampsCharacter* SelectedAttacker = Defender->LockOn->GetCurrentTarget();
		if (!BlockedEntry.Value.Contains(SelectedAttacker))
		{
			SelectedAttacker = nullptr;
			for (const TWeakObjectPtr<AVampsCharacter>& Candidate : Defender->LockOn->GetCandidates())
			{
				if (BlockedEntry.Value.Contains(Candidate.Get()))
				{
					SelectedAttacker = Candidate.Get();
					break;
				}
			}
		}
		checkf(SelectedAttacker, TEXT("A blocked attacker must be registered for lock-on."));
		Defender->LockOn->SelectTarget(SelectedAttacker);
	}

	for (const FVampsPendingContact& Contact : Contacts)
	{
		if (Contact.Attacker->CombatState.ActionNumber == Contact.ActionNumber)
		{
			Contact.Attacker->RegisterCombatContact(Contact.bBlocked);
		}
		Contact.Recipient->ReceiveCombatHit(Contact.Attacker.Get(), Contact.bBlocked);
	}
}
