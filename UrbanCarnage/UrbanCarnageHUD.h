// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/Overlay.h"
#include "UrbanCarnageHUD.generated.h"

UCLASS()
class URBANCARNAGE_API AUrbanCarnageHUD : public AHUD
{
    GENERATED_BODY()

public:
    AUrbanCarnageHUD();

protected:
    virtual void BeginPlay() override;
    virtual void DrawHUD() override;
    virtual void Tick(float DeltaTime) override;

public:
    // HUD Widgets
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
    TSubclassOf<UUserWidget> MainHUDWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
    TSubclassOf<UUserWidget> PauseMenuWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
    TSubclassOf<UUserWidget> GameOverWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
    TSubclassOf<UUserWidget> ScoreboardWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
    TSubclassOf<UUserWidget> MinimapWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
    TSubclassOf<UUserWidget> ChatWidgetClass;

    // Current Widgets
    UPROPERTY(BlueprintReadOnly, Category = "HUD")
    UUserWidget* MainHUDWidget;

    UPROPERTY(BlueprintReadOnly, Category = "HUD")
    UUserWidget* PauseMenuWidget;

    UPROPERTY(BlueprintReadOnly, Category = "HUD")
    UUserWidget* GameOverWidget;

    UPROPERTY(BlueprintReadOnly, Category = "HUD")
    UUserWidget* ScoreboardWidget;

    UPROPERTY(BlueprintReadOnly, Category = "HUD")
    UUserWidget* MinimapWidget;

    UPROPERTY(BlueprintReadOnly, Category = "HUD")
    UUserWidget* ChatWidget;

    // HUD State
    UPROPERTY(BlueprintReadOnly, Category = "HUD")
    bool bIsPaused = false;

    UPROPERTY(BlueprintReadOnly, Category = "HUD")
    bool bShowScoreboard = false;

    UPROPERTY(BlueprintReadOnly, Category = "HUD")
    bool bShowMinimap = true;

    UPROPERTY(BlueprintReadOnly, Category = "HUD")
    bool bShowChat = true;

    // HUD Functions
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowMainHUD();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void HideMainHUD();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void TogglePauseMenu();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowPauseMenu();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void HidePauseMenu();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ToggleScoreboard();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowScoreboard();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void HideScoreboard();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ToggleMinimap();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowMinimap();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void HideMinimap();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ToggleChat();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowChat();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void HideChat();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowGameOver();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void HideGameOver();

    // HUD Updates
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateHealthBar(float CurrentHealth, float MaxHealth);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateArmorBar(float CurrentArmor, float MaxArmor);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateSpeedDisplay(float Speed);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateGearDisplay(int32 Gear);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateAmmoDisplay(int32 CurrentAmmo, int32 MaxAmmo);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateScoreDisplay(int32 Score);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateKillCount(int32 Kills);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateDeathCount(int32 Deaths);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateTimeDisplay(float RemainingTime);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateTeamScore(int32 RedScore, int32 BlueScore);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdatePlayerList();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateMinimap();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void AddChatMessage(const FString& PlayerName, const FString& Message, FLinearColor Color = FLinearColor::White);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowKillFeed(const FString& KillerName, const FString& VictimName, const FString& WeaponName);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowNotification(const FString& Message, FLinearColor Color = FLinearColor::White, float Duration = 3.0f);

    // Crosshair
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateCrosshair(bool bIsAiming, bool bCanFire);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowHitMarker();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowDamageIndicator(FVector DamageDirection, float DamageAmount);

    // Minimap
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void AddMinimapIcon(AActor* Actor, UTexture2D* Icon, FLinearColor Color = FLinearColor::White);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void RemoveMinimapIcon(AActor* Actor);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateMinimapIcon(AActor* Actor, FVector NewLocation);

    // Input Handling
    virtual void SetupInputComponent() override;

    UFUNCTION()
    void OnPausePressed();

    UFUNCTION()
    void OnScoreboardPressed();

    UFUNCTION()
    void OnScoreboardReleased();

    UFUNCTION()
    void OnChatPressed();

    UFUNCTION()
    void OnMinimapPressed();

