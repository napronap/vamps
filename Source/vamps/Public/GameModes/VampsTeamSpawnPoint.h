#pragma once

#include "GameFramework/PlayerStart.h"
#include "VampsTeamSpawnPoint.generated.h"

UCLASS()
class VAMPS_API AVampsTeamSpawnPoint : public APlayerStart
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Match", meta = (ClampMin = "0", ClampMax = "1"))
	int32 TeamIndex = 0;
};
