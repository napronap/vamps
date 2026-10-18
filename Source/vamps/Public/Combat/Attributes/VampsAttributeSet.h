#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "VampsAttributeSet.generated.h"

UCLASS()
class VAMPS_API UVampsAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	UVampsAttributeSet();

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Health")
	FGameplayAttributeData Health;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Health")
	FGameplayAttributeData MaxHealth;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_GuardLoad, Category = "Guard")
	FGameplayAttributeData GuardLoad;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxGuardLoad, Category = "Guard")
	FGameplayAttributeData MaxGuardLoad;

	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UVampsAttributeSet, GuardLoad)
	GAMEPLAYATTRIBUTE_VALUE_GETTER(GuardLoad)
	GAMEPLAYATTRIBUTE_VALUE_SETTER(GuardLoad)
	GAMEPLAYATTRIBUTE_VALUE_INITTER(GuardLoad)

	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UVampsAttributeSet, MaxGuardLoad)
	GAMEPLAYATTRIBUTE_VALUE_GETTER(MaxGuardLoad)
	GAMEPLAYATTRIBUTE_VALUE_SETTER(MaxGuardLoad)
	GAMEPLAYATTRIBUTE_VALUE_INITTER(MaxGuardLoad)

	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UVampsAttributeSet, Health)
	GAMEPLAYATTRIBUTE_VALUE_GETTER(Health)
	GAMEPLAYATTRIBUTE_VALUE_SETTER(Health)
	GAMEPLAYATTRIBUTE_VALUE_INITTER(Health)

	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UVampsAttributeSet, MaxHealth)
	GAMEPLAYATTRIBUTE_VALUE_GETTER(MaxHealth)
	GAMEPLAYATTRIBUTE_VALUE_SETTER(MaxHealth)
	GAMEPLAYATTRIBUTE_VALUE_INITTER(MaxHealth)

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_GuardLoad(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxGuardLoad(const FGameplayAttributeData& OldValue);
};
