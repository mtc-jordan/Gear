// Copyright Epic Games, Inc. All Rights Reserved.

#include "LevelManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"

ALevelManager::ALevelManager()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = false;
}

void ALevelManager::BeginPlay()
{
    Super::BeginPlay();
    
    InitializeLevels();
    SetupCurrentLevel();
}

void ALevelManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    UpdateWeatherEffects();
}

void ALevelManager::LoadLevel(ELevelType LevelType)
{
    if (AvailableLevels.Contains(LevelType))
    {
        CurrentLevel = LevelType;
        CurrentLevelData = AvailableLevels[LevelType];
        
        // Load the level
        FString LevelPath = CurrentLevelData.MapPath;
        if (!LevelPath.IsEmpty())
        {
            UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelPath));
            bLevelLoaded = true;
            CurrentLevelName = CurrentLevelData.LevelName;
            
            OnLevelLoaded(LevelType);
            OnLevelLoadedEvent(LevelType);
        }
    }
}

void ALevelManager::LoadLevelByName(const FString& LevelName)
{
    for (const auto& LevelPair : AvailableLevels)
    {
        if (LevelPair.Value.LevelName == LevelName)
        {
            LoadLevel(LevelPair.Key);
            break;
        }
    }
}

void ALevelManager::UnloadCurrentLevel()
{
    if (bLevelLoaded)
    {
        bLevelLoaded = false;
        CurrentLevelName = "";
        
        OnLevelUnloaded();
        OnLevelUnloadedEvent();
    }
}

void ALevelManager::RestartCurrentLevel()
{
    if (bLevelLoaded)
    {
        LoadLevel(CurrentLevel);
    }
}

void ALevelManager::GetAvailableLevels(TArray<ELevelType>& OutLevels)
{
    AvailableLevels.GetKeys(OutLevels);
}

FLevelData ALevelManager::GetLevelData(ELevelType LevelType)
{
    if (AvailableLevels.Contains(LevelType))
    {
        return AvailableLevels[LevelType];
    }
    return FLevelData();
}

bool ALevelManager::IsLevelLoaded() const
{
    return bLevelLoaded;
}

ELevelType ALevelManager::GetCurrentLevel() const
{
    return CurrentLevel;
}

FVector ALevelManager::GetRandomPlayerSpawnPoint()
{
    if (CurrentLevelData.PlayerSpawnPoints.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, CurrentLevelData.PlayerSpawnPoints.Num() - 1);
        return CurrentLevelData.PlayerSpawnPoints[RandomIndex];
    }
    return FVector::ZeroVector;
}

FVector ALevelManager::GetRandomAISpawnPoint()
{
    if (CurrentLevelData.AISpawnPoints.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, CurrentLevelData.AISpawnPoints.Num() - 1);
        return CurrentLevelData.AISpawnPoints[RandomIndex];
    }
    return FVector::ZeroVector;
}

FVector ALevelManager::GetRandomFlagSpawnPoint()
{
    if (CurrentLevelData.FlagSpawnPoints.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, CurrentLevelData.FlagSpawnPoints.Num() - 1);
        return CurrentLevelData.FlagSpawnPoints[RandomIndex];
    }
    return FVector::ZeroVector;
}

FVector ALevelManager::GetRandomPowerUpSpawnPoint()
{
    if (CurrentLevelData.PowerUpSpawnPoints.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, CurrentLevelData.PowerUpSpawnPoints.Num() - 1);
        return CurrentLevelData.PowerUpSpawnPoints[RandomIndex];
    }
    return FVector::ZeroVector;
}

FVector ALevelManager::GetRandomWeaponSpawnPoint()
{
    if (CurrentLevelData.WeaponSpawnPoints.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, CurrentLevelData.WeaponSpawnPoints.Num() - 1);
        return CurrentLevelData.WeaponSpawnPoints[RandomIndex];
    }
    return FVector::ZeroVector;
}

TArray<FVector> ALevelManager::GetPlayerSpawnPoints()
{
    return CurrentLevelData.PlayerSpawnPoints;
}

TArray<FVector> ALevelManager::GetAISpawnPoints()
{
    return CurrentLevelData.AISpawnPoints;
}

TArray<FVector> ALevelManager::GetFlagSpawnPoints()
{
    return CurrentLevelData.FlagSpawnPoints;
}

TArray<FVector> ALevelManager::GetPowerUpSpawnPoints()
{
    return CurrentLevelData.PowerUpSpawnPoints;
}

