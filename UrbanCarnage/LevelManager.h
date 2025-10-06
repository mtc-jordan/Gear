// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "LevelManager.generated.h"

UENUM(BlueprintType)
enum class ELevelType : uint8
{
    UrbanArena      UMETA(DisplayName = "Urban Arena"),
    DesertWasteland UMETA(DisplayName = "Desert Wasteland"),
    ArcticBase      UMETA(DisplayName = "Arctic Base"),
    JungleTemple    UMETA(DisplayName = "Jungle Temple"),
    SpaceStation    UMETA(DisplayName = "Space Station"),
    TestLevel       UMETA(DisplayName = "Test Level")
};

USTRUCT(BlueprintType)
struct FLevelData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Data")
    FString LevelName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Data")
    FString LevelDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Data")
    ELevelType LevelType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Data")
    FString MapPath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Data")
    FString PreviewImagePath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Data")
    int32 MaxPlayers = 8;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Data")
    float LevelSize = 2000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Data")
    bool bSupportsTeamBattle = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Data")
    bool bSupportsCaptureTheFlag = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Data")
    bool bSupportsSurvival = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Data")
    bool bSupportsRace = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Data")
    TArray<FVector> PlayerSpawnPoints;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Data")
    TArray<FVector> AISpawnPoints;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Data")
    TArray<FVector> FlagSpawnPoints;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Data")
    TArray<FVector> PowerUpSpawnPoints;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Data")
    TArray<FVector> WeaponSpawnPoints;

    FLevelData()
    {
        LevelName = "Unknown Level";
        LevelDescription = "No description available";
        LevelType = ELevelType::UrbanArena;
        MapPath = "";
        PreviewImagePath = "";
        MaxPlayers = 8;
        LevelSize = 2000.0f;
        bSupportsTeamBattle = true;
        bSupportsCaptureTheFlag = true;
        bSupportsSurvival = true;
        bSupportsRace = false;
    }
};

USTRUCT(BlueprintType)
struct FWeatherSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    bool bEnableWeather = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    float RainIntensity = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    float SnowIntensity = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    float WindStrength = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    float FogDensity = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    FLinearColor FogColor = FLinearColor::White;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    float DayNightCycle = 0.5f; // 0.0 = midnight, 0.5 = noon, 1.0 = midnight

    FWeatherSettings()
    {
        bEnableWeather = false;
        RainIntensity = 0.0f;
        SnowIntensity = 0.0f;
        WindStrength = 0.0f;
        FogDensity = 0.0f;
        FogColor = FLinearColor::White;
        DayNightCycle = 0.5f;
    }
};

UCLASS()
class URBANCARNAGE_API ALevelManager : public AActor
{
    GENERATED_BODY()

public:
    ALevelManager();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
    // Level Data
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Management")
    TMap<ELevelType, FLevelData> AvailableLevels;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Management")
    ELevelType CurrentLevel = ELevelType::UrbanArena;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Management")
    FLevelData CurrentLevelData;

    // Weather Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    FWeatherSettings WeatherSettings;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    bool bRandomWeather = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    float WeatherChangeInterval = 300.0f; // 5 minutes

    // Level Functions
    UFUNCTION(BlueprintCallable, Category = "Level Management")
    void LoadLevel(ELevelType LevelType);

    UFUNCTION(BlueprintCallable, Category = "Level Management")
    void LoadLevelByName(const FString& LevelName);

    UFUNCTION(BlueprintCallable, Category = "Level Management")
    void UnloadCurrentLevel();

    UFUNCTION(BlueprintCallable, Category = "Level Management")
    void RestartCurrentLevel();

    UFUNCTION(BlueprintCallable, Category = "Level Management")
    void GetAvailableLevels(TArray<ELevelType>& OutLevels);

    UFUNCTION(BlueprintCallable, Category = "Level Management")
    FLevelData GetLevelData(ELevelType LevelType);

    UFUNCTION(BlueprintCallable, Category = "Level Management")
    bool IsLevelLoaded() const;

    UFUNCTION(BlueprintCallable, Category = "Level Management")
    ELevelType GetCurrentLevel() const;

    // Spawn Point Management
    UFUNCTION(BlueprintCallable, Category = "Spawn Points")
    FVector GetRandomPlayerSpawnPoint();

