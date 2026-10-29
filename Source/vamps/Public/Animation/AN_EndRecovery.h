#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_EndRecovery.generated.h"

UCLASS(meta = (DisplayName = "End Recovery"))
class VAMPS_API UAN_EndRecovery : public UAnimNotify
{
	GENERATED_BODY()

public:

	UAN_EndRecovery();
	virtual void BranchingPointNotify(FBranchingPointNotifyPayload& Payload) override;
};