TArray<FVector> ALevelManager::GetWeaponSpawnPoints()
{
    return CurrentLevelData.WeaponSpawnPoints;
}

void ALevelManager::SetWeatherSettings(const FWeatherSettings& NewSettings)
{
    WeatherSettings = NewSettings;
    ApplyWeatherSettings();
    OnWeatherChanged();
    OnWeatherChangedEvent();
}

void ALevelManager::EnableWeather(bool bEnable)
{
    WeatherSettings.bEnableWeather = bEnable;
    ApplyWeatherSettings();
    OnWeatherChanged();
    OnWeatherChangedEvent();
}

void ALevelManager::SetRainIntensity(float Intensity)
{
    WeatherSettings.RainIntensity = FMath::Clamp(Intensity, 0.0f, 1.0f);
    ApplyWeatherSettings();
    OnWeatherChanged();
    OnWeatherChangedEvent();
}

void ALevelManager::SetSnowIntensity(float Intensity)
{
    WeatherSettings.SnowIntensity = FMath::Clamp(Intensity, 0.0f, 1.0f);
    ApplyWeatherSettings();
    OnWeatherChanged();
    OnWeatherChangedEvent();
}

void ALevelManager::SetWindStrength(float Strength)
{
    WeatherSettings.WindStrength = FMath::Clamp(Strength, 0.0f, 1.0f);
    ApplyWeatherSettings();
    OnWeatherChanged();
    OnWeatherChangedEvent();
}

void ALevelManager::SetFogDensity(float Density)
{
    WeatherSettings.FogDensity = FMath::Clamp(Density, 0.0f, 1.0f);
    ApplyWeatherSettings();
    OnWeatherChanged();
    OnWeatherChangedEvent();
}

void ALevelManager::SetFogColor(FLinearColor Color)
{
    WeatherSettings.FogColor = Color;
    ApplyWeatherSettings();
    OnWeatherChanged();
    OnWeatherChangedEvent();
}

void ALevelManager::SetDayNightCycle(float Cycle)
{
    WeatherSettings.DayNightCycle = FMath::Clamp(Cycle, 0.0f, 1.0f);
    ApplyWeatherSettings();
    OnWeatherChanged();
    OnWeatherChangedEvent();
}

void ALevelManager::RandomizeWeather()
{
    GenerateRandomWeather();
    ApplyWeatherSettings();
    OnWeatherChanged();
    OnWeatherChangedEvent();
}

void ALevelManager::UpdateWeather()
{
    if (bRandomWeather && WeatherSettings.bEnableWeather)
    {
        RandomizeWeather();
    }
}

void ALevelManager::OnLevelLoaded(ELevelType LevelType)
{
    // Set up weather for the level
    if (bRandomWeather)
    {
        RandomizeWeather();
    }
    
    // Start weather change timer if enabled
    if (bRandomWeather && WeatherChangeInterval > 0.0f)
    {
        GetWorldTimerManager().SetTimer(WeatherChangeTimerHandle, this, &ALevelManager::UpdateWeather, WeatherChangeInterval, true);
    }
}

void ALevelManager::OnLevelUnloaded()
{
    // Stop weather change timer
    GetWorldTimerManager().ClearTimer(WeatherChangeTimerHandle);
    
    // Reset weather
    WeatherSettings = FWeatherSettings();
    ApplyWeatherSettings();
}

void ALevelManager::OnWeatherChanged()
{
    // Update current weather state
    CurrentRainIntensity = WeatherSettings.RainIntensity;
    CurrentSnowIntensity = WeatherSettings.SnowIntensity;
    CurrentWindStrength = WeatherSettings.WindStrength;
    CurrentFogDensity = WeatherSettings.FogDensity;
    CurrentFogColor = WeatherSettings.FogColor;
    CurrentDayNightCycle = WeatherSettings.DayNightCycle;
}

