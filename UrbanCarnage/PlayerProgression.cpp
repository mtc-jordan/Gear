// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlayerProgression.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

UPlayerProgression::UPlayerProgression()
{
    SaveSlotName = TEXT("PlayerProgression");
    UserIndex = 0;
    
    PlayerName = "Player";
    PlayerLevel = 1;
    CurrentExperience = 0;
    ExperienceToNextLevel = BASE_EXPERIENCE_PER_LEVEL;
    CurrentRank = EPlayerRank::Rookie;
    TotalExperience = 0;
    
    InitializeAchievements();
    InitializeUnlockables();
}

void UPlayerProgression::AddExperience(int32 Experience)
{
    if (Experience <= 0)
    {
        return;
    }
    
    CurrentExperience += Experience;
    TotalExperience += Experience;
    
    OnExperienceGained(Experience, TotalExperience);
    
    CheckLevelUp();
    CheckAchievements();
    CheckUnlockables();
}

void UPlayerProgression::LevelUp()
{
    PlayerLevel++;
    CurrentExperience -= ExperienceToNextLevel;
    CalculateExperienceToNextLevel();
    UpdateRank();
    
    OnLevelUp(PlayerLevel);
    CheckUnlockables();
}

void UPlayerProgression::UpdateRank()
{
    EPlayerRank NewRank = CurrentRank;
    
    if (PlayerLevel >= 50)
    {
        NewRank = EPlayerRank::Legend;
    }
    else if (PlayerLevel >= 40)
    {
        NewRank = EPlayerRank::Master;
    }
    else if (PlayerLevel >= 30)
    {
        NewRank = EPlayerRank::Expert;
    }
    else if (PlayerLevel >= 20)
    {
        NewRank = EPlayerRank::Veteran;
    }
    else if (PlayerLevel >= 10)
    {
        NewRank = EPlayerRank::Experienced;
    }
    else if (PlayerLevel >= 5)
    {
        NewRank = EPlayerRank::Novice;
    }
    else
    {
        NewRank = EPlayerRank::Rookie;
    }
    
    if (NewRank != CurrentRank)
    {
        CurrentRank = NewRank;
        OnRankUp(NewRank);
    }
}

int32 UPlayerProgression::GetExperienceToNextLevel() const
{
    return ExperienceToNextLevel;
}

float UPlayerProgression::GetLevelProgress() const
{
    if (ExperienceToNextLevel <= 0)
    {
        return 1.0f;
    }
    
    return (float)CurrentExperience / (float)ExperienceToNextLevel;
}

void UPlayerProgression::UpdateKillStats(int32 Kills, int32 Deaths, int32 Assists)
{
    Statistics.TotalKills += Kills;
    Statistics.TotalDeaths += Deaths;
    Statistics.TotalAssists += Assists;
    
    // Update kill streak
    if (Kills > 0)
    {
        Statistics.CurrentKillStreak += Kills;
        if (Statistics.CurrentKillStreak > Statistics.LongestKillStreak)
        {
            Statistics.LongestKillStreak = Statistics.CurrentKillStreak;
        }
    }
    
    if (Deaths > 0)
    {
        Statistics.CurrentKillStreak = 0;
    }
    
    CheckAchievements();
}

void UPlayerProgression::UpdateDamageStats(int32 DamageDealt, int32 DamageTaken)
{
    Statistics.TotalDamageDealt += DamageDealt;
    Statistics.TotalDamageTaken += DamageTaken;
    
    CheckAchievements();
}

void UPlayerProgression::UpdateGameStats(bool bWon, int32 Score, float PlayTime)
{
    Statistics.GamesPlayed++;
    
    if (bWon)
    {
        Statistics.GamesWon++;
    }
    else
    {
        Statistics.GamesLost++;
    }
    
    Statistics.TotalPlayTime += PlayTime;
    Statistics.TotalScore += Score;
    
    if (Score > Statistics.HighestScore)
    {
        Statistics.HighestScore = Score;
    }
    
    CheckAchievements();
}

void UPlayerProgression::UpdateVehicleStats(float Distance, float Speed, int32 Crashes, int32 Jumps, float AirTime)
{
    Statistics.TotalDistanceDriven += Distance;
    
    if (Speed > Statistics.TopSpeed)
    {
        Statistics.TopSpeed = Speed;
    }
    
    Statistics.Crashes += Crashes;
    Statistics.Jumps += Jumps;
    Statistics.AirTime += AirTime;
    
    CheckAchievements();
}

