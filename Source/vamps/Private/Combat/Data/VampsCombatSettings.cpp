#include "Combat/Data/VampsCombatSettings.h"
#include "Animation/AnimMontage.h"
#include "Combat/VampsCombatLog.h"

bool UVampsCombatSettings::HasValidMobilitySettings() const
{
	if (!FMath::IsFinite(MobilityEnergyCapacity) || MobilityEnergyCapacity <= 0.f || !FMath::IsFinite(DashEnergyCost) ||
	    DashEnergyCost < 0.f || DashEnergyCost > MobilityEnergyCapacity || !FMath::IsFinite(SprintEnergyPerSecond) ||
	    SprintEnergyPerSecond < 0.f || !FMath::IsFinite(MobilityRecoveryPerSecond) || MobilityRecoveryPerSecond < 0.f ||
	    !FMath::IsFinite(MobilityRecoveryDelay) || MobilityRecoveryDelay < 0.f || !FMath::IsFinite(SprintSpeed) ||
	    SprintSpeed <= 0.f || !FMath::IsFinite(ApproachDistance) || ApproachDistance < 0.f ||
	    !FMath::IsFinite(ApproachDuration) || ApproachDuration <= 0.f || !FMath::IsFinite(ApproachStopDistance) ||
	    ApproachStopDistance < 0.f)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': invalid mobility settings: capacity=%f, dashCost=%f, sprintCost=%f, recovery=%f, "
		            "delay=%f, speed=%f, approach=%f/%f/%f. Expected finite nonnegative costs/recovery/distances and "
		            "positive capacity/duration/speed; dash cost must not exceed capacity."),
		       __FUNCTION__,
		       *GetPathName(),
		       MobilityEnergyCapacity,
		       DashEnergyCost,
		       SprintEnergyPerSecond,
		       MobilityRecoveryPerSecond,
		       MobilityRecoveryDelay,
		       SprintSpeed,
		       ApproachDistance,
		       ApproachDuration,
		       ApproachStopDistance);
		return false;
	}
	return true;
}

bool UVampsCombatSettings::HasValidReactionSettings() const
{
	if (HitReactionMontage && GuardReactionMontage && HitReactionMontage == GuardReactionMontage)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': HitReactionMontage and GuardReactionMontage must be different assets; both reference "
		            "'%s'."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(HitReactionMontage));
		return false;
	}
	if (!FMath::IsFinite(CriticalDamage) || CriticalDamage < 0.f || !FMath::IsFinite(CriticalChancePercent) ||
	    CriticalChancePercent < 0.f || CriticalChancePercent > 100.f || !FMath::IsFinite(BlockDamageReduction) ||
	    BlockDamageReduction < 0.f || !FMath::IsFinite(GuardCapacity) || GuardCapacity <= 0.f ||
	    !FMath::IsFinite(GuardLoadPerHit) || GuardLoadPerHit < 0.f || !FMath::IsFinite(GuardRecoveryDelay) ||
	    GuardRecoveryDelay < 0.f || !FMath::IsFinite(GuardRecoveryPerSecond) || GuardRecoveryPerSecond < 0.f ||
	    !FMath::IsFinite(StunDuration) || StunDuration <= 0.f || !FMath::IsFinite(DefenseRequestDuration) ||
	    DefenseRequestDuration <= 0.f)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': invalid reaction settings: critical=%f, chance=%f, reduction=%f, capacity=%f, "
		            "load=%f, recoveryDelay=%f, recoveryRate=%f, stun=%f, defenseRequest=%f. Expected finite "
		            "nonnegative values, positive capacity/durations and chance in [0,100]."),
		       __FUNCTION__,
		       *GetPathName(),
		       CriticalDamage,
		       CriticalChancePercent,
		       BlockDamageReduction,
		       GuardCapacity,
		       GuardLoadPerHit,
		       GuardRecoveryDelay,
		       GuardRecoveryPerSecond,
		       StunDuration,
		       DefenseRequestDuration);
		return false;
	}
	return true;
}
