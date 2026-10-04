#pragma once

#include "GameFramework/GameModeBase.h"
#include "DuelGameMode.generated.h"

class AVampsDuelCharacter;
class UCombatConfig;

UCLASS()
class VAMPS_API ADuelGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	ADuelGameMode();

	virtual void StartPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Duel")
	TSubclassOf<AVampsDuelCharacter> OpponentClass;

	UPROPERTY(EditDefaultsOnly, Category = "Duel")
	TObjectPtr<UCombatConfig> SharedConfig;

	UPROPERTY(EditAnywhere, Category = "Duel")
	FTransform PlayerSpawn;

	UPROPERTY(EditAnywhere, Category = "Duel")
	FTransform OpponentSpawn;

	UPROPERTY(BlueprintReadOnly, Category = "Duel")
	TObjectPtr<AVampsDuelCharacter> Opponent;

	UFUNCTION(BlueprintCallable, Category = "Duel")
	void ResetDuel();
};