void ALevelManager::InitializeLevels()
{
    // Urban Arena
    FLevelData UrbanArenaData;
    UrbanArenaData.LevelName = "Urban Arena";
    UrbanArenaData.LevelDescription = "A futuristic city arena with high-rise buildings and neon lights";
    UrbanArenaData.LevelType = ELevelType::UrbanArena;
    UrbanArenaData.MapPath = "/Game/Maps/UrbanArena";
    UrbanArenaData.MaxPlayers = 8;
    UrbanArenaData.LevelSize = 2000.0f;
    UrbanArenaData.bSupportsTeamBattle = true;
    UrbanArenaData.bSupportsCaptureTheFlag = true;
    UrbanArenaData.bSupportsSurvival = true;
    UrbanArenaData.bSupportsRace = false;
    
    // Generate spawn points in a circle
    float Radius = 800.0f;
    int32 NumPoints = 8;
    for (int32 i = 0; i < NumPoints; i++)
    {
        float Angle = (i * 360.0f) / NumPoints;
        FVector SpawnPoint = FVector(
            FMath::Cos(FMath::DegreesToRadians(Angle)) * Radius,
            FMath::Sin(FMath::DegreesToRadians(Angle)) * Radius,
            0.0f
        );
        UrbanArenaData.PlayerSpawnPoints.Add(SpawnPoint);
        UrbanArenaData.AISpawnPoints.Add(SpawnPoint + FVector(0, 0, 100));
    }
    
    AvailableLevels.Add(ELevelType::UrbanArena, UrbanArenaData);
    
    // Desert Wasteland
    FLevelData DesertData;
    DesertData.LevelName = "Desert Wasteland";
    DesertData.LevelDescription = "A vast desert with sand dunes and abandoned structures";
    DesertData.LevelType = ELevelType::DesertWasteland;
    DesertData.MapPath = "/Game/Maps/DesertWasteland";
    DesertData.MaxPlayers = 12;
    DesertData.LevelSize = 3000.0f;
    DesertData.bSupportsTeamBattle = true;
    DesertData.bSupportsCaptureTheFlag = true;
    DesertData.bSupportsSurvival = true;
    DesertData.bSupportsRace = true;
    
    // Generate spawn points
    for (int32 i = 0; i < 12; i++)
    {
        float Angle = (i * 360.0f) / 12;
        float Distance = FMath::RandRange(600.0f, 1200.0f);
        FVector SpawnPoint = FVector(
            FMath::Cos(FMath::DegreesToRadians(Angle)) * Distance,
            FMath::Sin(FMath::DegreesToRadians(Angle)) * Distance,
            0.0f
        );
        DesertData.PlayerSpawnPoints.Add(SpawnPoint);
        DesertData.AISpawnPoints.Add(SpawnPoint + FVector(0, 0, 50));
    }
    
    AvailableLevels.Add(ELevelType::DesertWasteland, DesertData);
    
    // Arctic Base
    FLevelData ArcticData;
    ArcticData.LevelName = "Arctic Base";
    ArcticData.LevelDescription = "A frozen military base with ice and snow";
    ArcticData.LevelType = ELevelType::ArcticBase;
    ArcticData.MapPath = "/Game/Maps/ArcticBase";
    ArcticData.MaxPlayers = 6;
    ArcticData.LevelSize = 1500.0f;
    ArcticData.bSupportsTeamBattle = true;
    ArcticData.bSupportsCaptureTheFlag = false;
    ArcticData.bSupportsSurvival = true;
    ArcticData.bSupportsRace = false;
    
    // Generate spawn points
    for (int32 i = 0; i < 6; i++)
    {
        float Angle = (i * 360.0f) / 6;
        FVector SpawnPoint = FVector(
            FMath::Cos(FMath::DegreesToRadians(Angle)) * 600.0f,
            FMath::Sin(FMath::DegreesToRadians(Angle)) * 600.0f,
            0.0f
        );
        ArcticData.PlayerSpawnPoints.Add(SpawnPoint);
        ArcticData.AISpawnPoints.Add(SpawnPoint + FVector(0, 0, 50));
    }
    
    AvailableLevels.Add(ELevelType::ArcticBase, ArcticData);
    
    // Jungle Temple
    FLevelData JungleData;
    JungleData.LevelName = "Jungle Temple";
    JungleData.LevelDescription = "An ancient temple hidden in the jungle";
    JungleData.LevelType = ELevelType::JungleTemple;
    JungleData.MapPath = "/Game/Maps/JungleTemple";
    JungleData.MaxPlayers = 10;
    JungleData.LevelSize = 2500.0f;
    JungleData.bSupportsTeamBattle = true;
    JungleData.bSupportsCaptureTheFlag = true;
    JungleData.bSupportsSurvival = true;
    JungleData.bSupportsRace = false;
    
    // Generate spawn points
    for (int32 i = 0; i < 10; i++)
    {
        float Angle = (i * 360.0f) / 10;
        FVector SpawnPoint = FVector(
            FMath::Cos(FMath::DegreesToRadians(Angle)) * 700.0f,
            FMath::Sin(FMath::DegreesToRadians(Angle)) * 700.0f,
            0.0f
        );
        JungleData.PlayerSpawnPoints.Add(SpawnPoint);
        JungleData.AISpawnPoints.Add(SpawnPoint + FVector(0, 0, 100));
    }
    
    AvailableLevels.Add(ELevelType::JungleTemple, JungleData);
    
    // Space Station
    FLevelData SpaceData;
    SpaceData.LevelName = "Space Station";
    SpaceData.LevelDescription = "A futuristic space station in zero gravity";
    SpaceData.LevelType = ELevelType::SpaceStation;
    SpaceData.MapPath = "/Game/Maps/SpaceStation";
    SpaceData.MaxPlayers = 8;
    SpaceData.LevelSize = 1800.0f;
    SpaceData.bSupportsTeamBattle = true;
    SpaceData.bSupportsCaptureTheFlag = true;
    SpaceData.bSupportsSurvival = true;
    SpaceData.bSupportsRace = false;
    
    // Generate spawn points
    for (int32 i = 0; i < 8; i++)
    {
        float Angle = (i * 360.0f) / 8;
        FVector SpawnPoint = FVector(
            FMath::Cos(FMath::DegreesToRadians(Angle)) * 600.0f,
            FMath::Sin(FMath::DegreesToRadians(Angle)) * 600.0f,
            0.0f
        );
        SpaceData.PlayerSpawnPoints.Add(SpawnPoint);
        SpaceData.AISpawnPoints.Add(SpawnPoint + FVector(0, 0, 50));
    }
    
    AvailableLevels.Add(ELevelType::SpaceStation, SpaceData);
    
    // Test Level
    FLevelData TestData;
    TestData.LevelName = "Test Level";
    TestData.LevelDescription = "A simple test level for development";
    TestData.LevelType = ELevelType::TestLevel;
    TestData.MapPath = "/Game/Maps/TestLevel";
    TestData.MaxPlayers = 4;
    TestData.LevelSize = 1000.0f;
    TestData.bSupportsTeamBattle = true;
    TestData.bSupportsCaptureTheFlag = true;
    TestData.bSupportsSurvival = true;
    TestData.bSupportsRace = true;
    
    // Generate spawn points
    for (int32 i = 0; i < 4; i++)
    {
        float Angle = (i * 360.0f) / 4;
        FVector SpawnPoint = FVector(
            FMath::Cos(FMath::DegreesToRadians(Angle)) * 400.0f,
            FMath::Sin(FMath::DegreesToRadians(Angle)) * 400.0f,
            0.0f
        );
        TestData.PlayerSpawnPoints.Add(SpawnPoint);
        TestData.AISpawnPoints.Add(SpawnPoint + FVector(0, 0, 50));
    }
    
    AvailableLevels.Add(ELevelType::TestLevel, TestData);
}

