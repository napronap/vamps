#include "Animation/ANS_AttackTrace.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Characters/VampsCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Combat/VampsCombatLog.h"

UANS_AttackTrace::UANS_AttackTrace()
{
	bIsNativeBranchingPoint = true;
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif
}

void UANS_AttackTrace::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload)
{
	SetTracing(Payload, true);
}

void UANS_AttackTrace::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload)
{
	SetTracing(Payload, false);
}

void UANS_AttackTrace::SetTracing(FBranchingPointNotifyPayload& Payload, bool bEnabled)
{
	if (AVampsCharacter* Fighter = Payload.SkelMeshComponent ? Cast<AVampsCharacter>(Payload.SkelMeshComponent->GetOwner())) {
		Fighter->SetCombatContacts(Payload, bEnabled);
	}
	
	// AVampsCharacter* Fighter =
	//     Payload.SkelMeshComponent ? Cast<AVampsCharacter>(Payload.SkelMeshComponent->GetOwner()) : nullptr;
	// if (!Fighter)
	// {
	// 	UE_LOG(LogVampsCombat,
	// 	       Error,
	// 	       TEXT("%hs: notify '%s' cannot update attack traces; mesh '%s' requires a VampsCharacter owner."),
	// 	       __FUNCTION__,
	// 	       *GetPathName(),
	// 	       *GetPathNameSafe(Payload.SkelMeshComponent));
	// 	return;
	// }
}
