#include "Animation/ANS_AttackTrace.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Characters/VampsCharacter.h"
#include "Components/SkeletalMeshComponent.h"

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
	if (AVampsCharacter* Fighter =
	        Payload.SkelMeshComponent ? Cast<AVampsCharacter>(Payload.SkelMeshComponent->GetOwner()) : nullptr)
	{
		Fighter->SetCombatContacts(Payload, bEnabled);
	}
}
