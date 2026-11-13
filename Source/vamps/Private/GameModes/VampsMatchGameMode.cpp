#include "GameModes/VampsMatchGameMode.h"
#include "AbilitySystemComponent.h"
#include "AI/VampsAIController.h"
#include "Characters/VampsCharacter.h"
#include "Combat/Attributes/VampsAttributeSet.h"
#include "Combat/Data/VampsCombatSettings.h"
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
	bStartPlayersAsSpectators = true;
	TeamScores.Init(0, TeamCount);
}

void AVampsMatchGameMode::StartPlay()
{
	Super::StartPlay();

	TeamSpawnPoints.SetNum(TeamCount);
	for (AVampsTeamSpawnPoint* TeamSpawn : TActorRange<AVampsTeamSpawnPoint>(GetWorld()))
	{
		TeamSpawnPoints[TeamSpawn->TeamIndex] = TeamSpawn;
	}

	OpenCharacterSelection();
}

void AVampsMatchGameMode::OpenCharacterSelection()
{
	PhaseEndsAt = 0;
	SetRoundPhase(EVampsMatchPhase::Selecting);
	EnableMatchMenuInput();
}

void AVampsMatchGameMode::SelectCharacter(UVampsCombatSettings* CharacterSettings)
{
	SelectedCharacter = CharacterSettings;
	OnSelectedCharacterChanged.Broadcast(SelectedCharacter);
}

void AVampsMatchGameMode::StartLocalRound()
{
	if (RoundPhase != EVampsMatchPhase::Selecting)
	{
		return;
	}

	TArray<TObjectPtr<UVampsCombatSettings>> RemainingCharacters = AvailableCharacters;
	RemainingCharacters.Remove(SelectedCharacter);

	TArray<TObjectPtr<UVampsCombatSettings>> RoundCharacters = {SelectedCharacter};
	while (RoundCharacters.Num() < TeamCount * FightersPerTeam)
	{
		const int32 ChoiceIndex = FMath::RandHelper(RemainingCharacters.Num());
		RoundCharacters.Add(RemainingCharacters[ChoiceIndex]);
		RemainingCharacters.RemoveAtSwap(ChoiceIndex);
	}

	ClearParticipants();
	SpawnParticipants(RoundCharacters);
	TeamScores.Init(0, TeamCount);
	WinningTeamIndex = INDEX_NONE;

	APlayerController& LocalController = *GetWorld()->GetFirstPlayerController();
	LocalController.Possess(Participants[0].Fighter);
	LocalController.SetControlRotation(Participants[0].Fighter->GetActorRotation());

	for (int32 ParticipantIndex = 1; ParticipantIndex < Participants.Num(); ++ParticipantIndex)
	{
		AVampsAIController* BotController = GetWorld()->SpawnActor<AVampsAIController>();
		BotController->Mode = EVampsAIMode::Attack;
		BotController->Possess(Participants[ParticipantIndex].Fighter);
	}

	PhaseEndsAt = GetWorld()->GetTimeSeconds() + PreparationDuration;
	SetRoundPhase(EVampsMatchPhase::Preparing);
	LocalController.bShowMouseCursor = false;
	LocalController.bEnableClickEvents = false;
	LocalController.SetInputMode(FInputModeGameOnly());
}

