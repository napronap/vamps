#include "Combat/Data/CombatConfig.h"

UCombatConfig::UCombatConfig()
{
	for (int32 Index = 1; Index <= 3; ++Index)
	{
		FDuelStrike& Entry = Strikes.AddDefaulted_GetRef();
		Entry.PrepareSection = FName(*FString::Printf(TEXT("Hit%d_Prepare"), Index));
		Entry.ActiveSection = FName(*FString::Printf(TEXT("Hit%d_Active"), Index));
		Entry.RecoverySection = FName(*FString::Printf(TEXT("Hit%d_Recovery"), Index));
	}
}

const FDuelStrike& UCombatConfig::Strike(int32 Index) const
{
	static const FDuelStrike Fallback;
	return Strikes.IsValidIndex(Index) ? Strikes[Index] : Fallback;
}
