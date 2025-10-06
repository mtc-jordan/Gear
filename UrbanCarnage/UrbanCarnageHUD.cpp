// Copyright Epic Games, Inc. All Rights Reserved.

#include "UrbanCarnageHUD.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/Overlay.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "UrbanCarnageGameState.h"
#include "UrbanCarnagePawn.h"

AUrbanCarnageHUD::AUrbanCarnageHUD()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AUrbanCarnageHUD::BeginPlay()
{
    Super::BeginPlay();
    
    CreateHUDWidgets();
    ShowMainHUD();
}

void AUrbanCarnageHUD::DrawHUD()
{
    Super::DrawHUD();
    
    if (bIsPaused)
    {
        return;
    }
    
    DrawCrosshair();
    DrawMinimap();
    DrawHealthBar();
    DrawSpeedometer();
    DrawAmmoCounter();
    DrawScoreDisplay();
    DrawKillFeed();
    DrawNotifications();
}

void AUrbanCarnageHUD::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    UpdateHUD();
}

void AUrbanCarnageHUD::ShowMainHUD()
{
    if (MainHUDWidget)
    {
        MainHUDWidget->AddToViewport();
        OnMainHUDShown();
    }
}

void AUrbanCarnageHUD::HideMainHUD()
{
    if (MainHUDWidget)
    {
        MainHUDWidget->RemoveFromViewport();
        OnMainHUDHidden();
    }
}

void AUrbanCarnageHUD::TogglePauseMenu()
{
    if (bIsPaused)
    {
        HidePauseMenu();
    }
    else
    {
        ShowPauseMenu();
    }
}

void AUrbanCarnageHUD::ShowPauseMenu()
{
    if (PauseMenuWidget && !bIsPaused)
    {
        bIsPaused = true;
        PauseMenuWidget->AddToViewport();
        
        // Pause the game
        if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
        {
            PC->SetPause(true);
        }
        
        OnPauseMenuShown();
    }
}

void AUrbanCarnageHUD::HidePauseMenu()
{
    if (PauseMenuWidget && bIsPaused)
    {
        bIsPaused = false;
        PauseMenuWidget->RemoveFromViewport();
        
        // Unpause the game
        if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
        {
            PC->SetPause(false);
        }
        
        OnPauseMenuHidden();
    }
}

void AUrbanCarnageHUD::ToggleScoreboard()
{
    if (bShowScoreboard)
    {
        HideScoreboard();
    }
    else
    {
        ShowScoreboard();
    }
}

void AUrbanCarnageHUD::ShowScoreboard()
{
    if (ScoreboardWidget && !bShowScoreboard)
    {
        bShowScoreboard = true;
        ScoreboardWidget->AddToViewport();
        UpdatePlayerList();
        OnScoreboardShown();
    }
}

void AUrbanCarnageHUD::HideScoreboard()
{
    if (ScoreboardWidget && bShowScoreboard)
    {
        bShowScoreboard = false;
        ScoreboardWidget->RemoveFromViewport();
        OnScoreboardHidden();
    }
}

void AUrbanCarnageHUD::ToggleMinimap()
{
    if (bShowMinimap)
    {
        HideMinimap();
    }
    else
    {
        ShowMinimap();
    }
}

void AUrbanCarnageHUD::ShowMinimap()
{
    if (MinimapWidget && !bShowMinimap)
    {
        bShowMinimap = true;
        MinimapWidget->AddToViewport();
        UpdateMinimap();
        OnMinimapShown();
    }
}

void AUrbanCarnageHUD::HideMinimap()
{
    if (MinimapWidget && bShowMinimap)
    {
        bShowMinimap = false;
        MinimapWidget->RemoveFromViewport();
        OnMinimapHidden();
    }
}

void AUrbanCarnageHUD::ToggleChat()
{
    if (bShowChat)
    {
        HideChat();
    }
    else
    {
        ShowChat();
    }
}

void AUrbanCarnageHUD::ShowChat()
{
    if (ChatWidget && !bShowChat)
    {
        bShowChat = true;
        ChatWidget->AddToViewport();
        OnChatShown();
    }
}

void AUrbanCarnageHUD::HideChat()
{
    if (ChatWidget && bShowChat)
    {
        bShowChat = false;
        ChatWidget->RemoveFromViewport();
        OnChatHidden();
    }
}

