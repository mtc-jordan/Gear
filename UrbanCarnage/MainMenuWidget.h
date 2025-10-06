// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Components/ComboBoxString.h"
#include "Components/Slider.h"
#include "Components/CheckBox.h"
#include "MainMenuWidget.generated.h"

UCLASS()
class URBANCARNAGE_API UMainMenuWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UMainMenuWidget(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeConstruct() override;

public:
    // Main Menu Buttons
    UPROPERTY(meta = (BindWidget))
    UButton* PlayButton;

    UPROPERTY(meta = (BindWidget))
    UButton* MultiplayerButton;

    UPROPERTY(meta = (BindWidget))
    UButton* SettingsButton;

    UPROPERTY(meta = (BindWidget))
    UButton* QuitButton;

    // Multiplayer Menu
    UPROPERTY(meta = (BindWidget))
    UButton* HostGameButton;

    UPROPERTY(meta = (BindWidget))
    UButton* JoinGameButton;

    UPROPERTY(meta = (BindWidget))
    UButton* BackToMainButton;

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* ServerNameTextBox;

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* PlayerNameTextBox;

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* ServerIPTextBox;

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* ServerPortTextBox;

    // Settings Menu
    UPROPERTY(meta = (BindWidget))
    UButton* BackToMainFromSettingsButton;

    UPROPERTY(meta = (BindWidget))
    USlider* MasterVolumeSlider;

    UPROPERTY(meta = (BindWidget))
    USlider* MusicVolumeSlider;

    UPROPERTY(meta = (BindWidget))
    USlider* SFXVolumeSlider;

    UPROPERTY(meta = (BindWidget))
    USlider* VoiceVolumeSlider;

    UPROPERTY(meta = (BindWidget))
    UComboBoxString* GraphicsQualityComboBox;

    UPROPERTY(meta = (BindWidget))
    UComboBoxString* ResolutionComboBox;

    UPROPERTY(meta = (BindWidget))
    UCheckBox* FullscreenCheckBox;

    UPROPERTY(meta = (BindWidget))
    UCheckBox* VSyncCheckBox;

    UPROPERTY(meta = (BindWidget))
    UCheckBox* MotionBlurCheckBox;

    UPROPERTY(meta = (BindWidget))
    UCheckBox* AntiAliasingCheckBox;

    // Game Mode Selection
    UPROPERTY(meta = (BindWidget))
    UComboBoxString* GameModeComboBox;

    UPROPERTY(meta = (BindWidget))
    UComboBoxString* MapComboBox;

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* MaxPlayersTextBox;

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* TimeLimitTextBox;

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* ScoreLimitTextBox;

    // UI Panels
    UPROPERTY(meta = (BindWidget))
    UWidget* MainMenuPanel;

    UPROPERTY(meta = (BindWidget))
    UWidget* MultiplayerPanel;

    UPROPERTY(meta = (BindWidget))
    UWidget* SettingsPanel;

    UPROPERTY(meta = (BindWidget))
    UWidget* GameModePanel;

    // Button Click Handlers
    UFUNCTION()
    void OnPlayButtonClicked();

    UFUNCTION()
    void OnMultiplayerButtonClicked();

    UFUNCTION()
    void OnSettingsButtonClicked();

    UFUNCTION()
    void OnQuitButtonClicked();

    UFUNCTION()
    void OnHostGameButtonClicked();

    UFUNCTION()
    void OnJoinGameButtonClicked();

    UFUNCTION()
    void OnBackToMainButtonClicked();

    UFUNCTION()
    void OnBackToMainFromSettingsButtonClicked();

    // Settings Handlers
    UFUNCTION()
    void OnMasterVolumeChanged(float Value);

    UFUNCTION()
    void OnMusicVolumeChanged(float Value);

    UFUNCTION()
    void OnSFXVolumeChanged(float Value);

    UFUNCTION()
    void OnVoiceVolumeChanged(float Value);

    UFUNCTION()
    void OnGraphicsQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

    UFUNCTION()
    void OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

    UFUNCTION()
    void OnFullscreenChanged(bool bIsChecked);

    UFUNCTION()
    void OnVSyncChanged(bool bIsChecked);

    UFUNCTION()
    void OnMotionBlurChanged(bool bIsChecked);

    UFUNCTION()
    void OnAntiAliasingChanged(bool bIsChecked);

    // Game Mode Handlers
    UFUNCTION()
    void OnGameModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

    UFUNCTION()
    void OnMapChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

    // UI Management
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowMainMenu();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowMultiplayerMenu();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowSettingsMenu();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowGameModeMenu();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void HideAllPanels();

    // Game Functions
    UFUNCTION(BlueprintCallable, Category = "Game")
    void StartSinglePlayerGame();

    UFUNCTION(BlueprintCallable, Category = "Game")
    void HostMultiplayerGame();

    UFUNCTION(BlueprintCallable, Category = "Game")
    void JoinMultiplayerGame();

    UFUNCTION(BlueprintCallable, Category = "Game")
    void QuitGame();

    // Settings Functions
    UFUNCTION(BlueprintCallable, Category = "Settings")
    void LoadSettings();

    UFUNCTION(BlueprintCallable, Category = "Settings")
    void SaveSettings();

    UFUNCTION(BlueprintCallable, Category = "Settings")
    void ApplySettings();

    UFUNCTION(BlueprintCallable, Category = "Settings")
    void ResetSettingsToDefault();

    // Utility Functions
    UFUNCTION(BlueprintCallable, Category = "Utility")
    void PopulateResolutionList();

    UFUNCTION(BlueprintCallable, Category = "Utility")
    void PopulateGameModeList();

    UFUNCTION(BlueprintCallable, Category = "Utility")
    void PopulateMapList();

    // Blueprint Events
    UFUNCTION(BlueprintImplementableEvent, Category = "UI Events")
    void OnMainMenuShown();

    UFUNCTION(BlueprintImplementableEvent, Category = "UI Events")
    void OnMultiplayerMenuShown();

    UFUNCTION(BlueprintImplementableEvent, Category = "UI Events")
    void OnSettingsMenuShown();

    UFUNCTION(BlueprintImplementableEvent, Category = "UI Events")
    void OnGameModeMenuShown();

    UFUNCTION(BlueprintImplementableEvent, Category = "UI Events")
    void OnGameStarting();

    UFUNCTION(BlueprintImplementableEvent, Category = "UI Events")
    void OnConnectionFailed(const FString& ErrorMessage);

    UFUNCTION(BlueprintImplementableEvent, Category = "UI Events")
    void OnConnectionSuccessful();

protected:
    // Current Settings
    UPROPERTY(BlueprintReadOnly, Category = "Settings")
    float MasterVolume = 1.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Settings")
    float MusicVolume = 0.8f;

    UPROPERTY(BlueprintReadOnly, Category = "Settings")
    float SFXVolume = 1.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Settings")
    float VoiceVolume = 1.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Settings")
    int32 GraphicsQuality = 2; // Medium

    UPROPERTY(BlueprintReadOnly, Category = "Settings")
    FString SelectedResolution = "1920x1080";

    UPROPERTY(BlueprintReadOnly, Category = "Settings")
    bool bFullscreen = true;

    UPROPERTY(BlueprintReadOnly, Category = "Settings")
    bool bVSync = true;

    UPROPERTY(BlueprintReadOnly, Category = "Settings")
    bool bMotionBlur = true;

    UPROPERTY(BlueprintReadOnly, Category = "Settings")
    bool bAntiAliasing = true;

    // Game Settings
    UPROPERTY(BlueprintReadOnly, Category = "Game Settings")
    FString SelectedGameMode = "Deathmatch";

    UPROPERTY(BlueprintReadOnly, Category = "Game Settings")
    FString SelectedMap = "UrbanArena";

    UPROPERTY(BlueprintReadOnly, Category = "Game Settings")
    int32 MaxPlayers = 8;

    UPROPERTY(BlueprintReadOnly, Category = "Game Settings")
    int32 TimeLimit = 600; // 10 minutes

    UPROPERTY(BlueprintReadOnly, Category = "Game Settings")
    int32 ScoreLimit = 50;

    // Internal Functions
    void InitializeUI();
    void BindButtonEvents();
    void LoadGameSettings();
    void SaveGameSettings();
    void ApplyGraphicsSettings();
    void ApplyAudioSettings();
};