void UPlayerProgression::UpdateWeaponStats(int32 ShotsFired, int32 ShotsHit, int32 Reloads, const FString& WeaponName)
{
    Statistics.ShotsFired += ShotsFired;
    Statistics.ShotsHit += ShotsHit;
    Statistics.Reloads += Reloads;
    
    if (!WeaponName.IsEmpty())
    {
        if (int32* WeaponKills = Statistics.WeaponKills.Find(WeaponName))
        {
            (*WeaponKills)++;
        }
        else
        {
            Statistics.WeaponKills.Add(WeaponName, 1);
        }
        
        if (int32* WeaponShots = Statistics.WeaponShots.Find(WeaponName))
        {
            (*WeaponShots) += ShotsFired;
        }
        else
        {
            Statistics.WeaponShots.Add(WeaponName, ShotsFired);
        }
    }
    
    CheckAchievements();
}

void UPlayerProgression::UpdateTeamStats(int32 FlagsCaptured, int32 FlagsReturned, int32 TeamKills, int32 TeamDeaths)
{
    Statistics.FlagsCaptured += FlagsCaptured;
    Statistics.FlagsReturned += FlagsReturned;
    Statistics.TeamKills += TeamKills;
    Statistics.TeamDeaths += TeamDeaths;
    
    CheckAchievements();
}

float UPlayerProgression::GetKillDeathRatio() const
{
    if (Statistics.TotalDeaths == 0)
    {
        return Statistics.TotalKills > 0 ? (float)Statistics.TotalKills : 0.0f;
    }
    
    return (float)Statistics.TotalKills / (float)Statistics.TotalDeaths;
}

float UPlayerProgression::GetWinRate() const
{
    if (Statistics.GamesPlayed == 0)
    {
        return 0.0f;
    }
    
    return (float)Statistics.GamesWon / (float)Statistics.GamesPlayed;
}

float UPlayerProgression::GetAccuracy() const
{
    if (Statistics.ShotsFired == 0)
    {
        return 0.0f;
    }
    
    return (float)Statistics.ShotsHit / (float)Statistics.ShotsFired;
}

void UPlayerProgression::CheckAchievements()
{
    for (FAchievement& Achievement : Achievements)
    {
        if (Achievement.bIsUnlocked)
        {
            continue;
        }
        
        bool bShouldUnlock = false;
        
        // Check different achievement types
        if (Achievement.AchievementID == "FirstKill" && Statistics.TotalKills >= 1)
        {
            bShouldUnlock = true;
        }
        else if (Achievement.AchievementID == "KillStreak5" && Statistics.LongestKillStreak >= 5)
        {
            bShouldUnlock = true;
        }
        else if (Achievement.AchievementID == "KillStreak10" && Statistics.LongestKillStreak >= 10)
        {
            bShouldUnlock = true;
        }
        else if (Achievement.AchievementID == "FirstWin" && Statistics.GamesWon >= 1)
        {
            bShouldUnlock = true;
        }
        else if (Achievement.AchievementID == "WinStreak5" && Statistics.GamesWon >= 5)
        {
            bShouldUnlock = true;
        }
        else if (Achievement.AchievementID == "Level10" && PlayerLevel >= 10)
        {
            bShouldUnlock = true;
        }
        else if (Achievement.AchievementID == "Level25" && PlayerLevel >= 25)
        {
            bShouldUnlock = true;
        }
        else if (Achievement.AchievementID == "Level50" && PlayerLevel >= 50)
        {
            bShouldUnlock = true;
        }
        else if (Achievement.AchievementID == "Distance1000" && Statistics.TotalDistanceDriven >= 1000.0f)
        {
            bShouldUnlock = true;
        }
        else if (Achievement.AchievementID == "Speed200" && Statistics.TopSpeed >= 200.0f)
        {
            bShouldUnlock = true;
        }
        else if (Achievement.AchievementID == "Accuracy50" && GetAccuracy() >= 0.5f)
        {
            bShouldUnlock = true;
        }
        else if (Achievement.AchievementID == "Accuracy75" && GetAccuracy() >= 0.75f)
        {
            bShouldUnlock = true;
        }
        else if (Achievement.AchievementID == "PlayTime10" && Statistics.TotalPlayTime >= 600.0f) // 10 hours
        {
            bShouldUnlock = true;
        }
        else if (Achievement.AchievementID == "PlayTime50" && Statistics.TotalPlayTime >= 3000.0f) // 50 hours
        {
            bShouldUnlock = true;
        }
        
        if (bShouldUnlock)
        {
            UnlockAchievement(Achievement.AchievementID);
        }
    }
}

