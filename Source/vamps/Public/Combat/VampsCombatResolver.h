#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "Combat/Data/VampsCombatSettings.h"
#include "Delegates/Delegate.h"
#include "VampsCombatResolver.generated.h"

class AVampsCharacter;

// Stores both fighters and the values captured by the weapon sweep for this frame.
struct FVampsPendingContact
{
	TWeakObjectPtr<AVampsCharacter> Attacker;
	TWeakObjectPtr<AVampsCharacter> Recipient;
	int32 AttackerMontageInstanceID = INDEX_NONE;        // Identifies the registered attack playback.
	int32 RecipientAttackMontageInstanceID = INDEX_NONE; // Set if the recipient could deal damage at the sweep.
	int32 ActionNumber = 0;                              // Identifies the attacker's combo step at the sweep.
	uint32 AttackerGeneration = 0;                       // Attacker's combat generation at the sweep.
	uint32 RecipientGeneration = 0;                      // Recipient's combat generation at the sweep.

	float ContactDamage = 0.f;  // Normal or critical damage before applying guard reduction.
	float BlockReduction = 0.f; // Damage subtracted when the recipient blocks.
	float GuardLoad = 0.f;      // Load added to the recipient's guard on a block.

	FVector AttackOrigin = FVector::ZeroVector;       // Attacker position at the sweep, used for hit alignment.
	FVector AttackDirection = FVector::ForwardVector; // Attacker facing at the sweep, used for displacement.
	FVampsCombatStepSettings StepSettings;            // Reaction and displacement settings for this combo step.

	bool bBlocked = false;    // Set when the resolver classifies the contact as blocked.
	bool bGuardBreak = false; // Set on the blocked contact that fills the guard meter.
};

enum class EVampsCombatResolution : uint8
{
	ConfirmDefense,
	CreditContact,
	ApplyDamage,
	ApplyReaction
};

struct FVampsCombatResolution
{
	EVampsCombatResolution Type;
	const FVampsPendingContact& Contact; // Borrowed for the synchronous call.
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

	struct FAttackPlayback
	{
		TWeakObjectPtr<AVampsCharacter> Fighter;
		int32 MontageInstanceID;

		bool IsActive() const;
	};

	// Registers a montage once. Its combo sections keep the same initiative.
	void StartAttack(AVampsCharacter& Fighter);

	// Stores a contact captured by the attacker after a valid weapon sweep.
	void QueueCombatContact(FVampsPendingContact Contact);

	// Queues a one-shot callback after contacts, before the match advances.
	void RunAfterCombatResolution(FSimpleDelegate Callback);

	// Processes contacts and deferred callbacks before updating the match.
	void ResolveCombatFrame(UWorld* TickedWorld, ELevelTick TickType, float DeltaSeconds);

	// Determines which contacts are blocked before applying damage and selecting one reaction per recipient.
	void ResolvePendingContacts();
	void ClassifyDefense(TArray<FVampsPendingContact>& Contacts);
	void ApplyContactGroup(const TArray<FVampsPendingContact>& Contacts);

	TArray<FAttackPlayback> AttackPlaybacks;
	TArray<FVampsPendingContact> PendingContacts;
	TArray<FSimpleDelegate> AfterCombatResolutionCallbacks;
	FDelegateHandle PostActorTickHandle;
};
