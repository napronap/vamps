#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_EndRecovery.generated.h"

// End recovery signals when the character can move after finishing an attack (i.e starting the "recovery" part of the animation)
// and when the character enters NA mode
// this should be in ALL attack sequences to correctly call the montage to blend back once this plays, otherwise the montage gets stuck
UCLASS(meta = (DisplayName = "End Recovery"))
class VAMPS_API UAN_EndRecovery : public UAnimNotify
{
	GENERATED_BODY()

public:

	UAN_EndRecovery();
	virtual void BranchingPointNotify(FBranchingPointNotifyPayload& Payload) override;
};