void ALevelManager::SetupCurrentLevel()
{
    if (AvailableLevels.Contains(CurrentLevel))
    {
        CurrentLevelData = AvailableLevels[CurrentLevel];
        bLevelLoaded = true;
        CurrentLevelName = CurrentLevelData.LevelName;
    }
}

void ALevelManager::UpdateWeatherEffects()
{
    if (WeatherSettings.bEnableWeather)
    {
        // Update weather effects based on current settings
        // This would involve updating particle systems, lighting, etc.
    }
}

void ALevelManager::GenerateRandomWeather()
{
    WeatherSettings.RainIntensity = FMath::RandRange(0.0f, 1.0f);
    WeatherSettings.SnowIntensity = FMath::RandRange(0.0f, 1.0f);
    WeatherSettings.WindStrength = FMath::RandRange(0.0f, 1.0f);
    WeatherSettings.FogDensity = FMath::RandRange(0.0f, 0.8f);
    WeatherSettings.DayNightCycle = FMath::RandRange(0.0f, 1.0f);
    
    // Randomize fog color
    float R = FMath::RandRange(0.5f, 1.0f);
    float G = FMath::RandRange(0.5f, 1.0f);
    float B = FMath::RandRange(0.5f, 1.0f);
    WeatherSettings.FogColor = FLinearColor(R, G, B, 1.0f);
}

void ALevelManager::ApplyWeatherSettings()
{
    // Apply weather settings to the level
    // This would involve setting up particle systems, lighting, fog, etc.
    bWeatherEnabled = WeatherSettings.bEnableWeather;
}