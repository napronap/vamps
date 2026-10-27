#include "GameModes/VampsMatchGameMode.h"
#include "AbilitySystemComponent.h"
#include "AI/VampsAIController.h"
#include "Animation/AnimInstance.h"
#include "Characters/VampsCharacter.h"
#include "Combat/Attributes/VampsAttributeSet.h"
#include "Combat/Data/VampsCombatSettings.h"
#include "Combat/VampsCombatLog.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameModes/VampsTeamSpawnPoint.h"
#include "Player/VampsMatchPlayerController.h"
#include "UI/VampsCombatTestHUD.h"

AVampsMatchGameMode::AVampsMatchGameMode()
{
	PlayerControllerClass = AVampsMatchPlayerController::StaticClass();
	HUDClass = AVampsCombatTestHUD::StaticClass();
	TeamScores.Init(0, 2);
}

void AVampsMatchGameMode::StartPlay()
{
	Super::StartPlay();
	const float Durations[] = {PreparationDuration, RoundDuration, RespawnDelay, SpawnProtectionDuration};
	for (float MatchDuration : Durations)
	{
		if (!FMath::IsFinite(MatchDuration) || MatchDuration < 0.f)
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: '%s': invalid match duration %f."),
			       __FUNCTION__,
			       *GetPathName(),
			       MatchDuration);
			return;
		}
	}
	AVampsMatchPlayerController* LocalController =
	    Cast<AVampsMatchPlayerController>(GetWorld()->GetFirstPlayerController());
	AVampsCharacter* LocalFighter = LocalController ? Cast<AVampsCharacter>(LocalController->GetPawn()) : nullptr;
	if (!LocalFighter || !BotCharacterClass || BotCharacterClass->HasAnyClassFlags(CLASS_Abstract) ||
	    ScoreTarget <= 0 || BaseKillScore <= 0 || StreakScoreBonus < 0 || PreparationDuration <= 0.f ||
	    RoundDuration <= 0.f || RespawnDelay <= 0.f)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': requires match controller/player, concrete BotCharacterClass and valid match settings; "
		            "player='%s', bot='%s'."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(LocalFighter),
		       *GetPathNameSafe(BotCharacterClass.Get()));
		return;
	}
	TeamSpawnPoints.Init(nullptr, 2);
	for (TActorIterator<AVampsTeamSpawnPoint> SpawnIterator(GetWorld()); SpawnIterator; ++SpawnIterator)
	{
		AVampsTeamSpawnPoint* TeamSpawn = *SpawnIterator;
		if (!TeamSpawnPoints.IsValidIndex(TeamSpawn->TeamIndex) || TeamSpawnPoints[TeamSpawn->TeamIndex])
		{
			UE_LOG(
			    LogVampsCombat,
			    Error,
			    TEXT("%hs: '%s': spawn '%s' has invalid or duplicate team %d; require exactly one spawn per team 0/1."),
			    __FUNCTION__,
			    *GetPathName(),
			    *TeamSpawn->GetPathName(),
			    TeamSpawn->TeamIndex);
			return;
		}
		TeamSpawnPoints[TeamSpawn->TeamIndex] = TeamSpawn;
	}
	if (!TeamSpawnPoints[0] || !TeamSpawnPoints[1])
	{
		UE_LOG(LogVampsCombat, Error, TEXT("%hs: '%s': missing team A/B spawn point."), __FUNCTION__, *GetPathName());
		return;
	}
	Participants.SetNum(4);
	Participants[0].Fighter = LocalFighter;
	for (int32 ParticipantIndex = 0; ParticipantIndex < Participants.Num(); ++ParticipantIndex)
	{
		FVampsMatchParticipant& Participant = Participants[ParticipantIndex];
		Participant.TeamIndex = ParticipantIndex / 2;
		if (ParticipantIndex > 0)
		{
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			Participant.Fighter = GetWorld()->SpawnActor<AVampsCharacter>(
			    BotCharacterClass, TeamSpawnPoints[Participant.TeamIndex]->GetActorTransform(), SpawnParameters);
			AVampsAIController* BotController = GetWorld()->SpawnActor<AVampsAIController>();
			if (!Participant.Fighter || !BotController)
			{
				UE_LOG(LogVampsCombat,
				       Error,
				       TEXT("%hs: '%s': failed to create participant %d or its AI."),
				       __FUNCTION__,
				       *GetPathName(),
				       ParticipantIndex);
				return;
			}
			BotController->Possess(Participant.Fighter);
			BotController->SetMode(EVampsAIMode::Attack);
		}
		const AVampsCharacter* Fighter = Participant.Fighter;
		if (!Fighter->Attributes || !Fighter->AbilitySystem || !Fighter->CombatSettings ||
		    !Fighter->CombatSettings->HasValidDeathMontages() || !Fighter->CombatSettings->HasValidReactionSettings() ||
		    !Fighter->CombatSettings->HasValidMobilitySettings() ||
		    Fighter->AbilitySystem->GetActivatableAbilities().Num() < 2 || !Fighter->GetMesh()->GetAnimInstance())
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: '%s': participant '%s' requires attributes, abilities, settings, death montages and "
			            "animation."),
			       __FUNCTION__,
			       *GetPathName(),
			       *GetPathNameSafe(Fighter));
			return;
		}
	}
	RoundPhase = EVampsMatchPhase::Finished;
	RestartLocalRound();
}

