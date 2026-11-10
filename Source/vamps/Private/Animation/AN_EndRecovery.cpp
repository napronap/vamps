#include "Animation/AN_EndRecovery.h"
#include "Characters/VampsCharacter.h"
#include "Components/SkeletalMeshComponent.h"

UAN_EndRecovery::UAN_EndRecovery()
{
	bIsNativeBranchingPoint = true;
}

void UAN_EndRecovery::BranchingPointNotify(FBranchingPointNotifyPayload& Payload)
{
	if (AVampsCharacter* Fighter =
	        Payload.SkelMeshComponent ? Cast<AVampsCharacter>(Payload.SkelMeshComponent->GetOwner()) : nullptr)
	{
		Fighter->EndCombatRecovery(Payload);
	}
}