void AUrbanCarnageHUD::ShowGameOver()
{
    if (GameOverWidget)
    {
        GameOverWidget->AddToViewport();
        OnGameOverShown();
    }
}

void AUrbanCarnageHUD::HideGameOver()
{
    if (GameOverWidget)
    {
        GameOverWidget->RemoveFromViewport();
        OnGameOverHidden();
    }
}

void AUrbanCarnageHUD::UpdateHealthBar(float CurrentHealth, float MaxHealth)
{
    this->CurrentHealth = CurrentHealth;
    this->MaxHealth = MaxHealth;
    
    if (HealthBar)
    {
        HealthBar->SetPercent(CurrentHealth / MaxHealth);
    }
    
    if (HealthText)
    {
        HealthText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), CurrentHealth, MaxHealth)));
    }
    
    OnHealthUpdated(CurrentHealth, MaxHealth);
}

void AUrbanCarnageHUD::UpdateArmorBar(float CurrentArmor, float MaxArmor)
{
    this->CurrentArmor = CurrentArmor;
    this->MaxArmor = MaxArmor;
    
    if (ArmorBar)
    {
        ArmorBar->SetPercent(CurrentArmor / MaxArmor);
    }
}

void AUrbanCarnageHUD::UpdateSpeedDisplay(float Speed)
{
    CurrentSpeed = Speed;
    
    if (SpeedText)
    {
        SpeedText->SetText(FText::FromString(FString::Printf(TEXT("%.0f km/h"), Speed)));
    }
    
    OnSpeedUpdated(Speed);
}

void AUrbanCarnageHUD::UpdateGearDisplay(int32 Gear)
{
    CurrentGear = Gear;
    
    if (GearText)
    {
        GearText->SetText(FText::FromString(FString::Printf(TEXT("Gear: %d"), Gear)));
    }
}

void AUrbanCarnageHUD::UpdateAmmoDisplay(int32 CurrentAmmo, int32 MaxAmmo)
{
    this->CurrentAmmo = CurrentAmmo;
    this->MaxAmmo = MaxAmmo;
    
    if (AmmoText)
    {
        AmmoText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentAmmo, MaxAmmo)));
    }
    
    OnAmmoUpdated(CurrentAmmo, MaxAmmo);
}

void AUrbanCarnageHUD::UpdateScoreDisplay(int32 Score)
{
    CurrentScore = Score;
    
    if (ScoreText)
    {
        ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), Score)));
    }
    
    OnScoreUpdated(Score);
}

void AUrbanCarnageHUD::UpdateKillCount(int32 Kills)
{
    // Update kill count display
    OnKillCountUpdated(Kills);
}

void AUrbanCarnageHUD::UpdateDeathCount(int32 Deaths)
{
    // Update death count display
    OnDeathCountUpdated(Deaths);
}

void AUrbanCarnageHUD::UpdateTimeDisplay(float RemainingTime)
{
    this->RemainingTime = RemainingTime;
    
    if (TimeText)
    {
        int32 Minutes = FMath::FloorToInt(RemainingTime / 60.0f);
        int32 Seconds = FMath::FloorToInt(RemainingTime) % 60;
        TimeText->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds)));
    }
}

void AUrbanCarnageHUD::UpdateTeamScore(int32 RedScore, int32 BlueScore)
{
    RedTeamScore = RedScore;
    BlueTeamScore = BlueScore;
    
    if (RedTeamScoreText)
    {
        RedTeamScoreText->SetText(FText::FromString(FString::Printf(TEXT("Red: %d"), RedScore)));
    }
    
    if (BlueTeamScoreText)
    {
        BlueTeamScoreText->SetText(FText::FromString(FString::Printf(TEXT("Blue: %d"), BlueScore)));
    }
}

void AUrbanCarnageHUD::UpdatePlayerList()
{
    // Update player list in scoreboard
    OnPlayerListUpdated();
}

void AUrbanCarnageHUD::UpdateMinimap()
{
    // Update minimap with current player and enemy positions
    OnMinimapUpdated();
}

void AUrbanCarnageHUD::AddChatMessage(const FString& PlayerName, const FString& Message, FLinearColor Color)
{
    FString ChatMessage = FString::Printf(TEXT("<%s> %s"), *PlayerName, *Message);
    OnChatMessageAdded(ChatMessage, Color);
}

