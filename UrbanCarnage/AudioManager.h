// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Sound/SoundAttenuation.h"
#include "AudioManager.generated.h"

UENUM(BlueprintType)
enum class EAudioType : uint8
{
    Master      UMETA(DisplayName = "Master"),
    Music       UMETA(DisplayName = "Music"),
    SFX         UMETA(DisplayName = "SFX"),
    Voice       UMETA(DisplayName = "Voice"),
    Ambient     UMETA(DisplayName = "Ambient")
};

USTRUCT(BlueprintType)
struct FAudioSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings")
    float MasterVolume = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings")
    float MusicVolume = 0.8f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings")
    float SFXVolume = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings")
    float VoiceVolume = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings")
    float AmbientVolume = 0.6f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings")
    bool bMuteAll = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings")
    bool bMuteMusic = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings")
    bool bMuteSFX = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings")
    bool bMuteVoice = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings")
    bool bMuteAmbient = false;

    FAudioSettings()
    {
        MasterVolume = 1.0f;
        MusicVolume = 0.8f;
        SFXVolume = 1.0f;
        VoiceVolume = 1.0f;
        AmbientVolume = 0.6f;
        bMuteAll = false;
        bMuteMusic = false;
        bMuteSFX = false;
        bMuteVoice = false;
        bMuteAmbient = false;
    }
};

USTRUCT(BlueprintType)
struct FVehicleAudioData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Audio")
    USoundCue* EngineIdleSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Audio")
    USoundCue* EngineRevSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Audio")
    USoundCue* EngineHighRPMSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Audio")
    USoundCue* TireScreechSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Audio")
    USoundCue* BrakeSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Audio")
    USoundCue* CrashSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Audio")
    USoundCue* HornSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Audio")
    float EnginePitchMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Audio")
    float EngineVolumeMultiplier = 1.0f;

    FVehicleAudioData()
    {
        EngineIdleSound = nullptr;
        EngineRevSound = nullptr;
        EngineHighRPMSound = nullptr;
        TireScreechSound = nullptr;
        BrakeSound = nullptr;
        CrashSound = nullptr;
        HornSound = nullptr;
        EnginePitchMultiplier = 1.0f;
        EngineVolumeMultiplier = 1.0f;
    }
};

UCLASS()
class URBANCARNAGE_API AAudioManager : public AActor
{
    GENERATED_BODY()

public:
    AAudioManager();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
    // Audio Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio Components")
    UAudioComponent* MusicAudioComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio Components")
    UAudioComponent* AmbientAudioComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio Components")
    UAudioComponent* VoiceAudioComponent;

    // Audio Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings")
    FAudioSettings AudioSettings;

    // Music
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
    USoundCue* MainMenuMusic;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
    USoundCue* GameplayMusic;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
    USoundCue* VictoryMusic;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
    USoundCue* DefeatMusic;

    // SFX
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX")
    USoundCue* ButtonClickSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX")
    USoundCue* ButtonHoverSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX")
    USoundCue* WeaponFireSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX")
    USoundCue* WeaponReloadSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX")
    USoundCue* ExplosionSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX")
    USoundCue* HitMarkerSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX")
    USoundCue* KillSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX")
    USoundCue* DeathSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX")
    USoundCue* RespawnSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX")
    USoundCue* PowerUpSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX")
    USoundCue* NotificationSound;

    // Vehicle Audio
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Audio")
    FVehicleAudioData DefaultVehicleAudio;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Audio")
    TMap<FString, FVehicleAudioData> VehicleAudioData;

    // Ambient Sounds
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ambient")
    USoundCue* WindSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ambient")
    USoundCue* RainSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ambient")
    USoundCue* CityAmbientSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ambient")
    USoundCue* DesertAmbientSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ambient")
    USoundCue* ArcticAmbientSound;

