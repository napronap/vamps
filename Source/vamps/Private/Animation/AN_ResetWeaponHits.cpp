#include "Animation/AN_ResetWeaponHits.h"
#include "Characters/VampsCharacter.h"
#include "Combat/VampsCombatLog.h"
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
	AVampsCharacter* Fighter =
	    Payload.SkelMeshComponent ? Cast<AVampsCharacter>(Payload.SkelMeshComponent->GetOwner()) : nullptr;
	if (!Fighter)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: notify '%s' cannot reset weapon hits; mesh '%s' requires a VampsCharacter owner."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(Payload.SkelMeshComponent));
		return;
	}
	Fighter->ResetCombatWeaponHits(Payload, WeaponIndices);
}
