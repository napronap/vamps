#include "GameModes/VampsMatchGameMode.h"
#include "AbilitySystemComponent.h"
#include "AI/VampsAIController.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/BlendSpace.h"
#include "Characters/VampsCharacter.h"
#include "Combat/Attributes/VampsAttributeSet.h"
#include "Combat/Data/VampsCombatSettings.h"
#include "Combat/VampsCombatLog.h"
#include "Combat/VampsWeapon.h"
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
	TeamScores.Init(0, 2);
}

void AVampsMatchGameMode::StartPlay()
{
	bStartPlayersAsSpectators = true;
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
	if (!LocalController || !DefaultPawnClass || !DefaultPawnClass->IsChildOf(AVampsCharacter::StaticClass()) ||
	    DefaultPawnClass->HasAnyClassFlags(CLASS_Abstract) || !BotCharacterClass ||
	    BotCharacterClass->HasAnyClassFlags(CLASS_Abstract) || ScoreTarget <= 0 || BaseKillScore <= 0 ||
	    StreakScoreBonus < 0 || PreparationDuration <= 0.f || RoundDuration <= 0.f || RespawnDelay <= 0.f)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': requires match controller, concrete player/bot classes and valid match settings; "
		            "player='%s', bot='%s'."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(DefaultPawnClass.Get()),
		       *GetPathNameSafe(BotCharacterClass.Get()));
		return;
	}
	if (!ValidateCharacterCatalog())
	{
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
	RoundPhase = EVampsMatchPhase::Finished;
	OpenCharacterSelection();
}

bool AVampsMatchGameMode::ValidateCharacterCatalog() const
{
	if (AvailableCharacters.Num() < 4)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': AvailableCharacters requires at least four distinct assets; received %d."),
		       __FUNCTION__,
		       *GetPathName(),
		       AvailableCharacters.Num());
		return false;
	}
	TSet<const UVampsCombatSettings*> RegisteredCharacters;
	for (int32 CharacterIndex = 0; CharacterIndex < AvailableCharacters.Num(); ++CharacterIndex)
	{
		const UVampsCombatSettings* CharacterSettings = AvailableCharacters[CharacterIndex];
		if (!IsValid(CharacterSettings) || CharacterSettings->CharacterName.IsEmptyOrWhitespace() ||
		    RegisteredCharacters.Contains(CharacterSettings))
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: '%s': AvailableCharacters[%d] '%s' requires a valid asset, nonempty CharacterName and "
			            "a unique asset reference."),
			       __FUNCTION__,
			       *GetPathName(),
			       CharacterIndex,
			       *GetPathNameSafe(CharacterSettings));
			return false;
		}
		RegisteredCharacters.Add(CharacterSettings);
	}
	return true;
}

