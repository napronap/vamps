#include "Combat/Effects/GE_DuelDamage.h"
#include "Combat/CombatTags.h"
#include "Combat/Attributes/VampsAttributeSet.h"

UGE_DuelDamage::UGE_DuelDamage()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	FGameplayModifierInfo& Modifier = Modifiers.AddDefaulted_GetRef();
	Modifier.Attribute = UVampsAttributeSet::GetHealthAttribute();
	Modifier.ModifierOp = EGameplayModOp::Additive;
	FSetByCallerFloat Magnitude;
	Magnitude.DataTag = DuelTags::Damage;
	Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(Magnitude);
}