void AUrbanCarnageHUD::ShowKillFeed(const FString& KillerName, const FString& VictimName, const FString& WeaponName)
{
    FString KillMessage = FString::Printf(TEXT("%s killed %s with %s"), *KillerName, *VictimName, *WeaponName);
    KillFeedMessages.Add(KillMessage);
    
    // Keep only last 5 messages
    if (KillFeedMessages.Num() > 5)
    {
        KillFeedMessages.RemoveAt(0);
    }
    
    OnKillFeedUpdated(KillerName, VictimName, WeaponName);
}

void AUrbanCarnageHUD::ShowNotification(const FString& Message, FLinearColor Color, float Duration)
{
    NotificationMessages.Add(Message);
    OnNotificationShown(Message, Color, Duration);
    
    // Remove notification after duration
    GetWorld()->GetTimerManager().SetTimer(NotificationTimer, [this]()
    {
        if (NotificationMessages.Num() > 0)
        {
            NotificationMessages.RemoveAt(0);
        }
    }, Duration, false);
}

void AUrbanCarnageHUD::UpdateCrosshair(bool bIsAiming, bool bCanFire)
{
    OnCrosshairUpdated(bIsAiming, bCanFire);
}

void AUrbanCarnageHUD::ShowHitMarker()
{
    OnHitMarkerShown();
    
    // Hide hit marker after short duration
    GetWorld()->GetTimerManager().SetTimer(HitMarkerTimer, [this]()
    {
        OnHitMarkerHidden();
    }, 0.2f, false);
}

void AUrbanCarnageHUD::ShowDamageIndicator(FVector DamageDirection, float DamageAmount)
{
    OnDamageIndicatorShown(DamageDirection, DamageAmount);
}

void AUrbanCarnageHUD::AddMinimapIcon(AActor* Actor, UTexture2D* Icon, FLinearColor Color)
{
    if (Actor && Icon)
    {
        // Create minimap icon
        UImage* MinimapIcon = NewObject<UImage>();
        MinimapIcon->SetBrushFromTexture(Icon);
        MinimapIcon->SetColorAndOpacity(Color);
        
        MinimapIcons.Add(Actor, MinimapIcon);
        
        if (MinimapCanvas)
        {
            MinimapCanvas->AddChild(MinimapIcon);
        }
    }
}

void AUrbanCarnageHUD::RemoveMinimapIcon(AActor* Actor)
{
    if (UImage** Icon = MinimapIcons.Find(Actor))
    {
        if (*Icon)
        {
            (*Icon)->RemoveFromParent();
        }
        MinimapIcons.Remove(Actor);
    }
}

void AUrbanCarnageHUD::UpdateMinimapIcon(AActor* Actor, FVector NewLocation)
{
    if (UImage** Icon = MinimapIcons.Find(Actor))
    {
        if (*Icon)
        {
            // Update icon position on minimap
            // This would involve converting world coordinates to minimap coordinates
        }
    }
}

void AUrbanCarnageHUD::SetupInputComponent()
{
    Super::SetupInputComponent();
    
    if (InputComponent)
    {
        InputComponent->BindAction("Pause", IE_Pressed, this, &AUrbanCarnageHUD::OnPausePressed);
        InputComponent->BindAction("Scoreboard", IE_Pressed, this, &AUrbanCarnageHUD::OnScoreboardPressed);
        InputComponent->BindAction("Scoreboard", IE_Released, this, &AUrbanCarnageHUD::OnScoreboardReleased);
        InputComponent->BindAction("Chat", IE_Pressed, this, &AUrbanCarnageHUD::OnChatPressed);
        InputComponent->BindAction("Minimap", IE_Pressed, this, &AUrbanCarnageHUD::OnMinimapPressed);
    }
}

void AUrbanCarnageHUD::OnPausePressed()
{
    TogglePauseMenu();
}

void AUrbanCarnageHUD::OnScoreboardPressed()
{
    ShowScoreboard();
}

void AUrbanCarnageHUD::OnScoreboardReleased()
{
    HideScoreboard();
}

void AUrbanCarnageHUD::OnChatPressed()
{
    ToggleChat();
}

void AUrbanCarnageHUD::OnMinimapPressed()
{
    ToggleMinimap();
}

