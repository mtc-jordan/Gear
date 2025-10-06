// Copyright Epic Games, Inc. All Rights Reserved.

#include "UrbanCarnageGameMode.h"
#include "UrbanCarnagePlayerController.h"
#include "UrbanCarnageGameState.h"
#include "UrbanCarnageHUD.h"
#include "UrbanCarnagePawn.h"
#include "AIEnemyVehicle.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

AUrbanCarnageGameMode::AUrbanCarnageGameMode()
{
	PlayerControllerClass = AUrbanCarnagePlayerController::StaticClass();
	PrimaryActorTick.bCanEverTick = true;
	
	// Set default game state and HUD classes
	GameStateClass = AUrbanCarnageGameState::StaticClass();
	HUDClass = AUrbanCarnageHUD::StaticClass();
	
	// Set default pawn class
	DefaultPawnClass = AUrbanCarnagePawn::StaticClass();
	
	// Initialize AI settings
	bSpawnAIEnemies = true;
	MaxAIEnemies = 4;
	CurrentAIDifficulty = EAIDifficulty::Medium;
}

void AUrbanCarnageGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeGameMode();
}

void AUrbanCarnageGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	OnPlayerJoined(NewPlayer);
}

void AUrbanCarnageGameMode::Logout(AController* Exiting)
{
	OnPlayerLeft(Cast<APlayerController>(Exiting));
	
	Super::Logout(Exiting);
}

void AUrbanCarnageGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdateGameState();
}

void AUrbanCarnageGameMode::StartGame()
{
	if (UrbanCarnageGameState)
	{
		UrbanCarnageGameState->StartGame();
		OnGameStarted();
		
		// Spawn AI enemies if enabled
		if (bSpawnAIEnemies)
		{
			SpawnAIEnemies();
		}
	}
}

void AUrbanCarnageGameMode::EndGame()
{
	if (UrbanCarnageGameState)
	{
		UrbanCarnageGameState->EndGame();
		OnGameEnded();
		
		// Clean up AI enemies
		RemoveAllAIEnemies();
	}
}

void AUrbanCarnageGameMode::PauseGame()
{
	if (UrbanCarnageGameState)
	{
		UrbanCarnageGameState->PauseGame();
	}
}

void AUrbanCarnageGameMode::ResumeGame()
{
	if (UrbanCarnageGameState)
	{
		UrbanCarnageGameState->ResumeGame();
	}
}

void AUrbanCarnageGameMode::RestartGame()
{
	// Reset game state
	if (UrbanCarnageGameState)
	{
		UrbanCarnageGameState->ResetGame();
	}
	
	// Respawn all players
	RespawnAllPlayers();
	
	// Clean up and respawn AI
	RemoveAllAIEnemies();
	if (bSpawnAIEnemies)
	{
		SpawnAIEnemies();
	}
	
	// Start new game
	StartGame();
}

void AUrbanCarnageGameMode::RespawnPlayer(APlayerController* PlayerController)
{
	if (!PlayerController)
	{
		return;
	}
	
	// Get spawn location
	FVector SpawnLocation = FVector::ZeroVector;
	FRotator SpawnRotation = FRotator::ZeroRotator;
	
	if (AActor* StartSpot = FindPlayerStart(PlayerController))
	{
		SpawnLocation = StartSpot->GetActorLocation();
		SpawnRotation = StartSpot->GetActorRotation();
	}
	
	// Spawn new pawn
	if (APawn* NewPawn = SpawnDefaultPawnAtTransform(PlayerController, FTransform(SpawnRotation, SpawnLocation)))
	{
		PlayerController->Possess(NewPawn);
	}
}

void AUrbanCarnageGameMode::RespawnAllPlayers()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (APlayerController* PC = Iterator->Get())
		{
			RespawnPlayer(PC);
		}
	}
}

void AUrbanCarnageGameMode::KillPlayer(APlayerController* PlayerController)
{
	if (PlayerController && PlayerController->GetPawn())
	{
		// Apply damage to kill the player
		if (UHealthComponent* HealthComp = PlayerController->GetPawn()->GetComponentByClass<UHealthComponent>())
		{
			FDamageInfo DamageInfo;
			DamageInfo.DamageAmount = HealthComp->GetMaxHealth();
			DamageInfo.DamageType = EDamageType::Environmental;
			HealthComp->TakeDamage(DamageInfo);
		}
	}
}

void AUrbanCarnageGameMode::SpawnAIEnemies()
{
	if (!AIEnemyClass)
	{
		return;
	}
	
	int32 EnemiesToSpawn = FMath::Min(MaxAIEnemies, AISpawnPoints.Num());
	
	for (int32 i = 0; i < EnemiesToSpawn; i++)
	{
		if (i < AISpawnPoints.Num())
		{
			SpawnAIEnemyAtLocation(AISpawnPoints[i]);
		}
	}
}

void AUrbanCarnageGameMode::RemoveAllAIEnemies()
{
	for (AAIEnemyVehicle* AIEnemy : SpawnedAIEnemies)
	{
		if (AIEnemy)
		{
			AIEnemy->Destroy();
		}
	}
	SpawnedAIEnemies.Empty();
}

void AUrbanCarnageGameMode::SetAIDifficulty(EAIDifficulty Difficulty)
{
	CurrentAIDifficulty = Difficulty;
	
	// Update existing AI enemies
	for (AAIEnemyVehicle* AIEnemy : SpawnedAIEnemies)
	{
		if (AIEnemy)
		{
			AIEnemy->Difficulty = Difficulty;
			AIEnemy->UpdateDifficultySettings();
		}
	}
}