bool AVampsMatchGameMode::CreateParticipants(const TArray<TObjectPtr<UVampsCombatSettings>>& RoundCharacters,
                                             AVampsMatchPlayerController* LocalController)
{
	for (int32 ParticipantIndex = 0; ParticipantIndex < RoundCharacters.Num(); ++ParticipantIndex)
	{
		const UVampsCombatSettings* CharacterSettings = RoundCharacters[ParticipantIndex];
		UClass* FighterClass = ParticipantIndex == 0 ? DefaultPawnClass.Get() : BotCharacterClass.Get();
		const AVampsCharacter* FighterDefaults =
		    FighterClass ? Cast<AVampsCharacter>(FighterClass->GetDefaultObject()) : nullptr;
		if (!FighterDefaults || FighterClass->HasAnyClassFlags(CLASS_Abstract) || !FighterDefaults->GetMesh() ||
		    !FighterDefaults->GetMesh()->GetSkeletalMeshAsset() || !FighterDefaults->GetMesh()->GetAnimClass() ||
		    !FMath::IsFinite(FighterDefaults->StartingMaxHealth) || FighterDefaults->StartingMaxHealth <= 0.f)
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: '%s': participant %d requires a concrete VampsCharacter class with mesh, animation "
			            "class and positive finite health; class='%s'."),
			       __FUNCTION__,
			       *GetPathName(),
			       ParticipantIndex,
			       *GetPathNameSafe(FighterClass));
			return false;
		}
		if (!CharacterSettings->LocomotionBlendSpace || !CharacterSettings->AttackMontage ||
		    CharacterSettings->Weapons.IsEmpty())
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: '%s': participant %d asset '%s' is incomplete; requires LocomotionBlendSpace, "
			            "AttackMontage and weapons (blend space='%s', attack='%s', weapons=%d). "
			            "Round remains in character selection."),
			       __FUNCTION__,
			       *GetPathName(),
			       ParticipantIndex,
			       *GetPathNameSafe(CharacterSettings),
			       *GetPathNameSafe(CharacterSettings->LocomotionBlendSpace),
			       *GetPathNameSafe(CharacterSettings->AttackMontage),
			       CharacterSettings->Weapons.Num());
			return false;
		}
		for (const FVampsWeaponSettings& WeaponSettings : CharacterSettings->Weapons)
		{
			if (!WeaponSettings.WeaponClass || WeaponSettings.WeaponClass->HasAnyClassFlags(CLASS_Abstract) ||
			    WeaponSettings.AttachSocket.IsNone() ||
			    !FighterDefaults->GetMesh()->DoesSocketExist(WeaponSettings.AttachSocket))
			{
				UE_LOG(LogVampsCombat,
				       Error,
				       TEXT("%hs: '%s': participant %d asset '%s' requires a concrete weapon class and an existing "
				            "mesh socket; weapon='%s', socket='%s', fighterClass='%s'."),
				       __FUNCTION__,
				       *GetPathName(),
				       ParticipantIndex,
				       *CharacterSettings->GetPathName(),
				       *GetPathNameSafe(WeaponSettings.WeaponClass.Get()),
				       *WeaponSettings.AttachSocket.ToString(),
				       *GetPathNameSafe(FighterClass));
				return false;
			}
		}
	}
	ClearParticipants();
	Participants.SetNum(RoundCharacters.Num());
	for (int32 ParticipantIndex = 0; ParticipantIndex < Participants.Num(); ++ParticipantIndex)
	{
		FVampsMatchParticipant& Participant = Participants[ParticipantIndex];
		Participant.TeamIndex = ParticipantIndex / 2;
		UClass* FighterClass = ParticipantIndex == 0 ? DefaultPawnClass.Get() : BotCharacterClass.Get();
		const FTransform SpawnTransform = TeamSpawnPoints[Participant.TeamIndex]->GetActorTransform();
		Participant.Fighter = GetWorld()->SpawnActorDeferred<AVampsCharacter>(
		    FighterClass, SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		AVampsCharacter* Fighter = Participant.Fighter;
		if (Fighter)
		{
			Fighter->CombatSettings = RoundCharacters[ParticipantIndex];
			Fighter->FinishSpawning(SpawnTransform);
			if (IsValid(Fighter))
			{
				Fighter->GetMesh()->InitAnim(true);
			}
		}
		if (!IsValid(Fighter) || Fighter->CombatSettings != RoundCharacters[ParticipantIndex] || !Fighter->Attributes ||
		    !Fighter->AbilitySystem || Fighter->AbilitySystem->GetActivatableAbilities().Num() < 2 ||
		    Fighter->Weapons.Num() != RoundCharacters[ParticipantIndex]->Weapons.Num() ||
		    !Fighter->GetMesh()->GetAnimInstance())
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: '%s': participant %d failed initialization; class='%s', asset='%s', fighter='%s'. "
			            "Cancelling participant creation."),
			       __FUNCTION__,
			       *GetPathName(),
			       ParticipantIndex,
			       *GetPathNameSafe(FighterClass),
			       *GetPathNameSafe(RoundCharacters[ParticipantIndex]),
			       *GetPathNameSafe(Fighter));
			ClearParticipants();
			return false;
		}
		AController* ParticipantController = LocalController;
		if (ParticipantIndex > 0)
		{
			AVampsAIController* BotController = GetWorld()->SpawnActor<AVampsAIController>();
			ParticipantController = BotController;
			if (BotController)
			{
				BotController->SetMode(EVampsAIMode::Attack);
			}
		}
		if (ParticipantController)
		{
			ParticipantController->Possess(Fighter);
		}
		if (!ParticipantController || Fighter->GetController() != ParticipantController)
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: '%s': participant %d '%s' could not be possessed by '%s'."),
			       __FUNCTION__,
			       *GetPathName(),
			       ParticipantIndex,
			       *Fighter->GetPathName(),
			       *GetPathNameSafe(ParticipantController));
			if (ParticipantIndex > 0 && ParticipantController)
			{
				ParticipantController->Destroy();
			}
			ClearParticipants();
			return false;
		}
	}
	return true;
}