    // Blueprint Events
    UFUNCTION(BlueprintImplementableEvent, Category = "HUD Events")
    void OnMainHUDShown();

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD Events")
    void OnMainHUDHidden();

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD Events")
    void OnPauseMenuShown();

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD Events")
    void OnPauseMenuHidden();

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD Events")
    void OnScoreboardShown();

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD Events")
    void OnScoreboardHidden();

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD Events")
    void OnGameOverShown();

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD Events")
    void OnHealthUpdated(float CurrentHealth, float MaxHealth);

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD Events")
    void OnSpeedUpdated(float Speed);

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD Events")
    void OnAmmoUpdated(int32 CurrentAmmo, int32 MaxAmmo);

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD Events")
    void OnScoreUpdated(int32 Score);

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD Events")
    void OnKillFeedUpdated(const FString& KillerName, const FString& VictimName, const FString& WeaponName);

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD Events")
    void OnNotificationShown(const FString& Message, FLinearColor Color, float Duration);

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD Events")
    void OnCrosshairUpdated(bool bIsAiming, bool bCanFire);

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD Events")
    void OnHitMarkerShown();

    UFUNCTION(BlueprintImplementableEvent, Category = "HUD Events")
    void OnDamageIndicatorShown(FVector DamageDirection, float DamageAmount);

protected:
    // Internal Functions
    void CreateHUDWidgets();
    void UpdateHUD();
    void DrawCrosshair();
    void DrawMinimap();
    void DrawHealthBar();
    void DrawSpeedometer();
    void DrawAmmoCounter();
    void DrawScoreDisplay();
    void DrawKillFeed();
    void DrawNotifications();

    // Widget References
    UPROPERTY()
    UTextBlock* HealthText;

    UPROPERTY()
    UProgressBar* HealthBar;

    UPROPERTY()
    UProgressBar* ArmorBar;

    UPROPERTY()
    UTextBlock* SpeedText;

    UPROPERTY()
    UTextBlock* GearText;

    UPROPERTY()
    UTextBlock* AmmoText;

    UPROPERTY()
    UTextBlock* ScoreText;

    UPROPERTY()
    UTextBlock* TimeText;

    UPROPERTY()
    UTextBlock* RedTeamScoreText;

    UPROPERTY()
    UTextBlock* BlueTeamScoreText;

    UPROPERTY()
    UImage* CrosshairImage;

    UPROPERTY()
    UImage* HitMarkerImage;

    UPROPERTY()
    UCanvasPanel* MinimapCanvas;

    UPROPERTY()
    UOverlay* KillFeedOverlay;

    UPROPERTY()
    UOverlay* NotificationOverlay;

    // HUD Data
    UPROPERTY(BlueprintReadOnly, Category = "HUD Data")
    float CurrentHealth = 100.0f;

    UPROPERTY(BlueprintReadOnly, Category = "HUD Data")
    float MaxHealth = 100.0f;

    UPROPERTY(BlueprintReadOnly, Category = "HUD Data")
    float CurrentArmor = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "HUD Data")
    float MaxArmor = 50.0f;

    UPROPERTY(BlueprintReadOnly, Category = "HUD Data")
    float CurrentSpeed = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "HUD Data")
    int32 CurrentGear = 0;

    UPROPERTY(BlueprintReadOnly, Category = "HUD Data")
    int32 CurrentAmmo = 30;

    UPROPERTY(BlueprintReadOnly, Category = "HUD Data")
    int32 MaxAmmo = 30;

    UPROPERTY(BlueprintReadOnly, Category = "HUD Data")
    int32 CurrentScore = 0;

    UPROPERTY(BlueprintReadOnly, Category = "HUD Data")
    float RemainingTime = 600.0f;

    UPROPERTY(BlueprintReadOnly, Category = "HUD Data")
    int32 RedTeamScore = 0;

    UPROPERTY(BlueprintReadOnly, Category = "HUD Data")
    int32 BlueTeamScore = 0;

    // Timers
    FTimerHandle HitMarkerTimer;
    FTimerHandle NotificationTimer;

    // Minimap Data
    UPROPERTY(BlueprintReadOnly, Category = "Minimap")
    TMap<AActor*, UImage*> MinimapIcons;

    // Kill Feed Data
    UPROPERTY(BlueprintReadOnly, Category = "Kill Feed")
    TArray<FString> KillFeedMessages;

    // Notification Data
    UPROPERTY(BlueprintReadOnly, Category = "Notifications")
    TArray<FString> NotificationMessages;
};