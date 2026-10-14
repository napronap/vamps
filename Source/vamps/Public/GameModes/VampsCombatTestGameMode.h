#pragma once

#include "GameFramework/GameModeBase.h"
#include "AI/VampsAIController.h"
#include "VampsCombatTestGameMode.generated.h"

class AVampsCharacter;

UCLASS()
class VAMPS_API AVampsCombatTestGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	AVampsCombatTestGameMode();
	virtual void StartPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Combat Test")
	TSubclassOf<AVampsCharacter> OpponentClass;

	UPROPERTY(EditDefaultsOnly, Category = "Combat Test")
	TSubclassOf<AVampsAIController> OpponentControllerClass;

	UPROPERTY(EditDefaultsOnly, Category = "Combat Test")
	EVampsAIMode InitialAIMode = EVampsAIMode::Passive;

	UPROPERTY(EditDefaultsOnly, Category = "Combat Test")
	FTransform PlayerSpawn;

	UPROPERTY(EditDefaultsOnly, Category = "Combat Test")
	FTransform OpponentSpawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat Test")
	TObjectPtr<AVampsCharacter> Opponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat Test")
	TObjectPtr<AVampsAIController> OpponentAI;

	UFUNCTION(BlueprintCallable, Category = "Combat Test")
	void CycleAIMode();

	UFUNCTION(BlueprintCallable, Category = "Combat Test")
	void ResetCombat();
};
