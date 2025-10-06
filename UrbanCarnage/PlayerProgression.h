// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PlayerProgression.generated.h"

UENUM(BlueprintType)
enum class EPlayerRank : uint8
{
    Rookie       UMETA(DisplayName = "Rookie"),
    Novice       UMETA(DisplayName = "Novice"),
    Experienced  UMETA(DisplayName = "Experienced"),
    Veteran      UMETA(DisplayName = "Veteran"),
    Expert       UMETA(DisplayName = "Expert"),
    Master       UMETA(DisplayName = "Master"),
    Legend       UMETA(DisplayName = "Legend")
};

USTRUCT(BlueprintType)
struct FPlayerStatistics
{
    GENERATED_BODY()

    // Combat Stats
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    int32 TotalKills = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    int32 TotalDeaths = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    int32 TotalAssists = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    int32 TotalDamageDealt = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    int32 TotalDamageTaken = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    int32 Headshots = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    int32 LongestKillStreak = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    int32 CurrentKillStreak = 0;

    // Game Stats
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
    int32 GamesPlayed = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
    int32 GamesWon = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
    int32 GamesLost = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
    float TotalPlayTime = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
    int32 TotalScore = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
    int32 HighestScore = 0;

    // Vehicle Stats
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
    float TotalDistanceDriven = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
    float TopSpeed = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
    int32 Crashes = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
    int32 Jumps = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
    float AirTime = 0.0f;

    // Weapon Stats
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
    int32 ShotsFired = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
    int32 ShotsHit = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
    int32 Reloads = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
    TMap<FString, int32> WeaponKills;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
    TMap<FString, int32> WeaponShots;

    // Team Stats
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
    int32 FlagsCaptured = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
    int32 FlagsReturned = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
    int32 TeamKills = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
    int32 TeamDeaths = 0;

    FPlayerStatistics()
    {
        TotalKills = 0;
        TotalDeaths = 0;
        TotalAssists = 0;
        TotalDamageDealt = 0;
        TotalDamageTaken = 0;
        Headshots = 0;
        LongestKillStreak = 0;
        CurrentKillStreak = 0;
        GamesPlayed = 0;
        GamesWon = 0;
        GamesLost = 0;
        TotalPlayTime = 0.0f;
        TotalScore = 0;
        HighestScore = 0;
        TotalDistanceDriven = 0.0f;
        TopSpeed = 0.0f;
        Crashes = 0;
        Jumps = 0;
        AirTime = 0.0f;
        ShotsFired = 0;
        ShotsHit = 0;
        Reloads = 0;
        FlagsCaptured = 0;
        FlagsReturned = 0;
        TeamKills = 0;
        TeamDeaths = 0;
    }
};

USTRUCT(BlueprintType)
struct FAchievement
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
    FString AchievementID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
    FString AchievementName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
    FString AchievementDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
    int32 RequiredValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
    int32 CurrentValue = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
    bool bIsUnlocked = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
    int32 ExperienceReward = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
    FString IconPath;

    FAchievement()
    {
        AchievementID = "";
        AchievementName = "";
        AchievementDescription = "";
        RequiredValue = 0;
        CurrentValue = 0;
        bIsUnlocked = false;
        ExperienceReward = 100;
        IconPath = "";
    }
};

USTRUCT(BlueprintType)
struct FUnlockableItem
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unlockable")
    FString ItemID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unlockable")
    FString ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unlockable")
    FString ItemDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unlockable")
    int32 RequiredLevel = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unlockable")
    int32 RequiredExperience = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unlockable")
    bool bIsUnlocked = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unlockable")
    FString ItemType; // "Vehicle", "Weapon", "Skin", "Emote", etc.

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unlockable")
    FString IconPath;

    FUnlockableItem()
    {
        ItemID = "";
        ItemName = "";
        ItemDescription = "";
        RequiredLevel = 1;
        RequiredExperience = 0;
        bIsUnlocked = false;
        ItemType = "";
        IconPath = "";
    }
};

UCLASS()
class URBANCARNAGE_API UPlayerProgression : public USaveGame
{
    GENERATED_BODY()

public:
    UPlayerProgression();

    // Player Info
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
    FString PlayerName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
    int32 PlayerLevel = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
    int32 CurrentExperience = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
    int32 ExperienceToNextLevel = 1000;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
    EPlayerRank CurrentRank = EPlayerRank::Rookie;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
    int32 TotalExperience = 0;

    // Statistics
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Statistics")
    FPlayerStatistics Statistics;

    // Achievements
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievements")
    TArray<FAchievement> Achievements;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievements")
    TArray<FString> UnlockedAchievements;

    // Unlockables
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unlockables")
    TArray<FUnlockableItem> UnlockableItems;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unlockables")
    TArray<FString> UnlockedItems;

    // Customization
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
    FString SelectedVehicleSkin = "Default";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
    FString SelectedWeaponSkin = "Default";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
    FString SelectedEmote = "Default";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
    FLinearColor VehicleColor = FLinearColor::White;

    // Progression Functions
    UFUNCTION(BlueprintCallable, Category = "Progression")
    void AddExperience(int32 Experience);

