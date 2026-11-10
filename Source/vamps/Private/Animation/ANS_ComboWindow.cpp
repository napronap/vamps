#include "Animation/ANS_ComboWindow.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Characters/VampsCharacter.h"
#include "Components/SkeletalMeshComponent.h"

UANS_ComboWindow::UANS_ComboWindow()
{
	bIsNativeBranchingPoint = true;
}

void UANS_ComboWindow::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload)
{
	if (AVampsCharacter* Fighter =
	        Payload.SkelMeshComponent ? Cast<AVampsCharacter>(Payload.SkelMeshComponent->GetOwner()) : nullptr)
	{
		Fighter->SetCombatComboWindow(Payload, true);
	}
}

void UANS_ComboWindow::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload)
{
	if (AVampsCharacter* Fighter =
	        Payload.SkelMeshComponent ? Cast<AVampsCharacter>(Payload.SkelMeshComponent->GetOwner()) : nullptr)
	{
		Fighter->SetCombatComboWindow(Payload, false);
	}
}