void AUrbanCarnageHUD::CreateHUDWidgets()
{
    if (MainHUDWidgetClass)
    {
        MainHUDWidget = CreateWidget<UUserWidget>(GetWorld(), MainHUDWidgetClass);
    }
    
    if (PauseMenuWidgetClass)
    {
        PauseMenuWidget = CreateWidget<UUserWidget>(GetWorld(), PauseMenuWidgetClass);
    }
    
    if (GameOverWidgetClass)
    {
        GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
    }
    
    if (ScoreboardWidgetClass)
    {
        ScoreboardWidget = CreateWidget<UUserWidget>(GetWorld(), ScoreboardWidgetClass);
    }
    
    if (MinimapWidgetClass)
    {
        MinimapWidget = CreateWidget<UUserWidget>(GetWorld(), MinimapWidgetClass);
    }
    
    if (ChatWidgetClass)
    {
        ChatWidget = CreateWidget<UUserWidget>(GetWorld(), ChatWidgetClass);
    }
}

void AUrbanCarnageHUD::UpdateHUD()
{
    // Update HUD elements based on current game state
    if (AUrbanCarnageGameState* GameState = GetWorld()->GetGameState<AUrbanCarnageGameState>())
    {
        UpdateTimeDisplay(GameState->RemainingTime);
        UpdateTeamScore(GameState->RedTeamScore, GameState->BlueTeamScore);
    }
    
    // Update player-specific HUD elements
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        if (AUrbanCarnagePawn* Pawn = Cast<AUrbanCarnagePawn>(PC->GetPawn()))
        {
            // Update health
            if (UHealthComponent* HealthComp = Pawn->GetComponentByClass<UHealthComponent>())
            {
                UpdateHealthBar(HealthComp->GetCurrentHealth(), HealthComp->GetMaxHealth());
            }
            
            // Update speed
            if (UChaosWheeledVehicleMovementComponent* VehicleMovement = Pawn->GetChaosVehicleMovement())
            {
                float Speed = VehicleMovement->GetForwardSpeed() * 0.036f; // Convert to km/h
                UpdateSpeedDisplay(Speed);
            }
            
            // Update ammo
            if (Pawn->PrimaryWeapon_Ref)
            {
                // Update ammo display based on weapon
                UpdateAmmoDisplay(30, 30); // Placeholder
            }
        }
    }
}

void AUrbanCarnageHUD::DrawCrosshair()
{
    if (Canvas)
    {
        // Draw crosshair in center of screen
        FVector2D Center = FVector2D(Canvas->SizeX * 0.5f, Canvas->SizeY * 0.5f);
        
        // Draw crosshair lines
        FLinearColor CrosshairColor = FLinearColor::White;
        float CrosshairSize = 20.0f;
        
        // Horizontal line
        Canvas->DrawLine(
            Center.X - CrosshairSize, Center.Y,
            Center.X + CrosshairSize, Center.Y,
            CrosshairColor
        );
        
        // Vertical line
        Canvas->DrawLine(
            Center.X, Center.Y - CrosshairSize,
            Center.X, Center.Y + CrosshairSize,
            CrosshairColor
        );
    }
}

void AUrbanCarnageHUD::DrawMinimap()
{
    if (Canvas && bShowMinimap)
    {
        // Draw minimap in top-right corner
        float MinimapSize = 200.0f;
        float MinimapX = Canvas->SizeX - MinimapSize - 20.0f;
        float MinimapY = 20.0f;
        
        // Draw minimap background
        Canvas->DrawRect(
            MinimapX, MinimapY,
            MinimapSize, MinimapSize,
            FLinearColor(0.0f, 0.0f, 0.0f, 0.5f)
        );
        
        // Draw minimap border
        Canvas->DrawLine(
            MinimapX, MinimapY,
            MinimapX + MinimapSize, MinimapY,
            FLinearColor::White
        );
        Canvas->DrawLine(
            MinimapX + MinimapSize, MinimapY,
            MinimapX + MinimapSize, MinimapY + MinimapSize,
            FLinearColor::White
        );
        Canvas->DrawLine(
            MinimapX + MinimapSize, MinimapY + MinimapSize,
            MinimapX, MinimapY + MinimapSize,
            FLinearColor::White
        );
        Canvas->DrawLine(
            MinimapX, MinimapY + MinimapSize,
            MinimapX, MinimapY,
            FLinearColor::White
        );
    }
}

