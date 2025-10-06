// Copyright Epic Games, Inc. All Rights Reserved.

#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Components/ComboBoxString.h"
#include "Components/Slider.h"
#include "Components/CheckBox.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionInterface.h"

UMainMenuWidget::UMainMenuWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    InitializeUI();
    BindButtonEvents();
    LoadSettings();
    PopulateResolutionList();
    PopulateGameModeList();
    PopulateMapList();
}

void UMainMenuWidget::OnPlayButtonClicked()
{
    ShowGameModeMenu();
}

void UMainMenuWidget::OnMultiplayerButtonClicked()
{
    ShowMultiplayerMenu();
}

void UMainMenuWidget::OnSettingsButtonClicked()
{
    ShowSettingsMenu();
}

void UMainMenuWidget::OnQuitButtonClicked()
{
    QuitGame();
}

void UMainMenuWidget::OnHostGameButtonClicked()
{
    HostMultiplayerGame();
}

void UMainMenuWidget::OnJoinGameButtonClicked()
{
    JoinMultiplayerGame();
}

void UMainMenuWidget::OnBackToMainButtonClicked()
{
    ShowMainMenu();
}

void UMainMenuWidget::OnBackToMainFromSettingsButtonClicked()
{
    ShowMainMenu();
}

void UMainMenuWidget::OnMasterVolumeChanged(float Value)
{
    MasterVolume = Value;
    ApplyAudioSettings();
}

void UMainMenuWidget::OnMusicVolumeChanged(float Value)
{
    MusicVolume = Value;
    ApplyAudioSettings();
}

void UMainMenuWidget::OnSFXVolumeChanged(float Value)
{
    SFXVolume = Value;
    ApplyAudioSettings();
}

void UMainMenuWidget::OnVoiceVolumeChanged(float Value)
{
    VoiceVolume = Value;
    ApplyAudioSettings();
}

void UMainMenuWidget::OnGraphicsQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
    if (SelectedItem == "Low")
    {
        GraphicsQuality = 0;
    }
    else if (SelectedItem == "Medium")
    {
        GraphicsQuality = 2;
    }
    else if (SelectedItem == "High")
    {
        GraphicsQuality = 3;
    }
    else if (SelectedItem == "Epic")
    {
        GraphicsQuality = 4;
    }
    
    ApplyGraphicsSettings();
}

void UMainMenuWidget::OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
    SelectedResolution = SelectedItem;
    ApplyGraphicsSettings();
}

void UMainMenuWidget::OnFullscreenChanged(bool bIsChecked)
{
    bFullscreen = bIsChecked;
    ApplyGraphicsSettings();
}

void UMainMenuWidget::OnVSyncChanged(bool bIsChecked)
{
    bVSync = bIsChecked;
    ApplyGraphicsSettings();
}

void UMainMenuWidget::OnMotionBlurChanged(bool bIsChecked)
{
    bMotionBlur = bIsChecked;
    ApplyGraphicsSettings();
}

void UMainMenuWidget::OnAntiAliasingChanged(bool bIsChecked)
{
    bAntiAliasing = bIsChecked;
    ApplyGraphicsSettings();
}

void UMainMenuWidget::OnGameModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
    SelectedGameMode = SelectedItem;
}

void UMainMenuWidget::OnMapChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
    SelectedMap = SelectedItem;
}

void UMainMenuWidget::ShowMainMenu()
{
    HideAllPanels();
    if (MainMenuPanel)
    {
        MainMenuPanel->SetVisibility(ESlateVisibility::Visible);
    }
    OnMainMenuShown();
}

void UMainMenuWidget::ShowMultiplayerMenu()
{
    HideAllPanels();
    if (MultiplayerPanel)
    {
        MultiplayerPanel->SetVisibility(ESlateVisibility::Visible);
    }
    OnMultiplayerMenuShown();
}

void UMainMenuWidget::ShowSettingsMenu()
{
    HideAllPanels();
    if (SettingsPanel)
    {
        SettingsPanel->SetVisibility(ESlateVisibility::Visible);
    }
    OnSettingsMenuShown();
}

void UMainMenuWidget::ShowGameModeMenu()
{
    HideAllPanels();
    if (GameModePanel)
    {
        GameModePanel->SetVisibility(ESlateVisibility::Visible);
    }
    OnGameModeMenuShown();
}

