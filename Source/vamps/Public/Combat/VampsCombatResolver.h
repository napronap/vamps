#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "Combat/Data/VampsCombatSettings.h"
#include "VampsCombatResolver.generated.h"

class AVampsCharacter;
class AVampsWeapon;

// Stores both fighters and the values captured by the weapon sweep for this frame.
struct FVampsPendingContact
{
	TWeakObjectPtr<AVampsCharacter> Attacker;
	TWeakObjectPtr<AVampsCharacter> Recipient;
	int32 ActionNumber = 0;         // Identifies the attacker's combo step at the sweep.
	uint32 AttackerGeneration = 0;  // Attacker's combat generation at the sweep.
	uint32 RecipientGeneration = 0; // Recipient's combat generation at the sweep.

	float ContactDamage = 0.f;  // Normal or critical damage before applying guard reduction.
	float BlockReduction = 0.f; // Damage subtracted when the recipient blocks.
	float GuardLoad = 0.f;      // Load added to the recipient's guard on a block.

	FVector AttackOrigin = FVector::ZeroVector;       // Attacker position at the sweep, used for hit alignment.
	FVector AttackDirection = FVector::ForwardVector; // Attacker facing at the sweep, used for displacement.
	FVampsCombatStepSettings StepSettings;            // Reaction and displacement settings for this combo step.

	bool bBlocked = false;    // Set when the resolver classifies the contact as blocked.
	bool bCritical = false;   // Rolled when the weapon queues the contact.
	bool bGuardBreak = false; // Set on the blocked contact that fills the guard meter.
};

UCLASS()
class VAMPS_API UVampsCombatResolver : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	// Subscribes to the world post-actor tick so weapons and notifies can queue work first.
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// Unsubscribe and clear queues
	virtual void Deinitialize() override;

protected:

	// Creates a resolver in gameplay and Play In Editor worlds.
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

private:

	friend class AVampsCharacter;
	friend class AVampsWeapon;

	// Called by the attacker's weapon when a sweep finds a valid target.
	void QueueCombatContact(AVampsCharacter& Attacker, AVampsCharacter& Recipient);

	// Buffers dash request until incoming hits have resolved
	void QueueCombatDash(AVampsCharacter& Fighter);

	// Queues a buffered combo input when the combo window closes
	void QueueCombatContinuation(AVampsCharacter& Fighter);

	// Stores the action number from End Recovery so a new combo step can invalidate it
	void QueueCombatRecoveryEnd(AVampsCharacter& Fighter);

	// Drops a fighter's queued continuations, recovery ends and dashes on death, reset or EndPlay.
	void DiscardPendingCombatFor(AVampsCharacter& Fighter);

	// Processes contacts, continuations, recovery ends and dashes in order, then updates the match.
	void ResolveCombatFrame(UWorld* TickedWorld, ELevelTick TickType, float DeltaSeconds);

	// Determines which contacts are blocked before applying damage and selecting one reaction per recipient.
	void ResolvePendingContacts();

	static int32 GetContactRank(const FVampsPendingContact& Contact);

	TArray<FVampsPendingContact> PendingContacts;
	TArray<TWeakObjectPtr<AVampsCharacter>> PendingContinuations;
	TMap<TWeakObjectPtr<AVampsCharacter>, int32> PendingRecoveryEnds;
	TArray<TWeakObjectPtr<AVampsCharacter>> PendingDashes;
	FDelegateHandle PostActorTickHandle;
};
