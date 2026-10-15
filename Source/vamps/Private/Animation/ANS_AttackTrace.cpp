#include "Animation/ANS_AttackTrace.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Characters/VampsCharacter.h"
#include "Combat/FistHitboxComponent.h"
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
	AVampsCharacter* Fighter =
	    Payload.SkelMeshComponent ? Cast<AVampsCharacter>(Payload.SkelMeshComponent->GetOwner()) : nullptr;
	if (!Fighter || !Fighter->HasAuthority() || !Fighter->IsAttacking() ||
	    Payload.SkelMeshComponent != Fighter->GetMesh())
	{
		return;
	}

	UAnimInstance* Animation = Fighter->GetMesh()->GetAnimInstance();
	const FAnimMontageInstance* Montage =
	    Animation ? Animation->GetActiveInstanceForMontage(Fighter->AttackMontage) : nullptr;
	// Ignore notifications from a cancelled, older playback of the same montage.
	if (!Montage || Montage->GetInstanceID() != Payload.MontageInstanceID)
	{
		return;
	}

	if (bEnabled)
	{
		Fighter->FistHitbox->StartTracing();
	}
	else
	{
		Fighter->FistHitbox->StopTracing();
	}
}