void UMainMenuWidget::HideAllPanels()
{
    if (MainMenuPanel)
    {
        MainMenuPanel->SetVisibility(ESlateVisibility::Hidden);
    }
    if (MultiplayerPanel)
    {
        MultiplayerPanel->SetVisibility(ESlateVisibility::Hidden);
    }
    if (SettingsPanel)
    {
        SettingsPanel->SetVisibility(ESlateVisibility::Hidden);
    }
    if (GameModePanel)
    {
        GameModePanel->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UMainMenuWidget::StartSinglePlayerGame()
{
    OnGameStarting();
    
    // Load the selected map
    FString MapName = "/Game/Maps/" + SelectedMap;
    UGameplayStatics::OpenLevel(GetWorld(), FName(*MapName));
}

void UMainMenuWidget::HostMultiplayerGame()
{
    OnGameStarting();
    
    // Get player name
    FString PlayerName = "Player";
    if (PlayerNameTextBox)
    {
        PlayerName = PlayerNameTextBox->GetText().ToString();
    }
    
    // Get server name
    FString ServerName = "UrbanCarnage Server";
    if (ServerNameTextBox)
    {
        ServerName = ServerNameTextBox->GetText().ToString();
    }
    
    // TODO: Implement actual multiplayer hosting
    // This would involve creating a session and starting the server
    UE_LOG(LogTemp, Warning, TEXT("Hosting multiplayer game: %s"), *ServerName);
    
    // For now, just start single player
    StartSinglePlayerGame();
}

void UMainMenuWidget::JoinMultiplayerGame()
{
    OnGameStarting();
    
    // Get server IP and port
    FString ServerIP = "127.0.0.1";
    FString ServerPort = "7777";
    
    if (ServerIPTextBox)
    {
        ServerIP = ServerIPTextBox->GetText().ToString();
    }
    
    if (ServerPortTextBox)
    {
        ServerPort = ServerPortTextBox->GetText().ToString();
    }
    
    // TODO: Implement actual multiplayer joining
    // This would involve connecting to the specified server
    UE_LOG(LogTemp, Warning, TEXT("Joining multiplayer game: %s:%s"), *ServerIP, *ServerPort);
    
    // For now, just start single player
    StartSinglePlayerGame();
}

void UMainMenuWidget::QuitGame()
{
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        PC->ConsoleCommand("quit");
    }
}

void UMainMenuWidget::LoadSettings()
{
    // Load audio settings
    MasterVolume = UGameplayStatics::GetSoundClassVolume(GetWorld(), nullptr);
    
    // Load graphics settings
    // TODO: Implement proper settings loading from config files
    
    // Apply loaded settings to UI
    if (MasterVolumeSlider)
    {
        MasterVolumeSlider->SetValue(MasterVolume);
    }
    
    if (MusicVolumeSlider)
    {
        MusicVolumeSlider->SetValue(MusicVolume);
    }
    
    if (SFXVolumeSlider)
    {
        SFXVolumeSlider->SetValue(SFXVolume);
    }
    
    if (VoiceVolumeSlider)
    {
        VoiceVolumeSlider->SetValue(VoiceVolume);
    }
    
    if (GraphicsQualityComboBox)
    {
        FString QualityString;
        switch (GraphicsQuality)
        {
            case 0: QualityString = "Low"; break;
            case 2: QualityString = "Medium"; break;
            case 3: QualityString = "High"; break;
            case 4: QualityString = "Epic"; break;
            default: QualityString = "Medium"; break;
        }
        GraphicsQualityComboBox->SetSelectedOption(QualityString);
    }
    
    if (ResolutionComboBox)
    {
        ResolutionComboBox->SetSelectedOption(SelectedResolution);
    }
    
    if (FullscreenCheckBox)
    {
        FullscreenCheckBox->SetIsChecked(bFullscreen);
    }
    
    if (VSyncCheckBox)
    {
        VSyncCheckBox->SetIsChecked(bVSync);
    }
    
    if (MotionBlurCheckBox)
    {
        MotionBlurCheckBox->SetIsChecked(bMotionBlur);
    }
    
    if (AntiAliasingCheckBox)
    {
        AntiAliasingCheckBox->SetIsChecked(bAntiAliasing);
    }
}

void UMainMenuWidget::SaveSettings()
{
    // TODO: Implement proper settings saving to config files
    UE_LOG(LogTemp, Warning, TEXT("Saving settings..."));
}

void UMainMenuWidget::ApplySettings()
{
    ApplyGraphicsSettings();
    ApplyAudioSettings();
    SaveSettings();
}

void UMainMenuWidget::ResetSettingsToDefault()
{
    MasterVolume = 1.0f;
    MusicVolume = 0.8f;
    SFXVolume = 1.0f;
    VoiceVolume = 1.0f;
    GraphicsQuality = 2; // Medium
    SelectedResolution = "1920x1080";
    bFullscreen = true;
    bVSync = true;
    bMotionBlur = true;
    bAntiAliasing = true;
    
    LoadSettings();
}

void UMainMenuWidget::PopulateResolutionList()
{
    if (ResolutionComboBox)
    {
        ResolutionComboBox->ClearOptions();
        ResolutionComboBox->AddOption("1920x1080");
        ResolutionComboBox->AddOption("1680x1050");
        ResolutionComboBox->AddOption("1600x900");
        ResolutionComboBox->AddOption("1440x900");
        ResolutionComboBox->AddOption("1366x768");
        ResolutionComboBox->AddOption("1280x720");
        ResolutionComboBox->AddOption("1024x768");
        ResolutionComboBox->SetSelectedOption(SelectedResolution);
    }
}

void UMainMenuWidget::PopulateGameModeList()
{
    if (GameModeComboBox)
    {
        GameModeComboBox->ClearOptions();
        GameModeComboBox->AddOption("Deathmatch");
        GameModeComboBox->AddOption("Team Battle");
        GameModeComboBox->AddOption("Capture The Flag");
        GameModeComboBox->AddOption("Survival");
        GameModeComboBox->AddOption("Race");
        GameModeComboBox->SetSelectedOption(SelectedGameMode);
    }
}

void UMainMenuWidget::PopulateMapList()
{
    if (MapComboBox)
    {
        MapComboBox->ClearOptions();
        MapComboBox->AddOption("UrbanArena");
        MapComboBox->AddOption("DesertWasteland");
        MapComboBox->AddOption("ArcticBase");
        MapComboBox->AddOption("JungleTemple");
        MapComboBox->AddOption("SpaceStation");
        MapComboBox->SetSelectedOption(SelectedMap);
    }
}

void UMainMenuWidget::InitializeUI()
{
    // Set default values
    if (PlayerNameTextBox)
    {
        PlayerNameTextBox->SetText(FText::FromString("Player"));
    }
    
    if (ServerNameTextBox)
    {
        ServerNameTextBox->SetText(FText::FromString("UrbanCarnage Server"));
    }
    
    if (ServerIPTextBox)
    {
        ServerIPTextBox->SetText(FText::FromString("127.0.0.1"));
    }
    
    if (ServerPortTextBox)
    {
        ServerPortTextBox->SetText(FText::FromString("7777"));
    }
    
    if (MaxPlayersTextBox)
    {
        MaxPlayersTextBox->SetText(FText::FromString("8"));
    }
    
    if (TimeLimitTextBox)
    {
        TimeLimitTextBox->SetText(FText::FromString("600"));
    }
    
    if (ScoreLimitTextBox)
    {
        ScoreLimitTextBox->SetText(FText::FromString("50"));
    }
}

void UMainMenuWidget::BindButtonEvents()
{
    if (PlayButton)
    {
        PlayButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnPlayButtonClicked);
    }
    
    if (MultiplayerButton)
    {
        MultiplayerButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnMultiplayerButtonClicked);
    }
    
    if (SettingsButton)
    {
        SettingsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnSettingsButtonClicked);
    }
    
    if (QuitButton)
    {
        QuitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitButtonClicked);
    }
    
    if (HostGameButton)
    {
        HostGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnHostGameButtonClicked);
    }
    
    if (JoinGameButton)
    {
        JoinGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnJoinGameButtonClicked);
    }
    
    if (BackToMainButton)
    {
        BackToMainButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnBackToMainButtonClicked);
    }
    
    if (BackToMainFromSettingsButton)
    {
        BackToMainFromSettingsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnBackToMainFromSettingsButtonClicked);
    }
    
    // Bind slider events
    if (MasterVolumeSlider)
    {
        MasterVolumeSlider->OnValueChanged.AddDynamic(this, &UMainMenuWidget::OnMasterVolumeChanged);
    }
    
    if (MusicVolumeSlider)
    {
        MusicVolumeSlider->OnValueChanged.AddDynamic(this, &UMainMenuWidget::OnMusicVolumeChanged);
    }
    
    if (SFXVolumeSlider)
    {
        SFXVolumeSlider->OnValueChanged.AddDynamic(this, &UMainMenuWidget::OnSFXVolumeChanged);
    }
    
    if (VoiceVolumeSlider)
    {
        VoiceVolumeSlider->OnValueChanged.AddDynamic(this, &UMainMenuWidget::OnVoiceVolumeChanged);
    }
    
    // Bind combo box events
    if (GraphicsQualityComboBox)
    {
        GraphicsQualityComboBox->OnSelectionChanged.AddDynamic(this, &UMainMenuWidget::OnGraphicsQualityChanged);
    }
    
    if (ResolutionComboBox)
    {
        ResolutionComboBox->OnSelectionChanged.AddDynamic(this, &UMainMenuWidget::OnResolutionChanged);
    }
    
    if (GameModeComboBox)
    {
        GameModeComboBox->OnSelectionChanged.AddDynamic(this, &UMainMenuWidget::OnGameModeChanged);
    }
    
    if (MapComboBox)
    {
        MapComboBox->OnSelectionChanged.AddDynamic(this, &UMainMenuWidget::OnMapChanged);
    }
    
    // Bind checkbox events
    if (FullscreenCheckBox)
    {
        FullscreenCheckBox->OnCheckStateChanged.AddDynamic(this, &UMainMenuWidget::OnFullscreenChanged);
    }
    
    if (VSyncCheckBox)
    {
        VSyncCheckBox->OnCheckStateChanged.AddDynamic(this, &UMainMenuWidget::OnVSyncChanged);
    }
    
    if (MotionBlurCheckBox)
    {
        MotionBlurCheckBox->OnCheckStateChanged.AddDynamic(this, &UMainMenuWidget::OnMotionBlurChanged);
    }
    
    if (AntiAliasingCheckBox)
    {
        AntiAliasingCheckBox->OnCheckStateChanged.AddDynamic(this, &UMainMenuWidget::OnAntiAliasingChanged);
    }
}

