// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "UrbanCarnageGameState.generated.h"

UENUM(BlueprintType)
enum class EGameModeType : uint8
{
    Deathmatch       UMETA(DisplayName = "Deathmatch"),
    TeamBattle       UMETA(DisplayName = "Team Battle"),
    CaptureTheFlag   UMETA(DisplayName = "Capture The Flag"),
    Survival         UMETA(DisplayName = "Survival"),
    Race             UMETA(DisplayName = "Race")
};

UENUM(BlueprintType)
enum class EGameState : uint8
{
    WaitingForPlayers    UMETA(DisplayName = "Waiting For Players"),
    Countdown            UMETA(DisplayName = "Countdown"),
    InProgress           UMETA(DisplayName = "In Progress"),
    Paused               UMETA(DisplayName = "Paused"),
    GameOver             UMETA(DisplayName = "Game Over")
};

USTRUCT(BlueprintType)
struct FPlayerStats
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    int32 Kills = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 Deaths = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 Assists = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 Score = 0;

    UPROPERTY(BlueprintReadOnly)
    float DamageDealt = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float DamageTaken = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    int32 FlagsCaptured = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 FlagsReturned = 0;

    FPlayerStats()
    {
        Kills = 0;
        Deaths = 0;
        Assists = 0;
        Score = 0;
        DamageDealt = 0.0f;
        DamageTaken = 0.0f;
        FlagsCaptured = 0;
        FlagsReturned = 0;
    }
};

UCLASS()
class URBANCARNAGE_API AUrbanCarnageGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    AUrbanCarnageGameState();

protected:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
    // Game Mode Settings
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game Settings")
    EGameModeType CurrentGameMode = EGameModeType::Deathmatch;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game Settings")
    EGameState CurrentGameState = EGameState::WaitingForPlayers;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game Settings")
    int32 MaxPlayers = 8;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game Settings")
    int32 ScoreLimit = 50;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game Settings")
    float TimeLimit = 600.0f; // 10 minutes

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game Settings")
    float RespawnTime = 5.0f;

    // Game Timer
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game Timer")
    float RemainingTime = 0.0f;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game Timer")
    float CountdownTime = 10.0f;

    // Player Management
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Players")
    TMap<APlayerState*, FPlayerStats> PlayerStats;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Players")
    TArray<APlayerState*> RedTeam;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Players")
    TArray<APlayerState*> BlueTeam;

    // Team Scores
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Teams")
    int32 RedTeamScore = 0;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Teams")
    int32 BlueTeamScore = 0;

    // Game Events
    UFUNCTION(BlueprintCallable, Category = "Game Events")
    void StartGame();

    UFUNCTION(BlueprintCallable, Category = "Game Events")
    void EndGame();

    UFUNCTION(BlueprintCallable, Category = "Game Events")
    void PauseGame();

    UFUNCTION(BlueprintCallable, Category = "Game Events")
    void ResumeGame();

    // Player Events
    UFUNCTION(BlueprintCallable, Category = "Player Events")
    void OnPlayerKilled(APlayerState* Killer, APlayerState* Victim, APlayerState* Assister = nullptr);

    UFUNCTION(BlueprintCallable, Category = "Player Events")
    void OnPlayerJoined(APlayerState* Player);

    UFUNCTION(BlueprintCallable, Category = "Player Events")
    void OnPlayerLeft(APlayerState* Player);

    // Team Management
    UFUNCTION(BlueprintCallable, Category = "Team Management")
    void AssignPlayerToTeam(APlayerState* Player);

    UFUNCTION(BlueprintCallable, Category = "Team Management")
    int32 GetTeamForPlayer(APlayerState* Player);

    // Statistics
    UFUNCTION(BlueprintCallable, Category = "Statistics")
    FPlayerStats GetPlayerStats(APlayerState* Player);

    UFUNCTION(BlueprintCallable, Category = "Statistics")
    void UpdatePlayerStats(APlayerState* Player, const FPlayerStats& NewStats);

    // Game State Updates
    UFUNCTION(BlueprintCallable, Category = "Game State")
    void UpdateGameState(EGameState NewState);

    UFUNCTION(BlueprintCallable, Category = "Game State")
    bool IsGameInProgress() const;

    UFUNCTION(BlueprintCallable, Category = "Game State")
    bool IsGameOver() const;

    // Timer Management
    UFUNCTION(BlueprintCallable, Category = "Timer")
    void StartGameTimer();

    UFUNCTION(BlueprintCallable, Category = "Timer")
    void StopGameTimer();

    // Blueprint Events
    UFUNCTION(BlueprintImplementableEvent, Category = "Game Events")
    void OnGameStarted();

    UFUNCTION(BlueprintImplementableEvent, Category = "Game Events")
    void OnGameEnded();

    UFUNCTION(BlueprintImplementableEvent, Category = "Game Events")
    void OnPlayerKilledEvent(APlayerState* Killer, APlayerState* Victim, APlayerState* Assister);

    UFUNCTION(BlueprintImplementableEvent, Category = "Game Events")
    void OnTeamScoreUpdated(int32 RedScore, int32 BlueScore);

    UFUNCTION(BlueprintImplementableEvent, Category = "Game Events")
    void OnGameStateChanged(EGameState NewState);

protected:
    // Timer Handles
    FTimerHandle GameTimerHandle;
    FTimerHandle CountdownTimerHandle;

    // Internal Functions
    void UpdateGameTimer();
    void UpdateCountdownTimer();
    void CheckGameEndConditions();
    void ResetGame();

    // Server Functions
    UFUNCTION(Server, Reliable, Category = "Server")
    void Server_StartGame();

    UFUNCTION(Server, Reliable, Category = "Server")
    void Server_EndGame();

    UFUNCTION(Server, Reliable, Category = "Server")
    void Server_UpdatePlayerStats(APlayerState* Player, const FPlayerStats& NewStats);

    UFUNCTION(Server, Reliable, Category = "Server")
    void Server_AssignPlayerToTeam(APlayerState* Player);

    // Multicast Functions
    UFUNCTION(NetMulticast, Reliable, Category = "Multicast")
    void Multicast_OnPlayerKilled(APlayerState* Killer, APlayerState* Victim, APlayerState* Assister);

    UFUNCTION(NetMulticast, Reliable, Category = "Multicast")
    void Multicast_OnGameStateChanged(EGameState NewState);

    UFUNCTION(NetMulticast, Reliable, Category = "Multicast")
    void Multicast_OnTeamScoreUpdated(int32 RedScore, int32 BlueScore);
};