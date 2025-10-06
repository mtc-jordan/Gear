// Copyright Epic Games, Inc. All Rights Reserved.

#include "UrbanCarnageGameState.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerState.h"
#include "TimerManager.h"

AUrbanCarnageGameState::AUrbanCarnageGameState()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
}

void AUrbanCarnageGameState::BeginPlay()
{
    Super::BeginPlay();
    
    if (HasAuthority())
    {
        RemainingTime = TimeLimit;
        UpdateGameState(EGameState::WaitingForPlayers);
    }
}

void AUrbanCarnageGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(AUrbanCarnageGameState, CurrentGameMode);
    DOREPLIFETIME(AUrbanCarnageGameState, CurrentGameState);
    DOREPLIFETIME(AUrbanCarnageGameState, MaxPlayers);
    DOREPLIFETIME(AUrbanCarnageGameState, ScoreLimit);
    DOREPLIFETIME(AUrbanCarnageGameState, TimeLimit);
    DOREPLIFETIME(AUrbanCarnageGameState, RespawnTime);
    DOREPLIFETIME(AUrbanCarnageGameState, RemainingTime);
    DOREPLIFETIME(AUrbanCarnageGameState, CountdownTime);
    DOREPLIFETIME(AUrbanCarnageGameState, PlayerStats);
    DOREPLIFETIME(AUrbanCarnageGameState, RedTeam);
    DOREPLIFETIME(AUrbanCarnageGameState, BlueTeam);
    DOREPLIFETIME(AUrbanCarnageGameState, RedTeamScore);
    DOREPLIFETIME(AUrbanCarnageGameState, BlueTeamScore);
}

void AUrbanCarnageGameState::StartGame()
{
    if (HasAuthority())
    {
        Server_StartGame();
    }
}

void AUrbanCarnageGameState::EndGame()
{
    if (HasAuthority())
    {
        Server_EndGame();
    }
}

void AUrbanCarnageGameState::PauseGame()
{
    if (HasAuthority() && CurrentGameState == EGameState::InProgress)
    {
        UpdateGameState(EGameState::Paused);
        GetWorldTimerManager().PauseTimer(GameTimerHandle);
    }
}

void AUrbanCarnageGameState::ResumeGame()
{
    if (HasAuthority() && CurrentGameState == EGameState::Paused)
    {
        UpdateGameState(EGameState::InProgress);
        GetWorldTimerManager().UnPauseTimer(GameTimerHandle);
    }
}

void AUrbanCarnageGameState::OnPlayerKilled(APlayerState* Killer, APlayerState* Victim, APlayerState* Assister)
{
    if (HasAuthority())
    {
        // Update killer stats
        if (Killer && Killer != Victim)
        {
            FPlayerStats* KillerStats = PlayerStats.Find(Killer);
            if (KillerStats)
            {
                KillerStats->Kills++;
                KillerStats->Score += 100; // Base kill score
            }
        }

        // Update victim stats
        if (Victim)
        {
            FPlayerStats* VictimStats = PlayerStats.Find(Victim);
            if (VictimStats)
            {
                VictimStats->Deaths++;
            }
        }

        // Update assister stats
        if (Assister && Assister != Killer && Assister != Victim)
        {
            FPlayerStats* AssisterStats = PlayerStats.Find(Assister);
            if (AssisterStats)
            {
                AssisterStats->Assists++;
                AssisterStats->Score += 50; // Assist score
            }
        }

        // Update team scores for team-based game modes
        if (CurrentGameMode == EGameModeType::TeamBattle)
        {
            if (Killer && Victim)
            {
                int32 KillerTeam = GetTeamForPlayer(Killer);
                int32 VictimTeam = GetTeamForPlayer(Victim);
                
                if (KillerTeam != VictimTeam)
                {
                    if (KillerTeam == 0) // Red Team
                    {
                        RedTeamScore++;
                    }
                    else if (KillerTeam == 1) // Blue Team
                    {
                        BlueTeamScore++;
                    }
                    
                    Multicast_OnTeamScoreUpdated(RedTeamScore, BlueTeamScore);
                }
            }
        }

        Multicast_OnPlayerKilled(Killer, Victim, Assister);
        CheckGameEndConditions();
    }
}

void AUrbanCarnageGameState::OnPlayerJoined(APlayerState* Player)
{
    if (HasAuthority() && Player)
    {
        // Initialize player stats
        PlayerStats.Add(Player, FPlayerStats());
        
        // Assign to team for team-based modes
        if (CurrentGameMode == EGameModeType::TeamBattle)
        {
            AssignPlayerToTeam(Player);
        }
    }
}

void AUrbanCarnageGameState::OnPlayerLeft(APlayerState* Player)
{
    if (HasAuthority() && Player)
    {
        // Remove from team
        RedTeam.Remove(Player);
        BlueTeam.Remove(Player);
        
        // Remove stats
        PlayerStats.Remove(Player);
    }
}

void AUrbanCarnageGameState::AssignPlayerToTeam(APlayerState* Player)
{
    if (HasAuthority() && Player)
    {
        Server_AssignPlayerToTeam(Player);
    }
}

int32 AUrbanCarnageGameState::GetTeamForPlayer(APlayerState* Player)
{
    if (RedTeam.Contains(Player))
    {
        return 0; // Red Team
    }
    else if (BlueTeam.Contains(Player))
    {
        return 1; // Blue Team
    }
    return -1; // No team
}