void UPlayerProgression::UnlockAchievement(const FString& AchievementID)
{
    for (FAchievement& Achievement : Achievements)
    {
        if (Achievement.AchievementID == AchievementID && !Achievement.bIsUnlocked)
        {
            Achievement.bIsUnlocked = true;
            UnlockedAchievements.Add(AchievementID);
            
            // Award experience
            AddExperience(Achievement.ExperienceReward);
            
            OnAchievementUnlocked(AchievementID);
            break;
        }
    }
}

bool UPlayerProgression::IsAchievementUnlocked(const FString& AchievementID) const
{
    return UnlockedAchievements.Contains(AchievementID);
}

TArray<FAchievement> UPlayerProgression::GetUnlockedAchievements() const
{
    TArray<FAchievement> Unlocked;
    for (const FAchievement& Achievement : Achievements)
    {
        if (Achievement.bIsUnlocked)
        {
            Unlocked.Add(Achievement);
        }
    }
    return Unlocked;
}

TArray<FAchievement> UPlayerProgression::GetLockedAchievements() const
{
    TArray<FAchievement> Locked;
    for (const FAchievement& Achievement : Achievements)
    {
        if (!Achievement.bIsUnlocked)
        {
            Locked.Add(Achievement);
        }
    }
    return Locked;
}

void UPlayerProgression::CheckUnlockables()
{
    for (FUnlockableItem& Item : UnlockableItems)
    {
        if (Item.bIsUnlocked)
        {
            continue;
        }
        
        bool bShouldUnlock = false;
        
        // Check level requirement
        if (PlayerLevel >= Item.RequiredLevel)
        {
            // Check experience requirement
            if (TotalExperience >= Item.RequiredExperience)
            {
                bShouldUnlock = true;
            }
        }
        
        if (bShouldUnlock)
        {
            UnlockItem(Item.ItemID);
        }
    }
}

void UPlayerProgression::UnlockItem(const FString& ItemID)
{
    for (FUnlockableItem& Item : UnlockableItems)
    {
        if (Item.ItemID == ItemID && !Item.bIsUnlocked)
        {
            Item.bIsUnlocked = true;
            UnlockedItems.Add(ItemID);
            
            OnItemUnlocked(ItemID);
            break;
        }
    }
}

bool UPlayerProgression::IsItemUnlocked(const FString& ItemID) const
{
    return UnlockedItems.Contains(ItemID);
}

TArray<FUnlockableItem> UPlayerProgression::GetUnlockedItems() const
{
    TArray<FUnlockableItem> Unlocked;
    for (const FUnlockableItem& Item : UnlockableItems)
    {
        if (Item.bIsUnlocked)
        {
            Unlocked.Add(Item);
        }
    }
    return Unlocked;
}

TArray<FUnlockableItem> UPlayerProgression::GetLockedItems() const
{
    TArray<FUnlockableItem> Locked;
    for (const FUnlockableItem& Item : UnlockableItems)
    {
        if (!Item.bIsUnlocked)
        {
            Locked.Add(Item);
        }
    }
    return Locked;
}

void UPlayerProgression::SetVehicleSkin(const FString& SkinName)
{
    if (IsItemUnlocked(SkinName))
    {
        SelectedVehicleSkin = SkinName;
    }
}

void UPlayerProgression::SetWeaponSkin(const FString& SkinName)
{
    if (IsItemUnlocked(SkinName))
    {
        SelectedWeaponSkin = SkinName;
    }
}

void UPlayerProgression::SetEmote(const FString& EmoteName)
{
    if (IsItemUnlocked(EmoteName))
    {
        SelectedEmote = EmoteName;
    }
}

void UPlayerProgression::SetVehicleColor(FLinearColor Color)
{
    VehicleColor = Color;
}

void UPlayerProgression::SaveProgression()
{
    UGameplayStatics::SaveGameToSlot(this, SaveSlotName, UserIndex);
}

void UPlayerProgression::LoadProgression()
{
    if (UPlayerProgression* LoadedProgression = Cast<UPlayerProgression>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex)))
    {
        PlayerName = LoadedProgression->PlayerName;
        PlayerLevel = LoadedProgression->PlayerLevel;
        CurrentExperience = LoadedProgression->CurrentExperience;
        ExperienceToNextLevel = LoadedProgression->ExperienceToNextLevel;
        CurrentRank = LoadedProgression->CurrentRank;
        TotalExperience = LoadedProgression->TotalExperience;
        Statistics = LoadedProgression->Statistics;
        Achievements = LoadedProgression->Achievements;
        UnlockedAchievements = LoadedProgression->UnlockedAchievements;
        UnlockableItems = LoadedProgression->UnlockableItems;
        UnlockedItems = LoadedProgression->UnlockedItems;
        SelectedVehicleSkin = LoadedProgression->SelectedVehicleSkin;
        SelectedWeaponSkin = LoadedProgression->SelectedWeaponSkin;
        SelectedEmote = LoadedProgression->SelectedEmote;
        VehicleColor = LoadedProgression->VehicleColor;
    }
}