    UFUNCTION(BlueprintCallable, Category = "Progression")
    void LevelUp();

    UFUNCTION(BlueprintCallable, Category = "Progression")
    void UpdateRank();

    UFUNCTION(BlueprintCallable, Category = "Progression")
    int32 GetExperienceToNextLevel() const;

    UFUNCTION(BlueprintCallable, Category = "Progression")
    float GetLevelProgress() const;

    // Statistics Functions
    UFUNCTION(BlueprintCallable, Category = "Statistics")
    void UpdateKillStats(int32 Kills, int32 Deaths, int32 Assists);

    UFUNCTION(BlueprintCallable, Category = "Statistics")
    void UpdateDamageStats(int32 DamageDealt, int32 DamageTaken);

    UFUNCTION(BlueprintCallable, Category = "Statistics")
    void UpdateGameStats(bool bWon, int32 Score, float PlayTime);

    UFUNCTION(BlueprintCallable, Category = "Statistics")
    void UpdateVehicleStats(float Distance, float Speed, int32 Crashes, int32 Jumps, float AirTime);

    UFUNCTION(BlueprintCallable, Category = "Statistics")
    void UpdateWeaponStats(int32 ShotsFired, int32 ShotsHit, int32 Reloads, const FString& WeaponName);

    UFUNCTION(BlueprintCallable, Category = "Statistics")
    void UpdateTeamStats(int32 FlagsCaptured, int32 FlagsReturned, int32 TeamKills, int32 TeamDeaths);

    UFUNCTION(BlueprintCallable, Category = "Statistics")
    float GetKillDeathRatio() const;

    UFUNCTION(BlueprintCallable, Category = "Statistics")
    float GetWinRate() const;

    UFUNCTION(BlueprintCallable, Category = "Statistics")
    float GetAccuracy() const;

    // Achievement Functions
    UFUNCTION(BlueprintCallable, Category = "Achievements")
    void CheckAchievements();

    UFUNCTION(BlueprintCallable, Category = "Achievements")
    void UnlockAchievement(const FString& AchievementID);

    UFUNCTION(BlueprintCallable, Category = "Achievements")
    bool IsAchievementUnlocked(const FString& AchievementID) const;

    UFUNCTION(BlueprintCallable, Category = "Achievements")
    TArray<FAchievement> GetUnlockedAchievements() const;

    UFUNCTION(BlueprintCallable, Category = "Achievements")
    TArray<FAchievement> GetLockedAchievements() const;

    // Unlockable Functions
    UFUNCTION(BlueprintCallable, Category = "Unlockables")
    void CheckUnlockables();

    UFUNCTION(BlueprintCallable, Category = "Unlockables")
    void UnlockItem(const FString& ItemID);

    UFUNCTION(BlueprintCallable, Category = "Unlockables")
    bool IsItemUnlocked(const FString& ItemID) const;

    UFUNCTION(BlueprintCallable, Category = "Unlockables")
    TArray<FUnlockableItem> GetUnlockedItems() const;

    UFUNCTION(BlueprintCallable, Category = "Unlockables")
    TArray<FUnlockableItem> GetLockedItems() const;

    // Customization Functions
    UFUNCTION(BlueprintCallable, Category = "Customization")
    void SetVehicleSkin(const FString& SkinName);

    UFUNCTION(BlueprintCallable, Category = "Customization")
    void SetWeaponSkin(const FString& SkinName);

    UFUNCTION(BlueprintCallable, Category = "Customization")
    void SetEmote(const FString& EmoteName);

    UFUNCTION(BlueprintCallable, Category = "Customization")
    void SetVehicleColor(FLinearColor Color);

    // Save/Load Functions
    UFUNCTION(BlueprintCallable, Category = "Save/Load")
    void SaveProgression();

    UFUNCTION(BlueprintCallable, Category = "Save/Load")
    void LoadProgression();

    UFUNCTION(BlueprintCallable, Category = "Save/Load")
    void ResetProgression();

    // Blueprint Events
    UFUNCTION(BlueprintImplementableEvent, Category = "Progression Events")
    void OnLevelUp(int32 NewLevel);

    UFUNCTION(BlueprintImplementableEvent, Category = "Progression Events")
    void OnRankUp(EPlayerRank NewRank);

    UFUNCTION(BlueprintImplementableEvent, Category = "Progression Events")
    void OnAchievementUnlocked(const FString& AchievementID);

    UFUNCTION(BlueprintImplementableEvent, Category = "Progression Events")
    void OnItemUnlocked(const FString& ItemID);

    UFUNCTION(BlueprintImplementableEvent, Category = "Progression Events")
    void OnExperienceGained(int32 Experience, int32 TotalExperience);

protected:
    // Internal Functions
    void InitializeAchievements();
    void InitializeUnlockables();
    void CalculateExperienceToNextLevel();
    void CheckLevelUp();
    void CheckRankUp();
    void AwardAchievement(const FString& AchievementID);
    void AwardItem(const FString& ItemID);

    // Constants
    static const int32 BASE_EXPERIENCE_PER_LEVEL = 1000;
    static const float EXPERIENCE_MULTIPLIER = 1.2f;
    static const int32 MAX_LEVEL = 100;
};