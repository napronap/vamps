#include "Animation/ANS_ComboWindow.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Characters/VampsCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Combat/VampsCombatLog.h"

UANS_ComboWindow::UANS_ComboWindow()
{
	bIsNativeBranchingPoint = true;
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif
}

void UANS_ComboWindow::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload)
{
	if (AVampsCharacter* Fighter = Payload.SkelMeshComponent ? Cast<AVampsCharacter>(Payload.SkelMeshComponent->GetOwner())) {
		Fighter->SetCombatComboWindow(Payload, true);
	}
	// AVampsCharacter* Fighter =
	//     Payload.SkelMeshComponent ? Cast<AVampsCharacter>(Payload.SkelMeshComponent->GetOwner()) : nullptr;
	// if (!Fighter)
	// {
	// 	UE_LOG(LogVampsCombat,
	// 	       Error,
	// 	       TEXT("%hs: notify '%s' cannot open combo window; mesh '%s' requires a VampsCharacter owner."),
	// 	       __FUNCTION__,
	// 	       *GetPathName(),
	// 	       *GetPathNameSafe(Payload.SkelMeshComponent));
	// 	return;
	// }
}

void UANS_ComboWindow::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload)
{
	if (AVampsCharacter* Fighter = Payload.SkelMeshComponent ? Cast<AVampsCharacter>(Payload.SkelMeshComponent->GetOwner())) {
		Fighter->SetCombatComboWindow(Payload, false);
	}
	
	// AVampsCharacter* Fighter =
	//     Payload.SkelMeshComponent ? Cast<AVampsCharacter>(Payload.SkelMeshComponent->GetOwner()) : nullptr;
	// if (!Fighter)
	// {
	// 	UE_LOG(LogVampsCombat,
	// 	       Error,
	// 	       TEXT("%hs: notify '%s' cannot close combo window; mesh '%s' requires a VampsCharacter owner."),
	// 	       __FUNCTION__,
	// 	       *GetPathName(),
	// 	       *GetPathNameSafe(Payload.SkelMeshComponent));
	// 	return;
	// }
}