void AUrbanCarnageHUD::DrawHealthBar()
{
    if (Canvas)
    {
        // Draw health bar in bottom-left corner
        float HealthBarWidth = 200.0f;
        float HealthBarHeight = 20.0f;
        float HealthBarX = 20.0f;
        float HealthBarY = Canvas->SizeY - HealthBarHeight - 20.0f;
        
        // Draw health bar background
        Canvas->DrawRect(
            HealthBarX, HealthBarY,
            HealthBarWidth, HealthBarHeight,
            FLinearColor::Red
        );
        
        // Draw health bar fill
        float HealthPercentage = CurrentHealth / MaxHealth;
        Canvas->DrawRect(
            HealthBarX, HealthBarY,
            HealthBarWidth * HealthPercentage, HealthBarHeight,
            FLinearColor::Green
        );
    }
}

void AUrbanCarnageHUD::DrawSpeedometer()
{
    if (Canvas)
    {
        // Draw speedometer in bottom-right corner
        FString SpeedText = FString::Printf(TEXT("%.0f km/h"), CurrentSpeed);
        FVector2D TextSize;
        Canvas->TextSize(GEngine->GetSmallFont(), SpeedText, TextSize.X, TextSize.Y);
        
        float SpeedX = Canvas->SizeX - TextSize.X - 20.0f;
        float SpeedY = Canvas->SizeY - TextSize.Y - 20.0f;
        
        Canvas->DrawText(
            SpeedText,
            SpeedX, SpeedY,
            GEngine->GetSmallFont(),
            FLinearColor::White
        );
    }
}

void AUrbanCarnageHUD::DrawAmmoCounter()
{
    if (Canvas)
    {
        // Draw ammo counter in bottom-right corner
        FString AmmoText = FString::Printf(TEXT("%d / %d"), CurrentAmmo, MaxAmmo);
        FVector2D TextSize;
        Canvas->TextSize(GEngine->GetSmallFont(), AmmoText, TextSize.X, TextSize.Y);
        
        float AmmoX = Canvas->SizeX - TextSize.X - 20.0f;
        float AmmoY = Canvas->SizeY - TextSize.Y - 50.0f;
        
        Canvas->DrawText(
            AmmoText,
            AmmoX, AmmoY,
            GEngine->GetSmallFont(),
            FLinearColor::White
        );
    }
}

void AUrbanCarnageHUD::DrawScoreDisplay()
{
    if (Canvas)
    {
        // Draw score in top-left corner
        FString ScoreText = FString::Printf(TEXT("Score: %d"), CurrentScore);
        FVector2D TextSize;
        Canvas->TextSize(GEngine->GetSmallFont(), ScoreText, TextSize.X, TextSize.Y);
        
        float ScoreX = 20.0f;
        float ScoreY = 20.0f;
        
        Canvas->DrawText(
            ScoreText,
            ScoreX, ScoreY,
            GEngine->GetSmallFont(),
            FLinearColor::White
        );
    }
}

void AUrbanCarnageHUD::DrawKillFeed()
{
    if (Canvas && KillFeedMessages.Num() > 0)
    {
        // Draw kill feed in top-right corner
        float KillFeedX = Canvas->SizeX - 300.0f;
        float KillFeedY = 20.0f;
        float LineHeight = 20.0f;
        
        for (int32 i = 0; i < KillFeedMessages.Num(); i++)
        {
            FString KillMessage = KillFeedMessages[i];
            FVector2D TextSize;
            Canvas->TextSize(GEngine->GetSmallFont(), KillMessage, TextSize.X, TextSize.Y);
            
            Canvas->DrawText(
                KillMessage,
                KillFeedX, KillFeedY + (i * LineHeight),
                GEngine->GetSmallFont(),
                FLinearColor::White
            );
        }
    }
}

void AUrbanCarnageHUD::DrawNotifications()
{
    if (Canvas && NotificationMessages.Num() > 0)
    {
        // Draw notifications in center of screen
        float NotificationX = Canvas->SizeX * 0.5f - 150.0f;
        float NotificationY = Canvas->SizeY * 0.3f;
        float LineHeight = 30.0f;
        
        for (int32 i = 0; i < NotificationMessages.Num(); i++)
        {
            FString NotificationMessage = NotificationMessages[i];
            FVector2D TextSize;
            Canvas->TextSize(GEngine->GetMediumFont(), NotificationMessage, TextSize.X, TextSize.Y);
            
            Canvas->DrawText(
                NotificationMessage,
                NotificationX, NotificationY + (i * LineHeight),
                GEngine->GetMediumFont(),
                FLinearColor::Yellow
            );
        }
    }
}