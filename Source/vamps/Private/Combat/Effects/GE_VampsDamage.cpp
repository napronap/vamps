#include "Combat/Effects/GE_VampsDamage.h"
#include "Combat/VampsCombatTags.h"
#include "Combat/Attributes/VampsAttributeSet.h"

UGE_VampsDamage::UGE_VampsDamage()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	// add a modifier to the effect and keep a reference to configure it
	FGameplayModifierInfo& Modifier = Modifiers.AddDefaulted_GetRef();

	// add the modifier's signed value to the target's Health
	Modifier.Attribute = UVampsAttributeSet::GetHealthAttribute();
	Modifier.ModifierOp = EGameplayModOp::Additive;

	FSetByCallerFloat Magnitude;
	// ReceiveCombatHit stores -DamageAmount under this tag before applying
	Magnitude.DataTag = VampsTags::Damage;

	// and finally use that tag lookup to resolve the amount added to Health
	Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(Magnitude);
}