void UPlayerProgression::ResetProgression()
{
    PlayerLevel = 1;
    CurrentExperience = 0;
    ExperienceToNextLevel = BASE_EXPERIENCE_PER_LEVEL;
    CurrentRank = EPlayerRank::Rookie;
    TotalExperience = 0;
    Statistics = FPlayerStatistics();
    
    for (FAchievement& Achievement : Achievements)
    {
        Achievement.bIsUnlocked = false;
    }
    
    for (FUnlockableItem& Item : UnlockableItems)
    {
        Item.bIsUnlocked = false;
    }
    
    UnlockedAchievements.Empty();
    UnlockedItems.Empty();
    
    SelectedVehicleSkin = "Default";
    SelectedWeaponSkin = "Default";
    SelectedEmote = "Default";
    VehicleColor = FLinearColor::White;
}

void UPlayerProgression::InitializeAchievements()
{
    Achievements.Empty();
    
    // Combat Achievements
    FAchievement FirstKill;
    FirstKill.AchievementID = "FirstKill";
    FirstKill.AchievementName = "First Blood";
    FirstKill.AchievementDescription = "Get your first kill";
    FirstKill.RequiredValue = 1;
    FirstKill.ExperienceReward = 100;
    Achievements.Add(FirstKill);
    
    FAchievement KillStreak5;
    KillStreak5.AchievementID = "KillStreak5";
    KillStreak5.AchievementName = "Killing Spree";
    KillStreak5.AchievementDescription = "Get a 5 kill streak";
    KillStreak5.RequiredValue = 5;
    KillStreak5.ExperienceReward = 250;
    Achievements.Add(KillStreak5);
    
    FAchievement KillStreak10;
    KillStreak10.AchievementID = "KillStreak10";
    KillStreak10.AchievementName = "Rampage";
    KillStreak10.AchievementDescription = "Get a 10 kill streak";
    KillStreak10.RequiredValue = 10;
    KillStreak10.ExperienceReward = 500;
    Achievements.Add(KillStreak10);
    
    // Level Achievements
    FAchievement Level10;
    Level10.AchievementID = "Level10";
    Level10.AchievementName = "Getting Started";
    Level10.AchievementDescription = "Reach level 10";
    Level10.RequiredValue = 10;
    Level10.ExperienceReward = 200;
    Achievements.Add(Level10);
    
    FAchievement Level25;
    Level25.AchievementID = "Level25";
    Level25.AchievementName = "Experienced";
    Level25.AchievementDescription = "Reach level 25";
    Level25.RequiredValue = 25;
    Level25.ExperienceReward = 500;
    Achievements.Add(Level25);
    
    FAchievement Level50;
    Level50.AchievementID = "Level50";
    Level50.AchievementName = "Veteran";
    Level50.AchievementDescription = "Reach level 50";
    Level50.RequiredValue = 50;
    Level50.ExperienceReward = 1000;
    Achievements.Add(Level50);
    
    // Vehicle Achievements
    FAchievement Distance1000;
    Distance1000.AchievementID = "Distance1000";
    Distance1000.AchievementName = "Road Warrior";
    Distance1000.AchievementDescription = "Drive 1000 kilometers";
    Distance1000.RequiredValue = 1000;
    Distance1000.ExperienceReward = 300;
    Achievements.Add(Distance1000);
    
    FAchievement Speed200;
    Speed200.AchievementID = "Speed200";
    Speed200.AchievementName = "Speed Demon";
    Speed200.AchievementDescription = "Reach 200 km/h";
    Speed200.RequiredValue = 200;
    Speed200.ExperienceReward = 250;
    Achievements.Add(Speed200);
    
    // Accuracy Achievements
    FAchievement Accuracy50;
    Accuracy50.AchievementID = "Accuracy50";
    Accuracy50.AchievementName = "Marksman";
    Accuracy50.AchievementDescription = "Achieve 50% accuracy";
    Accuracy50.RequiredValue = 50;
    Accuracy50.ExperienceReward = 200;
    Achievements.Add(Accuracy50);
    
    FAchievement Accuracy75;
    Accuracy75.AchievementID = "Accuracy75";
    Accuracy75.AchievementName = "Sniper";
    Accuracy75.AchievementDescription = "Achieve 75% accuracy";
    Accuracy75.RequiredValue = 75;
    Accuracy75.ExperienceReward = 400;
    Achievements.Add(Accuracy75);
    
    // Play Time Achievements
    FAchievement PlayTime10;
    PlayTime10.AchievementID = "PlayTime10";
    PlayTime10.AchievementName = "Dedicated";
    PlayTime10.AchievementDescription = "Play for 10 hours";
    PlayTime10.RequiredValue = 600; // 10 hours in minutes
    PlayTime10.ExperienceReward = 500;
    Achievements.Add(PlayTime10);
    
    FAchievement PlayTime50;
    PlayTime50.AchievementID = "PlayTime50";
    PlayTime50.AchievementName = "Addicted";
    PlayTime50.AchievementDescription = "Play for 50 hours";
    PlayTime50.RequiredValue = 3000; // 50 hours in minutes
    PlayTime50.ExperienceReward = 1000;
    Achievements.Add(PlayTime50);
}