void AVampsMatchGameMode::ClearParticipants()
{
	for (const FVampsMatchParticipant& Participant : Participants)
	{
		AVampsCharacter* Fighter = Participant.Fighter;
		if (IsValid(Fighter))
		{
			AController* ParticipantController = Fighter->GetController();
			if (ParticipantController)
			{
				ParticipantController->UnPossess();
				if (Cast<AVampsAIController>(ParticipantController))
				{
					ParticipantController->Destroy();
				}
			}
			Fighter->Destroy();
		}
	}
	Participants.Reset();
	PendingDeaths.Reset();
}

int32 AVampsMatchGameMode::FindParticipantIndex(const AVampsCharacter* Fighter) const
{
	return Participants.IndexOfByPredicate(
	    [Fighter](const FVampsMatchParticipant& Participant)
	    {
		    return Participant.Fighter == Fighter;
	    });
}

// CHECK SI ES NECESARIO
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
	return IsValid(FirstFighter) && IsValid(SecondFighter) && FirstIndex != INDEX_NONE && SecondIndex != INDEX_NONE &&
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

void AVampsMatchGameMode::RegisterCombatDeath(AVampsCharacter* Victim, AVampsCharacter* Attacker)
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
	PendingDeaths.Add(
	    {VictimIndex, AttackerIndex, BaseKillScore + StreakScoreBonus * Participants[VictimIndex].KillStreak});
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
	Participant.RespawnAt = 0;
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
	Participant.ProtectionEndsAt = bRespawn ? GetWorld()->GetTimeSeconds() + SpawnProtectionDuration : 0;
	if (AVampsMatchPlayerController* MatchController = Cast<AVampsMatchPlayerController>(Fighter->GetController()))
	{
		MatchController->SetControlRotation(SpawnPoint->GetActorRotation());
	}
	return true;
}

void AVampsMatchGameMode::OpenCharacterSelection()
{
	if (RoundPhase != EVampsMatchPhase::Finished)
	{
		return;
	}
	PhaseEndsAt = 0;
	SetRoundPhase(EVampsMatchPhase::Selecting);
	PendingDeaths.Reset();
	for (FVampsMatchParticipant& Participant : Participants)
	{
		if (!IsValid(Participant.Fighter))
		{
			UE_LOG(LogVampsCombat,
			       Error,
			       TEXT("%hs: '%s': cannot open character selection; participant fighter '%s' is invalid."),
			       __FUNCTION__,
			       *GetPathName(),
			       *GetPathNameSafe(Participant.Fighter));
			SetRoundPhase(EVampsMatchPhase::Unavailable);
			return;
		}
		Participant.RespawnAt = 0;
		Participant.ProtectionEndsAt = 0;
		Participant.Fighter->StopMatchCombat();
	}
	if (!EnableMatchMenuInput())
	{
		return;
	}
	UE_LOG(LogVampsCombat, Display, TEXT("%s: character selection opened."), *GetPathName());
}

void AVampsMatchGameMode::SelectCharacter(UVampsCombatSettings* CharacterSettings)
{
	if (RoundPhase != EVampsMatchPhase::Selecting)
	{
		return;
	}
	if (!IsValid(CharacterSettings) || !AvailableCharacters.Contains(CharacterSettings))
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': cannot select asset '%s'; it must belong to AvailableCharacters."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(CharacterSettings));
		return;
	}
	if (SelectedCharacter != CharacterSettings)
	{
		SelectedCharacter = CharacterSettings;
		OnSelectedCharacterChanged.Broadcast(CharacterSettings);
	}
}

