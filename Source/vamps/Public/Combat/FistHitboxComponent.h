#pragma once

#include "Components/SphereComponent.h"
#include "FistHitboxComponent.generated.h"

class AVampsCharacter;

UCLASS()
class VAMPS_API UFistHitboxComponent : public USphereComponent
{
	GENERATED_BODY()

public:

	UFistHitboxComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime,
	                           ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FName SocketName = TEXT("FistHit");

	bool HasFistSocket() const;
	void ResetHits();
	void StartTracing();
	void StopTracing();

private:

	FVector PreviousPosition = FVector::ZeroVector;
	bool bHasPreviousPosition = false;
	TSet<TWeakObjectPtr<AVampsCharacter>> HitActors;

	void TraceHitbox(const FVector& Start, const FVector& End, AVampsCharacter* Fighter);
};