int32 AVampsMatchGameMode::FindParticipantIndex(const AVampsCharacter* Fighter) const
{
	return Participants.IndexOfByPredicate(
	    [Fighter](const FVampsMatchParticipant& Participant)
	    {
		    return Participant.Fighter == Fighter;
	    });
}

bool AVampsMatchGameMode::AllowsCombat(const AVampsCharacter* Fighter) const
{
	const int32 ParticipantIndex = FindParticipantIndex(Fighter);
	return IsValid(Fighter) && RoundPhase == EVampsMatchPhase::Active && GetWorld()->GetTimeSeconds() < PhaseEndsAt &&
	       ParticipantIndex != INDEX_NONE && Participants[ParticipantIndex].RespawnAt == 0 &&
	       Fighter->CombatState.bIsAlive;
}

bool AVampsMatchGameMode::AreEnemies(const AVampsCharacter* FirstFighter, const AVampsCharacter* SecondFighter) const
{
	const int32 FirstIndex = FindParticipantIndex(FirstFighter);
	const int32 SecondIndex = FindParticipantIndex(SecondFighter);
	return IsValid(FirstFighter) && IsValid(SecondFighter) && RoundPhase == EVampsMatchPhase::Active &&
	       FirstIndex != INDEX_NONE && SecondIndex != INDEX_NONE &&
	       Participants[FirstIndex].TeamIndex != Participants[SecondIndex].TeamIndex;
}

bool AVampsMatchGameMode::IsSpawnProtected(const AVampsCharacter* Fighter) const
{
	const int32 ParticipantIndex = FindParticipantIndex(Fighter);
	return ParticipantIndex != INDEX_NONE &&
	       GetWorld()->GetTimeSeconds() < Participants[ParticipantIndex].ProtectionEndsAt;
}

void AVampsMatchGameMode::EndSpawnProtection(AVampsCharacter* Fighter)
{
	const int32 ParticipantIndex = FindParticipantIndex(Fighter);
	if (ParticipantIndex != INDEX_NONE)
	{
		Participants[ParticipantIndex].ProtectionEndsAt = 0;
	}
}

void AVampsMatchGameMode::RegisterCombatDeath(AVampsCharacter* Victim, AVampsCharacter* Attacker, bool bBlockedDeath)
{
	const int32 VictimIndex = FindParticipantIndex(Victim);
	const int32 AttackerIndex = FindParticipantIndex(Attacker);
	if (RoundPhase != EVampsMatchPhase::Active || VictimIndex == INDEX_NONE || AttackerIndex == INDEX_NONE ||
	    !AreEnemies(Victim, Attacker))
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': invalid match death, victim='%s', attacker='%s'."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(Victim),
		       *GetPathNameSafe(Attacker));
		return;
	}
	PendingDeaths.Add({VictimIndex,
	                   AttackerIndex,
	                   BaseKillScore + StreakScoreBonus * Participants[VictimIndex].KillStreak,
	                   bBlockedDeath});
}

