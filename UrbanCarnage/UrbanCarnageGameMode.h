// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UrbanCarnageGameState.h"
#include "UrbanCarnageHUD.h"
#include "UrbanCarnageGameMode.generated.h"

UCLASS()
class URBANCARNAGE_API AUrbanCarnageGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AUrbanCarnageGameMode();

protected:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual void Tick(float DeltaTime) override;

public:
	// Game Mode Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Settings")
	EGameModeType DefaultGameMode = EGameModeType::Deathmatch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Settings")
	int32 MaxPlayers = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Settings")
	int32 ScoreLimit = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Settings")
	float TimeLimit = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Settings")
	float RespawnTime = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Settings")
	bool bAllowFriendlyFire = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Settings")
	bool bAllowTeamKills = false;

	// AI Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
	bool bSpawnAIEnemies = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
	int32 MaxAIEnemies = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
	TSubclassOf<AAIEnemyVehicle> AIEnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
	TArray<FVector> AISpawnPoints;

	// Game Functions
	UFUNCTION(BlueprintCallable, Category = "Game")
	void StartGame();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void EndGame();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void PauseGame();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void ResumeGame();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void RestartGame();

	// Player Management
	UFUNCTION(BlueprintCallable, Category = "Player Management")
	void RespawnPlayer(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Player Management")
	void RespawnAllPlayers();

	UFUNCTION(BlueprintCallable, Category = "Player Management")
	void KillPlayer(APlayerController* PlayerController);

	// AI Management
	UFUNCTION(BlueprintCallable, Category = "AI Management")
	void SpawnAIEnemies();

	UFUNCTION(BlueprintCallable, Category = "AI Management")
	void RemoveAllAIEnemies();

	UFUNCTION(BlueprintCallable, Category = "AI Management")
	void SetAIDifficulty(EAIDifficulty Difficulty);

	// Game Events
	UFUNCTION(BlueprintCallable, Category = "Game Events")
	void OnPlayerKilled(APlayerController* Killer, APlayerController* Victim, APlayerController* Assister = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Game Events")
	void OnPlayerJoined(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Game Events")
	void OnPlayerLeft(APlayerController* PlayerController);

	// Blueprint Events
	UFUNCTION(BlueprintImplementableEvent, Category = "Game Events")
	void OnGameStarted();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Events")
	void OnGameEnded();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Events")
	void OnPlayerKilledEvent(APlayerController* Killer, APlayerController* Victim, APlayerController* Assister);

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Events")
	void OnPlayerJoinedEvent(APlayerController* PlayerController);

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Events")
	void OnPlayerLeftEvent(APlayerController* PlayerController);

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Events")
	void OnAISpawned(AAIEnemyVehicle* AIEnemy);

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Events")
	void OnAIKilled(AAIEnemyVehicle* AIEnemy, APlayerController* Killer);

protected:
	// Internal Functions
	void InitializeGameMode();
	void UpdateGameState();
	void CheckGameEndConditions();
	void SpawnAIEnemyAtLocation(const FVector& SpawnLocation);
	void CleanupAIEnemies();

	// Timer Handles
	FTimerHandle GameStartTimerHandle;
	FTimerHandle RespawnTimerHandle;
	FTimerHandle AISpawnTimerHandle;

	// AI Management
	UPROPERTY(BlueprintReadOnly, Category = "AI Management")
	TArray<AAIEnemyVehicle*> SpawnedAIEnemies;

	UPROPERTY(BlueprintReadOnly, Category = "AI Management")
	EAIDifficulty CurrentAIDifficulty = EAIDifficulty::Medium;

	// Game State Reference
	UPROPERTY(BlueprintReadOnly, Category = "Game State")
	AUrbanCarnageGameState* UrbanCarnageGameState;

	// HUD Reference
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	AUrbanCarnageHUD* UrbanCarnageHUD;
};GameMode();
};