FPlayerStats AUrbanCarnageGameState::GetPlayerStats(APlayerState* Player)
{
    if (PlayerStats.Contains(Player))
    {
        return PlayerStats[Player];
    }
    return FPlayerStats();
}

void AUrbanCarnageGameState::UpdatePlayerStats(APlayerState* Player, const FPlayerStats& NewStats)
{
    if (HasAuthority())
    {
        Server_UpdatePlayerStats(Player, NewStats);
    }
}

void AUrbanCarnageGameState::UpdateGameState(EGameState NewState)
{
    if (HasAuthority())
    {
        CurrentGameState = NewState;
        Multicast_OnGameStateChanged(NewState);
    }
}

bool AUrbanCarnageGameState::IsGameInProgress() const
{
    return CurrentGameState == EGameState::InProgress;
}

bool AUrbanCarnageGameState::IsGameOver() const
{
    return CurrentGameState == EGameState::GameOver;
}

void AUrbanCarnageGameState::StartGameTimer()
{
    if (HasAuthority())
    {
        GetWorldTimerManager().SetTimer(GameTimerHandle, this, &AUrbanCarnageGameState::UpdateGameTimer, 1.0f, true);
    }
}

void AUrbanCarnageGameState::StopGameTimer()
{
    if (HasAuthority())
    {
        GetWorldTimerManager().ClearTimer(GameTimerHandle);
    }
}

void AUrbanCarnageGameState::UpdateGameTimer()
{
    if (HasAuthority() && CurrentGameState == EGameState::InProgress)
    {
        RemainingTime -= 1.0f;
        
        if (RemainingTime <= 0.0f)
        {
            RemainingTime = 0.0f;
            EndGame();
        }
    }
}

void AUrbanCarnageGameState::UpdateCountdownTimer()
{
    if (HasAuthority())
    {
        CountdownTime -= 1.0f;
        
        if (CountdownTime <= 0.0f)
        {
            CountdownTime = 0.0f;
            GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
            UpdateGameState(EGameState::InProgress);
            StartGameTimer();
            OnGameStarted();
        }
    }
}

void AUrbanCarnageGameState::CheckGameEndConditions()
{
    if (HasAuthority())
    {
        bool bGameShouldEnd = false;
        
        switch (CurrentGameMode)
        {
            case EGameModeType::Deathmatch:
                // Check if any player reached score limit
                for (const auto& PlayerStat : PlayerStats)
                {
                    if (PlayerStat.Value.Score >= ScoreLimit)
                    {
                        bGameShouldEnd = true;
                        break;
                    }
                }
                break;
                
            case EGameModeType::TeamBattle:
                // Check if any team reached score limit
                if (RedTeamScore >= ScoreLimit || BlueTeamScore >= ScoreLimit)
                {
                    bGameShouldEnd = true;
                }
                break;
                
            case EGameModeType::Survival:
                // Check if only one player/team remains
                if (PlayerStats.Num() <= 1)
                {
                    bGameShouldEnd = true;
                }
                break;
        }
        
        if (bGameShouldEnd)
        {
            EndGame();
        }
    }
}

void AUrbanCarnageGameState::ResetGame()
{
    if (HasAuthority())
    {
        // Reset all scores and stats
        RedTeamScore = 0;
        BlueTeamScore = 0;
        RemainingTime = TimeLimit;
        CountdownTime = 10.0f;
        
        // Clear all player stats
        for (auto& PlayerStat : PlayerStats)
        {
            PlayerStat.Value = FPlayerStats();
        }
        
        // Clear teams
        RedTeam.Empty();
        BlueTeam.Empty();
        
        // Reset game state
        UpdateGameState(EGameState::WaitingForPlayers);
        
        // Clear timers
        GetWorldTimerManager().ClearTimer(GameTimerHandle);
        GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
    }
}

// Server Functions
void AUrbanCarnageGameState::Server_StartGame_Implementation()
{
    if (PlayerArray.Num() >= 2) // Minimum 2 players
    {
        UpdateGameState(EGameState::Countdown);
        GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &AUrbanCarnageGameState::UpdateCountdownTimer, 1.0f, true);
    }
}

void AUrbanCarnageGameState::Server_EndGame_Implementation()
{
    UpdateGameState(EGameState::GameOver);
    StopGameTimer();
    OnGameEnded();
}

void AUrbanCarnageGameState::Server_UpdatePlayerStats_Implementation(APlayerState* Player, const FPlayerStats& NewStats)
{
    if (Player)
    {
        PlayerStats.Add(Player, NewStats);
    }
}

void AUrbanCarnageGameState::Server_AssignPlayerToTeam_Implementation(APlayerState* Player)
{
    if (Player)
    {
        // Simple team balancing - assign to team with fewer players
        if (RedTeam.Num() <= BlueTeam.Num())
        {
            RedTeam.Add(Player);
        }
        else
        {
            BlueTeam.Add(Player);
        }
    }
}

// Multicast Functions
void AUrbanCarnageGameState::Multicast_OnPlayerKilled_Implementation(APlayerState* Killer, APlayerState* Victim, APlayerState* Assister)
{
    OnPlayerKilledEvent(Killer, Victim, Assister);
}

void AUrbanCarnageGameState::Multicast_OnGameStateChanged_Implementation(EGameState NewState)
{
    OnGameStateChanged(NewState);
}

void AUrbanCarnageGameState::Multicast_OnTeamScoreUpdated_Implementation(int32 RedScore, int32 BlueScore)
{
    OnTeamScoreUpdated(RedScore, BlueScore);
}