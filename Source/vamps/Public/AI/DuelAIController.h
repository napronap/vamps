#pragma once

#include "AIController.h"
#include "Combat/Data/CombatConfig.h"
#include "DuelAIController.generated.h"

UCLASS()
class VAMPS_API ADuelAIController : public AAIController
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Duel")
	EDuelAIMode Mode = EDuelAIMode::Mixed;

	void Advance(double Now);
	void ResetSequence(double Now);

	UFUNCTION(BlueprintCallable, Category = "Duel")
	void CycleMode();

private:

	double NextDecision = 0;
	double NextPress = 0;
	double Epoch = 0;
};