bool AVampsMatchGameMode::RestoreParticipant(int32 ParticipantIndex, bool bRespawn)
{
	FVampsMatchParticipant& Participant = Participants[ParticipantIndex];
	AVampsCharacter* Fighter = Participant.Fighter;
	UAnimInstance* FighterAnimation = IsValid(Fighter) ? Fighter->GetMesh()->GetAnimInstance() : nullptr;
	if (!IsValid(Fighter) || !Fighter->Attributes || !Fighter->AbilitySystem || !FighterAnimation)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': cannot restore participant %d, fighter='%s'; requires attributes, AbilitySystem and "
		            "AnimInstance."),
		       __FUNCTION__,
		       *GetPathName(),
		       ParticipantIndex,
		       *GetPathNameSafe(Fighter));
		return false;
	}
	Fighter->GetMesh()->bPauseAnims = false;
	FighterAnimation->Montage_Stop(0.f);
	Fighter->ResetCombatState();
	Fighter->AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetHealthAttribute(),
	                                                Fighter->Attributes->GetMaxHealth());
	const AVampsTeamSpawnPoint* SpawnPoint = TeamSpawnPoints[Participant.TeamIndex];
	const FVector FormationOffset = SpawnPoint->GetActorRightVector() * ((ParticipantIndex % 2 == 0) ? -100.f : 100.f);
	Fighter->SetActorLocationAndRotation(SpawnPoint->GetActorLocation() + FormationOffset,
	                                     SpawnPoint->GetActorRotation(),
	                                     false,
	                                     nullptr,
	                                     ETeleportType::TeleportPhysics);
	Fighter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Fighter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	Fighter->ConsumeMovementInputVector();
	Participant.RespawnAt = 0;
	Participant.ProtectionEndsAt = bRespawn ? GetWorld()->GetTimeSeconds() + SpawnProtectionDuration : 0;
	if (AVampsMatchPlayerController* MatchController = Cast<AVampsMatchPlayerController>(Fighter->GetController()))
	{
		MatchController->FinishRespawnCamera();
		MatchController->SetControlRotation(SpawnPoint->GetActorRotation());
	}
	return true;
}

void AVampsMatchGameMode::RestartLocalRound()
{
	if (RoundPhase != EVampsMatchPhase::Finished)
	{
		return;
	}
	PendingDeaths.Reset();
	TeamScores.Init(0, 2);
	WinningTeamIndex = INDEX_NONE;
	RoundPhase = EVampsMatchPhase::Preparing;
	PhaseEndsAt = GetWorld()->GetTimeSeconds() + PreparationDuration;
	for (int32 ParticipantIndex = 0; ParticipantIndex < Participants.Num(); ++ParticipantIndex)
	{
		FVampsMatchParticipant& Participant = Participants[ParticipantIndex];
		Participant.Kills = 0;
		Participant.Deaths = 0;
		Participant.KillStreak = 0;
		if (!RestoreParticipant(ParticipantIndex, false))
		{
			RoundPhase = EVampsMatchPhase::Unavailable;
			return;
		}
	}
	if (APlayerController* LocalController = GetWorld()->GetFirstPlayerController())
	{
		LocalController->bShowMouseCursor = false;
		LocalController->bEnableClickEvents = false;
		LocalController->SetInputMode(FInputModeGameOnly());
	}
	UE_LOG(LogVampsCombat, Display, TEXT("%s: preparing local 2v2 round."), *GetPathName());
}

