#include "Animation/AN_ResetWeaponHits.h"
#include "Characters/VampsCharacter.h"
#include "Components/SkeletalMeshComponent.h"

UAN_ResetWeaponHits::UAN_ResetWeaponHits()
{
	bIsNativeBranchingPoint = true;
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif
}

void UAN_ResetWeaponHits::BranchingPointNotify(FBranchingPointNotifyPayload& Payload)
{
	if (AVampsCharacter* Fighter =
	        Payload.SkelMeshComponent ? Cast<AVampsCharacter>(Payload.SkelMeshComponent->GetOwner()) : nullptr)
	{
		Fighter->ResetCombatWeaponHits(Payload, WeaponIndices);
	}
}
