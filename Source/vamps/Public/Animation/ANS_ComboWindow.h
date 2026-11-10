#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_ComboWindow.generated.h"

// At the end of combo window the next montage will jump to the next section and continue the combo if there was an input for it
// within the window. Otherwise animation continues into "recovery"
UCLASS(meta = (DisplayName = "Combo Window"))
class VAMPS_API UANS_ComboWindow : public UAnimNotifyState
{
	GENERATED_BODY()

public:

	UANS_ComboWindow();
	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload) override;
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload) override;
};