void AUrbanCarnageGameMode::OnPlayerKilled(APlayerController* Killer, APlayerController* Victim, APlayerController* Assister)
{
	if (UrbanCarnageGameState)
	{
		APlayerState* KillerState = Killer ? Killer->PlayerState : nullptr;
		APlayerState* VictimState = Victim ? Victim->PlayerState : nullptr;
		APlayerState* AssisterState = Assister ? Assister->PlayerState : nullptr;
		
		UrbanCarnageGameState->OnPlayerKilled(KillerState, VictimState, AssisterState);
	}
	
	OnPlayerKilledEvent(Killer, Victim, Assister);
	
	// Respawn victim after delay
	if (Victim)
	{
		GetWorldTimerManager().SetTimer(RespawnTimerHandle, [this, Victim]()
		{
			RespawnPlayer(Victim);
		}, RespawnTime, false);
	}
}

void AUrbanCarnageGameMode::OnPlayerJoined(APlayerController* PlayerController)
{
	if (UrbanCarnageGameState)
	{
		UrbanCarnageGameState->OnPlayerJoined(PlayerController->PlayerState);
	}
	
	OnPlayerJoinedEvent(PlayerController);
}

void AUrbanCarnageGameMode::OnPlayerLeft(APlayerController* PlayerController)
{
	if (UrbanCarnageGameState)
	{
		UrbanCarnageGameState->OnPlayerLeft(PlayerController->PlayerState);
	}
	
	OnPlayerLeftEvent(PlayerController);
}

void AUrbanCarnageGameMode::InitializeGameMode()
{
	// Get references to game state and HUD
	UrbanCarnageGameState = GetGameState<AUrbanCarnageGameState>();
	UrbanCarnageHUD = Cast<AUrbanCarnageHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	
	// Set game mode settings
	if (UrbanCarnageGameState)
	{
		UrbanCarnageGameState->CurrentGameMode = DefaultGameMode;
		UrbanCarnageGameState->MaxPlayers = MaxPlayers;
		UrbanCarnageGameState->ScoreLimit = ScoreLimit;
		UrbanCarnageGameState->TimeLimit = TimeLimit;
		UrbanCarnageGameState->RespawnTime = RespawnTime;
	}
	
	// Set up AI spawn points if none are defined
	if (AISpawnPoints.Num() == 0)
	{
		// Create default spawn points in a circle
		float Radius = 2000.0f;
		int32 NumPoints = 8;
		
		for (int32 i = 0; i < NumPoints; i++)
		{
			float Angle = (i * 360.0f) / NumPoints;
			FVector SpawnPoint = FVector(
				FMath::Cos(FMath::DegreesToRadians(Angle)) * Radius,
				FMath::Sin(FMath::DegreesToRadians(Angle)) * Radius,
				0.0f
			);
			AISpawnPoints.Add(SpawnPoint);
		}
	}
}

void AUrbanCarnageGameMode::UpdateGameState()
{
	if (UrbanCarnageGameState)
	{
		CheckGameEndConditions();
	}
}

void AUrbanCarnageGameMode::CheckGameEndConditions()
{
	if (!UrbanCarnageGameState || UrbanCarnageGameState->IsGameOver())
	{
		return;
	}
	
	// Check if game should end based on current game mode
	bool bShouldEndGame = false;
	
	switch (UrbanCarnageGameState->CurrentGameMode)
	{
		case EGameModeType::Deathmatch:
			// Check if any player reached score limit
			for (const auto& PlayerStat : UrbanCarnageGameState->PlayerStats)
			{
				if (PlayerStat.Value.Score >= ScoreLimit)
				{
					bShouldEndGame = true;
					break;
				}
			}
			break;
			
		case EGameModeType::TeamBattle:
			// Check if any team reached score limit
			if (UrbanCarnageGameState->RedTeamScore >= ScoreLimit || UrbanCarnageGameState->BlueTeamScore >= ScoreLimit)
			{
				bShouldEndGame = true;
			}
			break;
			
		case EGameModeType::Survival:
			// Check if only one player remains
			if (GetWorld()->GetNumPlayers() <= 1)
			{
				bShouldEndGame = true;
			}
			break;
	}
	
	if (bShouldEndGame)
	{
		EndGame();
	}
}

void AUrbanCarnageGameMode::SpawnAIEnemyAtLocation(const FVector& SpawnLocation)
{
	if (!AIEnemyClass)
	{
		return;
	}
	
	// Spawn AI enemy
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	AAIEnemyVehicle* AIEnemy = GetWorld()->SpawnActor<AAIEnemyVehicle>(AIEnemyClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
	
	if (AIEnemy)
	{
		AIEnemy->Difficulty = CurrentAIDifficulty;
		AIEnemy->UpdateDifficultySettings();
		SpawnedAIEnemies.Add(AIEnemy);
		OnAISpawned(AIEnemy);
	}
}

void AUrbanCarnageGameMode::CleanupAIEnemies()
{
	// Remove dead AI enemies
	SpawnedAIEnemies.RemoveAll([](AAIEnemyVehicle* AIEnemy)
	{
		return !AIEnemy || AIEnemy->IsPendingKill();
	});
}ss();
}