    UFUNCTION(BlueprintCallable, Category = "Spawn Points")
    FVector GetRandomAISpawnPoint();

    UFUNCTION(BlueprintCallable, Category = "Spawn Points")
    FVector GetRandomFlagSpawnPoint();

    UFUNCTION(BlueprintCallable, Category = "Spawn Points")
    FVector GetRandomPowerUpSpawnPoint();

    UFUNCTION(BlueprintCallable, Category = "Spawn Points")
    FVector GetRandomWeaponSpawnPoint();

    UFUNCTION(BlueprintCallable, Category = "Spawn Points")
    TArray<FVector> GetPlayerSpawnPoints();

    UFUNCTION(BlueprintCallable, Category = "Spawn Points")
    TArray<FVector> GetAISpawnPoints();

    UFUNCTION(BlueprintCallable, Category = "Spawn Points")
    TArray<FVector> GetFlagSpawnPoints();

    UFUNCTION(BlueprintCallable, Category = "Spawn Points")
    TArray<FVector> GetPowerUpSpawnPoints();

    UFUNCTION(BlueprintCallable, Category = "Spawn Points")
    TArray<FVector> GetWeaponSpawnPoints();

    // Weather Functions
    UFUNCTION(BlueprintCallable, Category = "Weather")
    void SetWeatherSettings(const FWeatherSettings& NewSettings);

    UFUNCTION(BlueprintCallable, Category = "Weather")
    void EnableWeather(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "Weather")
    void SetRainIntensity(float Intensity);

    UFUNCTION(BlueprintCallable, Category = "Weather")
    void SetSnowIntensity(float Intensity);

    UFUNCTION(BlueprintCallable, Category = "Weather")
    void SetWindStrength(float Strength);

    UFUNCTION(BlueprintCallable, Category = "Weather")
    void SetFogDensity(float Density);

    UFUNCTION(BlueprintCallable, Category = "Weather")
    void SetFogColor(FLinearColor Color);

    UFUNCTION(BlueprintCallable, Category = "Weather")
    void SetDayNightCycle(float Cycle);

    UFUNCTION(BlueprintCallable, Category = "Weather")
    void RandomizeWeather();

    UFUNCTION(BlueprintCallable, Category = "Weather")
    void UpdateWeather();

    // Level Events
    UFUNCTION(BlueprintCallable, Category = "Level Events")
    void OnLevelLoaded(ELevelType LevelType);

    UFUNCTION(BlueprintCallable, Category = "Level Events")
    void OnLevelUnloaded();

    UFUNCTION(BlueprintCallable, Category = "Level Events")
    void OnWeatherChanged();

    // Blueprint Events
    UFUNCTION(BlueprintImplementableEvent, Category = "Level Events")
    void OnLevelLoadedEvent(ELevelType LevelType);

    UFUNCTION(BlueprintImplementableEvent, Category = "Level Events")
    void OnLevelUnloadedEvent();

    UFUNCTION(BlueprintImplementableEvent, Category = "Level Events")
    void OnWeatherChangedEvent();

    UFUNCTION(BlueprintImplementableEvent, Category = "Level Events")
    void OnSpawnPointsUpdated();

protected:
    // Internal Functions
    void InitializeLevels();
    void SetupCurrentLevel();
    void UpdateWeatherEffects();
    void GenerateRandomWeather();
    void ApplyWeatherSettings();

    // Timer Handles
    FTimerHandle WeatherChangeTimerHandle;

    // Level State
    UPROPERTY(BlueprintReadOnly, Category = "Level State")
    bool bLevelLoaded = false;

    UPROPERTY(BlueprintReadOnly, Category = "Level State")
    FString CurrentLevelName;

    // Weather State
    UPROPERTY(BlueprintReadOnly, Category = "Weather State")
    bool bWeatherEnabled = false;

    UPROPERTY(BlueprintReadOnly, Category = "Weather State")
    float CurrentRainIntensity = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Weather State")
    float CurrentSnowIntensity = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Weather State")
    float CurrentWindStrength = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Weather State")
    float CurrentFogDensity = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Weather State")
    FLinearColor CurrentFogColor = FLinearColor::White;

    UPROPERTY(BlueprintReadOnly, Category = "Weather State")
    float CurrentDayNightCycle = 0.5f;
};