void UMainMenuWidget::LoadGameSettings()
{
    // Load game-specific settings
    // TODO: Implement game settings loading
}

void UMainMenuWidget::SaveGameSettings()
{
    // Save game-specific settings
    // TODO: Implement game settings saving
}

void UMainMenuWidget::ApplyGraphicsSettings()
{
    // Apply graphics settings
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        // Set resolution
        FString ResolutionCommand = FString::Printf(TEXT("r.SetRes %s"), *SelectedResolution);
        PC->ConsoleCommand(ResolutionCommand);
        
        // Set fullscreen
        FString FullscreenCommand = bFullscreen ? TEXT("r.FullScreenMode 1") : TEXT("r.FullScreenMode 0");
        PC->ConsoleCommand(FullscreenCommand);
        
        // Set VSync
        FString VSyncCommand = bVSync ? TEXT("r.VSync 1") : TEXT("r.VSync 0");
        PC->ConsoleCommand(VSyncCommand);
        
        // Set graphics quality
        FString QualityCommand = FString::Printf(TEXT("sg.ViewDistanceQuality %d"), GraphicsQuality);
        PC->ConsoleCommand(QualityCommand);
        
        QualityCommand = FString::Printf(TEXT("sg.AntiAliasingQuality %d"), GraphicsQuality);
        PC->ConsoleCommand(QualityCommand);
        
        QualityCommand = FString::Printf(TEXT("sg.ShadowQuality %d"), GraphicsQuality);
        PC->ConsoleCommand(QualityCommand);
        
        QualityCommand = FString::Printf(TEXT("sg.PostProcessQuality %d"), GraphicsQuality);
        PC->ConsoleCommand(QualityCommand);
        
        QualityCommand = FString::Printf(TEXT("sg.TextureQuality %d"), GraphicsQuality);
        PC->ConsoleCommand(QualityCommand);
        
        QualityCommand = FString::Printf(TEXT("sg.EffectsQuality %d"), GraphicsQuality);
        PC->ConsoleCommand(QualityCommand);
        
        QualityCommand = FString::Printf(TEXT("sg.FoliageQuality %d"), GraphicsQuality);
        PC->ConsoleCommand(QualityCommand);
        
        QualityCommand = FString::Printf(TEXT("sg.ShadingQuality %d"), GraphicsQuality);
        PC->ConsoleCommand(QualityCommand);
    }
}

void UMainMenuWidget::ApplyAudioSettings()
{
    // Apply audio settings
    UGameplayStatics::SetSoundClassVolume(GetWorld(), nullptr, MasterVolume);
    
    // TODO: Apply individual volume settings for music, SFX, and voice
}