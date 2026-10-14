#include "Combat/Effects/GE_VampsDamage.h"
#include "Combat/VampsCombatTags.h"
#include "Combat/Attributes/VampsAttributeSet.h"

UGE_VampsDamage::UGE_VampsDamage()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	FGameplayModifierInfo& Modifier = Modifiers.AddDefaulted_GetRef();
	Modifier.Attribute = UVampsAttributeSet::GetHealthAttribute();
	Modifier.ModifierOp = EGameplayModOp::Additive;
	FSetByCallerFloat Magnitude;
	Magnitude.DataTag = VampsTags::Damage;
	Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(Magnitude);
}
