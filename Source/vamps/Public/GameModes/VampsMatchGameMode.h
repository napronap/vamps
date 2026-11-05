#pragma once

#include "GameFramework/GameModeBase.h"
#include "VampsMatchGameMode.generated.h"

class AVampsCharacter;
class AVampsMatchPlayerController;
class AVampsTeamSpawnPoint;
class UVampsCombatSettings;

UENUM(BlueprintType)
enum class EVampsMatchPhase : uint8
{
	Unavailable,
	Selecting,
	Preparing,
	Active,
	Finished
};

USTRUCT(BlueprintType)
struct VAMPS_API FVampsMatchParticipant
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AVampsCharacter> Fighter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 TeamIndex = INDEX_NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Kills = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Deaths = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 KillStreak = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	double RespawnAt = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	double ProtectionEndsAt = 0;
};

UCLASS()
class VAMPS_API AVampsMatchGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	AVampsMatchGameMode();
	virtual void StartPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Match")
	TSubclassOf<AVampsCharacter> BotCharacterClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Match")
	TArray<TObjectPtr<UVampsCombatSettings>> AvailableCharacters;

	UPROPERTY(EditDefaultsOnly, Category = "Match", meta = (ClampMin = "1"))
	int32 ScoreTarget = 1000;

	UPROPERTY(EditDefaultsOnly, Category = "Match", meta = (ClampMin = "1"))
	int32 BaseKillScore = 100;

	UPROPERTY(EditDefaultsOnly, Category = "Match", meta = (ClampMin = "0"))
	int32 StreakScoreBonus = 25;

	UPROPERTY(EditDefaultsOnly, Category = "Match", meta = (ClampMin = "0.001"))
	float PreparationDuration = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Match", meta = (ClampMin = "0.001"))
	float RoundDuration = 300.f;

	UPROPERTY(EditDefaultsOnly, Category = "Match", meta = (ClampMin = "0.001"))
	float RespawnDelay = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Match", meta = (ClampMin = "0"))
	float SpawnProtectionDuration = 2.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Match")
	EVampsMatchPhase RoundPhase = EVampsMatchPhase::Unavailable;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Match")
	double PhaseEndsAt = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Match")
	TArray<FVampsMatchParticipant> Participants;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Match")
	TArray<int32> TeamScores;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Match")
	int32 WinningTeamIndex = INDEX_NONE;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Transient, Category = "Match")
	TObjectPtr<UVampsCombatSettings> SelectedCharacter;

	bool AllowsCombat(const AVampsCharacter* Fighter) const;
	bool AreEnemies(const AVampsCharacter* FirstFighter, const AVampsCharacter* SecondFighter) const;
	bool IsSpawnProtected(const AVampsCharacter* Fighter) const;
	void EndSpawnProtection(AVampsCharacter* Fighter);
	void RegisterCombatDeath(AVampsCharacter* Victim, AVampsCharacter* Attacker, bool bBlockedDeath);
	void CompleteCombatFrame();
	void OpenCharacterSelection();
	void SelectCharacter(UVampsCombatSettings* CharacterSettings);
	void StartLocalRound();

private:

	UPROPERTY()
	TArray<TObjectPtr<AVampsTeamSpawnPoint>> TeamSpawnPoints;

	struct FPendingMatchDeath
	{
		int32 VictimIndex;
		int32 AttackerIndex;
		int32 Reward;
		bool bBlocked;
	};

	TArray<FPendingMatchDeath> PendingDeaths;

	int32 FindParticipantIndex(const AVampsCharacter* Fighter) const;
	bool ValidateCharacterCatalog() const;
	bool CreateParticipants(const TArray<TObjectPtr<UVampsCombatSettings>>& RoundCharacters,
	                        AVampsMatchPlayerController* LocalController);
	void ClearParticipants();
	bool RestoreParticipant(int32 ParticipantIndex, bool bRespawn);
	void FinishLocalRound(bool bTimeExpired);
	bool EnableMatchMenuInput();
};