void AVampsMatchGameMode::StartLocalRound()
{
	if (RoundPhase != EVampsMatchPhase::Selecting || !SelectedCharacter)
	{
		return;
	}
	if (!ValidateCharacterCatalog())
	{
		return;
	}
	if (!AvailableCharacters.Contains(SelectedCharacter))
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': selected asset '%s' no longer belongs to AvailableCharacters."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(SelectedCharacter));
		return;
	}
	AVampsMatchPlayerController* LocalController =
	    Cast<AVampsMatchPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!IsValid(LocalController))
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': cannot start the local round without a valid player controller."),
		       __FUNCTION__,
		       *GetPathName());
		SetRoundPhase(EVampsMatchPhase::Unavailable);
		return;
	}
	TArray<TObjectPtr<UVampsCombatSettings>> RemainingCharacters = AvailableCharacters;
	RemainingCharacters.Remove(SelectedCharacter);
	TArray<TObjectPtr<UVampsCombatSettings>> RoundCharacters = {SelectedCharacter};
	while (RoundCharacters.Num() < 4)
	{
		const int32 ChoiceIndex = FMath::RandHelper(RemainingCharacters.Num());
		RoundCharacters.Add(RemainingCharacters[ChoiceIndex]);
		RemainingCharacters.RemoveAtSwap(ChoiceIndex);
	}
	if (!CreateParticipants(RoundCharacters, LocalController))
	{
		return;
	}
	TeamScores.Init(0, 2);
	WinningTeamIndex = INDEX_NONE;
	for (int32 ParticipantIndex = 0; ParticipantIndex < Participants.Num(); ++ParticipantIndex)
	{
		if (!RestoreParticipant(ParticipantIndex, false))
		{
			ClearParticipants();
			return;
		}
	}
	PhaseEndsAt = GetWorld()->GetTimeSeconds() + PreparationDuration;
	SetRoundPhase(EVampsMatchPhase::Preparing);
	LocalController->bShowMouseCursor = false;
	LocalController->bEnableClickEvents = false;
	LocalController->SetInputMode(FInputModeGameOnly());
	UE_LOG(LogVampsCombat, Display, TEXT("%s: preparing local 2v2 round."), *GetPathName());
}

void AVampsMatchGameMode::CompleteCombatFrame()
{
	const double Now = GetWorld()->GetTimeSeconds();
	if (RoundPhase == EVampsMatchPhase::Preparing && Now >= PhaseEndsAt)
	{
		PhaseEndsAt = Now + RoundDuration;
		SetRoundPhase(EVampsMatchPhase::Active);
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
			SetRoundPhase(EVampsMatchPhase::Unavailable);
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
	PhaseEndsAt = GetWorld()->GetTimeSeconds();
	SetRoundPhase(EVampsMatchPhase::Finished);
	for (FVampsMatchParticipant& Participant : Participants)
	{
		Participant.RespawnAt = 0;
		Participant.ProtectionEndsAt = 0;
		Participant.Fighter->StopMatchCombat();
	}
	if (!EnableMatchMenuInput())
	{
		return;
	}
	UE_LOG(LogVampsCombat,
	       Display,
	       TEXT("%s: round finished; winning team=%d (-1 draw), score=%d/%d."),
	       *GetPathName(),
	       WinningTeamIndex,
	       TeamScores[0],
	       TeamScores[1]);
}

void AVampsMatchGameMode::SetRoundPhase(EVampsMatchPhase NewPhase)
{
	if (RoundPhase != NewPhase)
	{
		RoundPhase = NewPhase;
		OnRoundPhaseChanged.Broadcast();
	}
}

bool AVampsMatchGameMode::EnableMatchMenuInput()
{
	APlayerController* LocalController = GetWorld()->GetFirstPlayerController();
	if (!IsValid(LocalController))
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s': match menu requires a valid local player controller."),
		       __FUNCTION__,
		       *GetPathName());
		SetRoundPhase(EVampsMatchPhase::Unavailable);
		return false;
	}
	LocalController->bShowMouseCursor = true;
	LocalController->bEnableClickEvents = true;
	FInputModeGameAndUI MenuInput;
	MenuInput.SetHideCursorDuringCapture(false);
	LocalController->SetInputMode(MenuInput);
	return true;
}
