#include "Animation/VampsAnimInstance.h"
#include "Animation/BlendSpace.h"
#include "Characters/VampsCharacter.h"
#include "Combat/Data/VampsCombatSettings.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Combat/VampsCombatLog.h"

void UVampsAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	LocomotionBlendSpace = nullptr;
	APawn* OwningPawn = TryGetPawnOwner();
	if (!OwningPawn || !GetWorld()->IsGameWorld())
	{
		return;
	}

	const AVampsCharacter* OwningCharacter = Cast<AVampsCharacter>(OwningPawn);
	const UVampsCombatSettings* Settings = OwningCharacter ? OwningCharacter->CombatSettings.Get() : nullptr;
	if (!Settings || !Settings->LocomotionBlendSpace)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': locomotion initialization requires a VampsCharacter with CombatSettings and "
		            "LocomotionBlendSpace; pawn='%s', settings='%s'."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(OwningPawn),
		       *GetPathNameSafe(Settings));
		return;
	}

	LocomotionBlendSpace = Settings->LocomotionBlendSpace;
}

void UVampsAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	APawn* OwningPawn = TryGetPawnOwner();
	if (!OwningPawn || !GetWorld()->IsGameWorld())
	{
		return;
	}

	const AVampsCharacter* OwningCharacter = Cast<AVampsCharacter>(OwningPawn);
	if (!OwningCharacter || !OwningCharacter->GetCharacterMovement())
	{
		UE_LOG(
		    LogVampsCombat,
		    Error,
		    TEXT("%hs: %s: animation update rejected; pawn '%s' must be a VampsCharacter with a movement component."),
		    __FUNCTION__,
		    *GetPathName(),
		    *GetPathNameSafe(OwningPawn));
		return;
	}
	const FVector Velocity = OwningCharacter->GetVelocity();
	Speed = Velocity.Size2D();
	Direction = Speed > 1.f
	                ? FMath::FindDeltaAngleDegrees(OwningCharacter->GetActorRotation().Yaw, Velocity.Rotation().Yaw)
	                : 0.f;
	bInAir = OwningCharacter->GetCharacterMovement()->IsFalling();
	const FVampsCombatState& CurrentCombat = OwningCharacter->CombatState;
	bIsGuarding = CurrentCombat.Reaction == EVampsCombatReaction::Guard;
	bIsHitReacting = CurrentCombat.Reaction == EVampsCombatReaction::Hit;
	bIsStunned = CurrentCombat.Reaction == EVampsCombatReaction::Stun;
	CombatReactionRemaining = FMath::Max(0.f, float(CurrentCombat.ReactionEndsAt - GetWorld()->GetTimeSeconds()));
	bIsDashing = CurrentCombat.bIsDashing;
	bIsSprinting = CurrentCombat.bIsSprinting;
}
