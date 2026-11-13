#pragma once

#include "GameFramework/Actor.h"
#include "VampsWeapon.generated.h"

class AVampsCharacter;
class UBoxComponent;
class USceneComponent;
class UStaticMeshComponent;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UBoxComponent> TraceBox;

	bool InitializeCombatTrace(AVampsCharacter& Fighter);
	void ResetSectionHits();
	void ResetImpactHits();
	void SetTracingEnabled(bool bEnabled);

private:

	TWeakObjectPtr<AVampsCharacter> OwningFighter;
	TSet<TWeakObjectPtr<AVampsCharacter>> HitActors;
};