    // Audio Functions
    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlayMusic(USoundCue* MusicCue, bool bLoop = true);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void StopMusic();

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PauseMusic();

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void ResumeMusic();

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlaySFX(USoundCue* SFXCue, FVector Location = FVector::ZeroVector, float VolumeMultiplier = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlaySFXAtLocation(USoundCue* SFXCue, FVector Location, float VolumeMultiplier = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlayVoice(USoundCue* VoiceCue, FVector Location = FVector::ZeroVector, float VolumeMultiplier = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlayAmbient(USoundCue* AmbientCue, bool bLoop = true);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void StopAmbient();

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlayVehicleSound(AActor* Vehicle, const FString& SoundType, float PitchMultiplier = 1.0f, float VolumeMultiplier = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void StopVehicleSound(AActor* Vehicle, const FString& SoundType);

    // Volume Control
    UFUNCTION(BlueprintCallable, Category = "Audio Volume")
    void SetMasterVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "Audio Volume")
    void SetMusicVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "Audio Volume")
    void SetSFXVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "Audio Volume")
    void SetVoiceVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "Audio Volume")
    void SetAmbientVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "Audio Volume")
    void SetVolume(EAudioType AudioType, float Volume);

    UFUNCTION(BlueprintCallable, Category = "Audio Volume")
    float GetVolume(EAudioType AudioType) const;

    // Mute Control
    UFUNCTION(BlueprintCallable, Category = "Audio Mute")
    void MuteAll(bool bMute);

    UFUNCTION(BlueprintCallable, Category = "Audio Mute")
    void MuteAudioType(EAudioType AudioType, bool bMute);

    UFUNCTION(BlueprintCallable, Category = "Audio Mute")
    bool IsMuted(EAudioType AudioType) const;

    // Settings
    UFUNCTION(BlueprintCallable, Category = "Audio Settings")
    void LoadAudioSettings();

    UFUNCTION(BlueprintCallable, Category = "Audio Settings")
    void SaveAudioSettings();

    UFUNCTION(BlueprintCallable, Category = "Audio Settings")
    void ApplyAudioSettings();

    UFUNCTION(BlueprintCallable, Category = "Audio Settings")
    void ResetAudioSettings();

    // Vehicle Audio Management
    UFUNCTION(BlueprintCallable, Category = "Vehicle Audio")
    void RegisterVehicle(AActor* Vehicle, const FString& VehicleType);

    UFUNCTION(BlueprintCallable, Category = "Vehicle Audio")
    void UnregisterVehicle(AActor* Vehicle);

    UFUNCTION(BlueprintCallable, Category = "Vehicle Audio")
    void UpdateVehicleAudio(AActor* Vehicle, float Speed, float RPM, bool bIsBraking, bool bIsScreeching);

    // Audio Events
    UFUNCTION(BlueprintCallable, Category = "Audio Events")
    void OnWeaponFired(AActor* Weapon, FVector Location);

    UFUNCTION(BlueprintCallable, Category = "Audio Events")
    void OnPlayerKilled(AActor* Killer, AActor* Victim);

    UFUNCTION(BlueprintCallable, Category = "Audio Events")
    void OnPlayerRespawned(AActor* Player);

    UFUNCTION(BlueprintCallable, Category = "Audio Events")
    void OnExplosion(FVector Location, float Intensity);

    UFUNCTION(BlueprintCallable, Category = "Audio Events")
    void OnPowerUpCollected(AActor* Player, const FString& PowerUpType);

    // Blueprint Events
    UFUNCTION(BlueprintImplementableEvent, Category = "Audio Events")
    void OnMusicStarted(USoundCue* MusicCue);

    UFUNCTION(BlueprintImplementableEvent, Category = "Audio Events")
    void OnMusicStopped();

    UFUNCTION(BlueprintImplementableEvent, Category = "Audio Events")
    void OnSFXPlayed(USoundCue* SFXCue, FVector Location);

    UFUNCTION(BlueprintImplementableEvent, Category = "Audio Events")
    void OnVolumeChanged(EAudioType AudioType, float Volume);

    UFUNCTION(BlueprintImplementableEvent, Category = "Audio Events")
    void OnMuteChanged(EAudioType AudioType, bool bMuted);

protected:
    // Internal Functions
    void InitializeAudioComponents();
    void UpdateAudioVolumes();
    void UpdateVehicleAudioComponents();
    UAudioComponent* GetOrCreateAudioComponent(AActor* Owner, const FString& ComponentName);

    // Vehicle Audio Management
    UPROPERTY()
    TMap<AActor*, TMap<FString, UAudioComponent*>> VehicleAudioComponents;

    UPROPERTY()
    TMap<AActor*, FString> RegisteredVehicles;

    // Audio Component Pool
    UPROPERTY()
    TArray<UAudioComponent*> AudioComponentPool;

    // Settings
    UPROPERTY()
    bool bSettingsLoaded = false;
};