void AVampsMatchGameMode::SpawnParticipants(const TArray<TObjectPtr<UVampsCombatSettings>>& RoundCharacters)
{
	Participants.SetNum(RoundCharacters.Num());
	for (int32 ParticipantIndex = 0; ParticipantIndex < Participants.Num(); ++ParticipantIndex)
	{
		FVampsMatchParticipant& Participant = Participants[ParticipantIndex];
		Participant.TeamIndex = ParticipantIndex / FightersPerTeam;
		const FTransform SpawnTransform = GetFormationTransform(ParticipantIndex);
		UClass* FighterClass = ParticipantIndex == 0 ? DefaultPawnClass.Get() : BotCharacterClass.Get();
		Participant.Fighter = GetWorld()->SpawnActorDeferred<AVampsCharacter>(
		    FighterClass, SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		AVampsCharacter& Fighter = *Participant.Fighter;
		Fighter.CombatSettings = RoundCharacters[ParticipantIndex];
		Fighter.FinishSpawning(SpawnTransform);
		Fighter.GetMesh()->InitAnim(true);
	}
}

void AVampsMatchGameMode::ClearParticipants()
{
	for (const FVampsMatchParticipant& Participant : Participants)
	{
		AController& ParticipantController = *Participant.Fighter->GetController();
		ParticipantController.UnPossess();
		if (!ParticipantController.IsPlayerController())
		{
			ParticipantController.Destroy();
		}
		Participant.Fighter->Destroy();
	}
	Participants.Reset();
}

FTransform AVampsMatchGameMode::GetFormationTransform(int32 ParticipantIndex) const
{
	const AVampsTeamSpawnPoint& SpawnPoint = *TeamSpawnPoints[Participants[ParticipantIndex].TeamIndex];
	const float SideOffset = ParticipantIndex % FightersPerTeam == 0 ? -100.f : 100.f;
	const FVector Location = SpawnPoint.GetActorLocation() + SpawnPoint.GetActorRightVector() * SideOffset;
	return FTransform(SpawnPoint.GetActorQuat(), Location, SpawnPoint.GetActorScale3D());
}

void AVampsMatchGameMode::RespawnParticipant(int32 ParticipantIndex, double Now)
{
	FVampsMatchParticipant& Participant = Participants[ParticipantIndex];
	AVampsCharacter& Fighter = *Participant.Fighter;
	const FTransform SpawnTransform = GetFormationTransform(ParticipantIndex);

	Fighter.ResetCombatState();
	Participant.RespawnAt = 0;
	Fighter.AbilitySystem->SetNumericAttributeBase(UVampsAttributeSet::GetHealthAttribute(),
	                                               Fighter.Attributes->GetMaxHealth());
	Fighter.SetActorLocationAndRotation(
	    SpawnTransform.GetLocation(), SpawnTransform.GetRotation(), false, nullptr, ETeleportType::TeleportPhysics);
	Fighter.GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Fighter.GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	Fighter.ConsumeMovementInputVector();
	Participant.ProtectionEndsAt = Now + SpawnProtectionDuration;

	if (ParticipantIndex == 0)
	{
		Fighter.GetController()->SetControlRotation(SpawnTransform.Rotator());
	}
}

int32 AVampsMatchGameMode::FindParticipantIndex(const AVampsCharacter* Fighter) const
{
	return Participants.IndexOfByPredicate(
	    [Fighter](const FVampsMatchParticipant& Participant)
	    {
		    return Participant.Fighter.Get() == Fighter;
	    });
}

bool AVampsMatchGameMode::AllowsCombat(const AVampsCharacter* Fighter) const
{
	const int32 ParticipantIndex = FindParticipantIndex(Fighter);
	return RoundPhase == EVampsMatchPhase::Active && GetWorld()->GetTimeSeconds() < PhaseEndsAt &&
	       ParticipantIndex != INDEX_NONE && Participants[ParticipantIndex].RespawnAt == 0 &&
	       Fighter->CombatState.bIsAlive;
}

bool AVampsMatchGameMode::AreEnemies(const AVampsCharacter* FirstFighter, const AVampsCharacter* SecondFighter) const
{
	const int32 FirstIndex = FindParticipantIndex(FirstFighter);
	const int32 SecondIndex = FindParticipantIndex(SecondFighter);
	return FirstIndex != INDEX_NONE && SecondIndex != INDEX_NONE &&
	       Participants[FirstIndex].TeamIndex != Participants[SecondIndex].TeamIndex;
}

bool AVampsMatchGameMode::IsSpawnProtected(const AVampsCharacter* Fighter) const
{
	return GetWorld()->GetTimeSeconds() < Participants[FindParticipantIndex(Fighter)].ProtectionEndsAt;
}

void AVampsMatchGameMode::EndSpawnProtection(AVampsCharacter* Fighter)
{
	Participants[FindParticipantIndex(Fighter)].ProtectionEndsAt = 0;
}

void AVampsMatchGameMode::RegisterCombatDeath(AVampsCharacter* Victim, AVampsCharacter* Attacker)
{
	const int32 VictimIndex = FindParticipantIndex(Victim);
	PendingDeaths.Add({VictimIndex,
	                   FindParticipantIndex(Attacker),
	                   BaseKillScore + StreakScoreBonus * Participants[VictimIndex].KillStreak});
}

void AVampsMatchGameMode::CompleteCombatFrame()
{
	const double Now = GetWorld()->GetTimeSeconds();
	if (RoundPhase == EVampsMatchPhase::Preparing && Now >= PhaseEndsAt)
	{
		PhaseEndsAt = Now + RoundDuration;
		SetRoundPhase(EVampsMatchPhase::Active);
	}
	if (RoundPhase != EVampsMatchPhase::Active)
	{
		return;
	}

	for (const FPendingMatchDeath& Death : PendingDeaths)
	{
		FVampsMatchParticipant& Attacker = Participants[Death.AttackerIndex];
		++Attacker.Kills;
		++Attacker.KillStreak;
		++Participants[Death.VictimIndex].Deaths;
		TeamScores[Attacker.TeamIndex] += Death.Reward;
	}
	for (const FPendingMatchDeath& Death : PendingDeaths)
	{
		FVampsMatchParticipant& Victim = Participants[Death.VictimIndex];
		Victim.KillStreak = 0;
		Victim.RespawnAt = Now + RespawnDelay;
		Victim.ProtectionEndsAt = 0;
	}
	PendingDeaths.Reset();

	if (Now >= PhaseEndsAt || TeamScores[0] >= ScoreTarget || TeamScores[1] >= ScoreTarget)
	{
		FinishLocalRound();
		return;
	}

	for (int32 ParticipantIndex = 0; ParticipantIndex < Participants.Num(); ++ParticipantIndex)
	{
		const double RespawnAt = Participants[ParticipantIndex].RespawnAt;
		if (RespawnAt > 0 && Now >= RespawnAt)
		{
			RespawnParticipant(ParticipantIndex, Now);
		}
	}
}

void AVampsMatchGameMode::FinishLocalRound()
{
	WinningTeamIndex = DetermineWinningTeam();
	PhaseEndsAt = GetWorld()->GetTimeSeconds();
	SetRoundPhase(EVampsMatchPhase::Finished);

	for (FVampsMatchParticipant& Participant : Participants)
	{
		Participant.RespawnAt = 0;
		Participant.ProtectionEndsAt = 0;
		Participant.Fighter->StopMatchCombat();
	}

	EnableMatchMenuInput();
}

int32 AVampsMatchGameMode::DetermineWinningTeam() const
{
	const bool bTeamAReachedTarget = TeamScores[0] >= ScoreTarget;
	const bool bTeamBReachedTarget = TeamScores[1] >= ScoreTarget;

	if (bTeamAReachedTarget != bTeamBReachedTarget)
	{
		return bTeamAReachedTarget ? 0 : 1;
	}
	if (!bTeamAReachedTarget && TeamScores[0] != TeamScores[1])
	{
		return TeamScores[0] > TeamScores[1] ? 0 : 1;
	}

	int32 TeamKills[TeamCount] = {};
	for (const FVampsMatchParticipant& Participant : Participants)
	{
		TeamKills[Participant.TeamIndex] += Participant.Kills;
	}
	if (TeamKills[0] == TeamKills[1])
	{
		return INDEX_NONE;
	}
	return TeamKills[0] > TeamKills[1] ? 0 : 1;
}

void AVampsMatchGameMode::SetRoundPhase(EVampsMatchPhase NewPhase)
{
	RoundPhase = NewPhase;
	OnRoundPhaseChanged.Broadcast();
}

void AVampsMatchGameMode::EnableMatchMenuInput()
{
	APlayerController& LocalController = *GetWorld()->GetFirstPlayerController();
	LocalController.bShowMouseCursor = true;
	LocalController.bEnableClickEvents = true;
	FInputModeGameAndUI MenuInput;
	MenuInput.SetHideCursorDuringCapture(false);
	LocalController.SetInputMode(MenuInput);
}
