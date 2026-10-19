#pragma once

#include "GameFramework/Actor.h"
#include "VampsWeapon.generated.h"

class AVampsCharacter;
class USceneComponent;
class UStaticMeshComponent;
struct FCollisionQueryParams;

UCLASS(Abstract, Blueprintable)
class VAMPS_API AVampsWeapon : public AActor
{
	GENERATED_BODY()

public:

	AVampsWeapon();
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<USceneComponent> WeaponRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<USceneComponent> TraceStart;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<USceneComponent> TraceEnd;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (ClampMin = "0.1", Units = "cm"))
	float TraceRadius = 8.f;

	void ResetSectionHits();
	int32 ImpactIndex = 0;
	void ResetImpactHits();
	bool SetTracingEnabled(bool bEnabled);

protected:

	virtual void BeginPlay() override;

private:

	FTransform PreviousTraceTransform;
	bool bHasPreviousTrace = false;
	TSet<TWeakObjectPtr<AVampsCharacter>> HitActors;

	bool ValidateTraceConfiguration() const;
	void SweepWeaponSegment(const FVector& SweepStart,
	                        const FVector& SweepEnd,
	                        float Radius,
	                        AVampsCharacter* Fighter,
	                        const FCollisionQueryParams& Query);
};
