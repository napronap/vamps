#include "Combat/Attributes/VampsAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UVampsAttributeSet::UVampsAttributeSet()
{
	InitMaxHealth(100);
	InitHealth(100);

	InitMaxGuardLoad(100);
	InitGuardLoad(0);
	
	InitMaxMobilityEnergy(100);
	InitMobilityEnergy(100);
}

void UVampsAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if (Attribute == GetMaxMobilityEnergyAttribute())
	{
		NewValue = FMath::Max(0.001f, NewValue);
	}

	if (Attribute == GetMobilityEnergyAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMobilityEnergy());
	}

	if (Attribute == GetMaxGuardLoadAttribute())
	{
		NewValue = FMath::Max(0.001f, NewValue);
	}

	if (Attribute == GetGuardLoadAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxGuardLoad());
	}

	if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(1.f, NewValue);
	}

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
}

void UVampsAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	SetMaxHealth(FMath::Max(1.f, GetMaxHealth()));
	SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));

	SetMaxGuardLoad(FMath::Max(0.001f, GetMaxGuardLoad()));
	SetGuardLoad(FMath::Clamp(GetGuardLoad(), 0.f, GetMaxGuardLoad()));

	SetMaxMobilityEnergy(FMath::Max(0.001f, GetMaxMobilityEnergy()));
	SetMobilityEnergy(FMath::Clamp(GetMobilityEnergy(), 0.f, GetMaxMobilityEnergy()));
}

void UVampsAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UVampsAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UVampsAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UVampsAttributeSet, GuardLoad, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UVampsAttributeSet, MaxGuardLoad, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UVampsAttributeSet, MobilityEnergy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UVampsAttributeSet, MaxMobilityEnergy, COND_None, REPNOTIFY_Always);
}

void UVampsAttributeSet::OnRep_MobilityEnergy(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UVampsAttributeSet, MobilityEnergy, OldValue);
}

void UVampsAttributeSet::OnRep_MaxMobilityEnergy(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UVampsAttributeSet, MaxMobilityEnergy, OldValue);
}

void UVampsAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UVampsAttributeSet, Health, OldValue);
}

void UVampsAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UVampsAttributeSet, MaxHealth, OldValue);
}

void UVampsAttributeSet::OnRep_GuardLoad(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UVampsAttributeSet, GuardLoad, OldValue);
}

void UVampsAttributeSet::OnRep_MaxGuardLoad(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UVampsAttributeSet, MaxGuardLoad, OldValue);
}