void AVampsMatchGameMode::CompleteCombatFrame()
{
	const double Now = GetWorld()->GetTimeSeconds();
	if (RoundPhase == EVampsMatchPhase::Preparing && Now >= PhaseEndsAt)
	{
		RoundPhase = EVampsMatchPhase::Active;
		PhaseEndsAt = Now + RoundDuration;
		UE_LOG(LogVampsCombat, Display, TEXT("%s: local round started."), *GetPathName());
	}
	if (RoundPhase != EVampsMatchPhase::Active)
	{
		return;
	}
	for (const FPendingMatchDeath& PendingDeath : PendingDeaths)
	{
		FVampsMatchParticipant& Attacker = Participants[PendingDeath.AttackerIndex];
		++Attacker.Kills;
		++Attacker.KillStreak;
		++Participants[PendingDeath.VictimIndex].Deaths;
		TeamScores[Attacker.TeamIndex] += PendingDeath.Reward;
	}
	for (const FPendingMatchDeath& PendingDeath : PendingDeaths)
	{
		FVampsMatchParticipant& Victim = Participants[PendingDeath.VictimIndex];
		Victim.KillStreak = 0;
		Victim.RespawnAt = Now + RespawnDelay;
		Victim.ProtectionEndsAt = 0;
		Victim.Fighter->PresentCombatDeath(PendingDeath.bBlocked);
		if (AVampsMatchPlayerController* MatchController =
		        Cast<AVampsMatchPlayerController>(Victim.Fighter->GetController()))
		{
			MatchController->BeginRespawnCamera();
		}
		UE_LOG(LogVampsCombat,
		       Display,
		       TEXT("%s: participant %d eliminated by %d; reward=%d, score A/B=%d/%d."),
		       *GetPathName(),
		       PendingDeath.VictimIndex,
		       PendingDeath.AttackerIndex,
		       PendingDeath.Reward,
		       TeamScores[0],
		       TeamScores[1]);
	}
	PendingDeaths.Reset();
	if (TeamScores[0] >= ScoreTarget || TeamScores[1] >= ScoreTarget || Now >= PhaseEndsAt)
	{
		FinishLocalRound(Now >= PhaseEndsAt);
		return;
	}
	for (int32 ParticipantIndex = 0; ParticipantIndex < Participants.Num(); ++ParticipantIndex)
	{
		if (Participants[ParticipantIndex].RespawnAt > 0 && Now >= Participants[ParticipantIndex].RespawnAt &&
		    !RestoreParticipant(ParticipantIndex, true))
		{
			RoundPhase = EVampsMatchPhase::Unavailable;
			return;
		}
	}
}

void AVampsMatchGameMode::FinishLocalRound(bool bTimeExpired)
{
	int32 TeamKills[2] = {};
	int32 TeamDeaths[2] = {};
	for (const FVampsMatchParticipant& Participant : Participants)
	{
		TeamKills[Participant.TeamIndex] += Participant.Kills;
		TeamDeaths[Participant.TeamIndex] += Participant.Deaths;
	}
	if (bTimeExpired && TeamScores[0] < ScoreTarget && TeamScores[1] < ScoreTarget && TeamScores[0] != TeamScores[1])
	{
		WinningTeamIndex = TeamScores[0] > TeamScores[1] ? 0 : 1;
	}
	else if ((TeamScores[0] >= ScoreTarget) != (TeamScores[1] >= ScoreTarget))
	{
		WinningTeamIndex = TeamScores[0] >= ScoreTarget ? 0 : 1;
	}
	else
	{
		const bool bInvictusA = TeamKills[0] > 0 && TeamDeaths[0] == 0;
		const bool bInvictusB = TeamKills[1] > 0 && TeamDeaths[1] == 0;
		const int64 RatioA = int64(TeamKills[0]) * FMath::Max(1, TeamDeaths[1]);
		const int64 RatioB = int64(TeamKills[1]) * FMath::Max(1, TeamDeaths[0]);
		if (bInvictusA != bInvictusB)
		{
			WinningTeamIndex = bInvictusA ? 0 : 1;
		}
		else if (!bInvictusA && RatioA != RatioB)
		{
			WinningTeamIndex = RatioA > RatioB ? 0 : 1;
		}
	}
	RoundPhase = EVampsMatchPhase::Finished;
	PhaseEndsAt = GetWorld()->GetTimeSeconds();
	for (FVampsMatchParticipant& Participant : Participants)
	{
		Participant.RespawnAt = 0;
		Participant.ProtectionEndsAt = 0;
		Participant.Fighter->StopMatchCombat();
	}
	if (APlayerController* LocalController = GetWorld()->GetFirstPlayerController())
	{
		LocalController->bShowMouseCursor = true;
		LocalController->bEnableClickEvents = true;
		FInputModeGameAndUI ResultInput;
		ResultInput.SetHideCursorDuringCapture(false);
		LocalController->SetInputMode(ResultInput);
	}
	UE_LOG(LogVampsCombat,
	       Display,
	       TEXT("%s: round finished; winning team=%d (-1 draw), score=%d/%d."),
	       *GetPathName(),
	       WinningTeamIndex,
	       TeamScores[0],
	       TeamScores[1]);
}
