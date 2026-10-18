#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_ComboWindow.generated.h"

UCLASS(meta = (DisplayName = "Combo Window"))
class VAMPS_API UANS_ComboWindow : public UAnimNotifyState
{
	GENERATED_BODY()

public:

	UANS_ComboWindow();
	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload) override;
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload) override;
};
