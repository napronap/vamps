#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_ComboWindow.generated.h"

// takes combo input and queues the requested continuation when the window closes
// if there's no further combo input, animation will go on, entering the recovery part and triggering recovery state
// this also serves as a signal for when the combo continues
UCLASS(meta = (DisplayName = "Combo Window"))
class VAMPS_API UANS_ComboWindow : public UAnimNotifyState
{
	GENERATED_BODY()

public:

	UANS_ComboWindow();
	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload) override;
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload) override;
};
