#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_AttackTrace.generated.h"

// Marks the part of the attack animation that can hit OR block
UCLASS(meta = (DisplayName = "Attack Trace"))
class VAMPS_API UANS_AttackTrace : public UAnimNotifyState
{
	GENERATED_BODY()

public:

	UANS_AttackTrace();
	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& Payload) override;
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& Payload) override;

private:

	void SetTracing(FBranchingPointNotifyPayload& Payload, bool bEnabled);
};