void UPlayerProgression::InitializeUnlockables()
{
    UnlockableItems.Empty();
    
    // Vehicle Skins
    FUnlockableItem RedSkin;
    RedSkin.ItemID = "VehicleSkin_Red";
    RedSkin.ItemName = "Red Skin";
    RedSkin.ItemDescription = "A bright red vehicle skin";
    RedSkin.RequiredLevel = 2;
    RedSkin.RequiredExperience = 500;
    RedSkin.ItemType = "VehicleSkin";
    UnlockableItems.Add(RedSkin);
    
    FUnlockableItem BlueSkin;
    BlueSkin.ItemID = "VehicleSkin_Blue";
    BlueSkin.ItemName = "Blue Skin";
    BlueSkin.ItemDescription = "A cool blue vehicle skin";
    BlueSkin.RequiredLevel = 3;
    BlueSkin.RequiredExperience = 750;
    BlueSkin.ItemType = "VehicleSkin";
    UnlockableItems.Add(BlueSkin);
    
    FUnlockableItem GoldSkin;
    GoldSkin.ItemID = "VehicleSkin_Gold";
    GoldSkin.ItemName = "Gold Skin";
    GoldSkin.ItemDescription = "A luxurious gold vehicle skin";
    GoldSkin.RequiredLevel = 10;
    GoldSkin.RequiredExperience = 2000;
    GoldSkin.ItemType = "VehicleSkin";
    UnlockableItems.Add(GoldSkin);
    
    // Weapon Skins
    FUnlockableItem CamoWeapon;
    CamoWeapon.ItemID = "WeaponSkin_Camo";
    CamoWeapon.ItemName = "Camo Weapon";
    CamoWeapon.ItemDescription = "A camouflage weapon skin";
    CamoWeapon.RequiredLevel = 5;
    CamoWeapon.RequiredExperience = 1000;
    CamoWeapon.ItemType = "WeaponSkin";
    UnlockableItems.Add(CamoWeapon);
    
    // Emotes
    FUnlockableItem WaveEmote;
    WaveEmote.ItemID = "Emote_Wave";
    WaveEmote.ItemName = "Wave";
    WaveEmote.ItemDescription = "Wave to other players";
    WaveEmote.RequiredLevel = 1;
    WaveEmote.RequiredExperience = 100;
    WaveEmote.ItemType = "Emote";
    UnlockableItems.Add(WaveEmote);
    
    FUnlockableItem DanceEmote;
    DanceEmote.ItemID = "Emote_Dance";
    DanceEmote.ItemName = "Dance";
    DanceEmote.ItemDescription = "Dance to celebrate";
    DanceEmote.RequiredLevel = 8;
    DanceEmote.RequiredExperience = 1500;
    DanceEmote.ItemType = "Emote";
    UnlockableItems.Add(DanceEmote);
}

void UPlayerProgression::CalculateExperienceToNextLevel()
{
    ExperienceToNextLevel = FMath::RoundToInt(BASE_EXPERIENCE_PER_LEVEL * FMath::Pow(EXPERIENCE_MULTIPLIER, PlayerLevel - 1));
}

void UPlayerProgression::CheckLevelUp()
{
    while (CurrentExperience >= ExperienceToNextLevel && PlayerLevel < MAX_LEVEL)
    {
        LevelUp();
    }
}

void UPlayerProgression::CheckRankUp()
{
    UpdateRank();
}

void UPlayerProgression::AwardAchievement(const FString& AchievementID)
{
    UnlockAchievement(AchievementID);
}

void UPlayerProgression::AwardItem(const FString& ItemID)
{
    UnlockItem(ItemID);
}