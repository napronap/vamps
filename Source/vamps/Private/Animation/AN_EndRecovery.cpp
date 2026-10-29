#include "Animation/AN_EndRecovery.h"
#include "Characters/VampsCharacter.h"
#include "Combat/VampsCombatLog.h"
#include "Components/SkeletalMeshComponent.h"

UAN_EndRecovery::UAN_EndRecovery()
{
	bIsNativeBranchingPoint = true;
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif
}

void UAN_EndRecovery::BranchingPointNotify(FBranchingPointNotifyPayload& Payload)
{
	AVampsCharacter* Fighter =
	    Payload.SkelMeshComponent ? Cast<AVampsCharacter>(Payload.SkelMeshComponent->GetOwner()) : nullptr;
	if (!Fighter)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: notify '%s' cannot end recovery; mesh '%s' requires a VampsCharacter owner."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(Payload.SkelMeshComponent));
		return;
	}
	Fighter->EndCombatRecovery(Payload);
}
