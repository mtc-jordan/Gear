// Copyright Epic Games, Inc. All Rights Reserved.

#include "AudioManager.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Sound/SoundAttenuation.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

AAudioManager::AAudioManager()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = false;
}

void AAudioManager::BeginPlay()
{
    Super::BeginPlay();
    
    InitializeAudioComponents();
    LoadAudioSettings();
    ApplyAudioSettings();
}

void AAudioManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    UpdateAudioVolumes();
    UpdateVehicleAudioComponents();
}

void AAudioManager::PlayMusic(USoundCue* MusicCue, bool bLoop)
{
    if (!MusicCue || !MusicAudioComponent)
    {
        return;
    }
    
    MusicAudioComponent->SetSound(MusicCue);
    MusicAudioComponent->bLooping = bLoop;
    MusicAudioComponent->Play();
    
    OnMusicStarted(MusicCue);
}

void AAudioManager::StopMusic()
{
    if (MusicAudioComponent)
    {
        MusicAudioComponent->Stop();
        OnMusicStopped();
    }
}

void AAudioManager::PauseMusic()
{
    if (MusicAudioComponent)
    {
        MusicAudioComponent->SetPaused(true);
    }
}

void AAudioManager::ResumeMusic()
{
    if (MusicAudioComponent)
    {
        MusicAudioComponent->SetPaused(false);
    }
}

void AAudioManager::PlaySFX(USoundCue* SFXCue, FVector Location, float VolumeMultiplier)
{
    if (!SFXCue)
    {
        return;
    }
    
    if (Location == FVector::ZeroVector)
    {
        // Play at listener location
        UGameplayStatics::PlaySound2D(GetWorld(), SFXCue, AudioSettings.SFXVolume * VolumeMultiplier);
    }
    else
    {
        // Play at specific location
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), SFXCue, Location, AudioSettings.SFXVolume * VolumeMultiplier);
    }
    
    OnSFXPlayed(SFXCue, Location);
}

void AAudioManager::PlaySFXAtLocation(USoundCue* SFXCue, FVector Location, float VolumeMultiplier)
{
    PlaySFX(SFXCue, Location, VolumeMultiplier);
}

void AAudioManager::PlayVoice(USoundCue* VoiceCue, FVector Location, float VolumeMultiplier)
{
    if (!VoiceCue)
    {
        return;
    }
    
    if (Location == FVector::ZeroVector)
    {
        UGameplayStatics::PlaySound2D(GetWorld(), VoiceCue, AudioSettings.VoiceVolume * VolumeMultiplier);
    }
    else
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), VoiceCue, Location, AudioSettings.VoiceVolume * VolumeMultiplier);
    }
}

void AAudioManager::PlayAmbient(USoundCue* AmbientCue, bool bLoop)
{
    if (!AmbientCue || !AmbientAudioComponent)
    {
        return;
    }
    
    AmbientAudioComponent->SetSound(AmbientCue);
    AmbientAudioComponent->bLooping = bLoop;
    AmbientAudioComponent->Play();
}

void AAudioManager::StopAmbient()
{
    if (AmbientAudioComponent)
    {
        AmbientAudioComponent->Stop();
    }
}

void AAudioManager::PlayVehicleSound(AActor* Vehicle, const FString& SoundType, float PitchMultiplier, float VolumeMultiplier)
{
    if (!Vehicle)
    {
        return;
    }
    
    FString* VehicleType = RegisteredVehicles.Find(Vehicle);
    if (!VehicleType)
    {
        return;
    }
    
    FVehicleAudioData* AudioData = VehicleAudioData.Find(*VehicleType);
    if (!AudioData)
    {
        AudioData = &DefaultVehicleAudio;
    }
    
    USoundCue* SoundCue = nullptr;
    
    if (SoundType == "EngineIdle")
    {
        SoundCue = AudioData->EngineIdleSound;
    }
    else if (SoundType == "EngineRev")
    {
        SoundCue = AudioData->EngineRevSound;
    }
    else if (SoundType == "EngineHighRPM")
    {
        SoundCue = AudioData->EngineHighRPMSound;
    }
    else if (SoundType == "TireScreech")
    {
        SoundCue = AudioData->TireScreechSound;
    }
    else if (SoundType == "Brake")
    {
        SoundCue = AudioData->BrakeSound;
    }
    else if (SoundType == "Crash")
    {
        SoundCue = AudioData->CrashSound;
    }
    else if (SoundType == "Horn")
    {
        SoundCue = AudioData->HornSound;
    }
    
    if (SoundCue)
    {
        UAudioComponent* AudioComp = GetOrCreateAudioComponent(Vehicle, SoundType);
        if (AudioComp)
        {
            AudioComp->SetSound(SoundCue);
            AudioComp->SetPitchMultiplier(PitchMultiplier * AudioData->EnginePitchMultiplier);
            AudioComp->SetVolumeMultiplier(VolumeMultiplier * AudioData->EngineVolumeMultiplier * AudioSettings.SFXVolume);
            AudioComp->Play();
        }
    }
}

void AAudioManager::StopVehicleSound(AActor* Vehicle, const FString& SoundType)
{
    if (!Vehicle)
    {
        return;
    }
    
    TMap<FString, UAudioComponent*>* VehicleAudioMap = VehicleAudioComponents.Find(Vehicle);
    if (VehicleAudioMap)
    {
        if (UAudioComponent** AudioComp = VehicleAudioMap->Find(SoundType))
        {
            if (*AudioComp)
            {
                (*AudioComp)->Stop();
            }
        }
    }
}

void AAudioManager::SetMasterVolume(float Volume)
{
    AudioSettings.MasterVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
    ApplyAudioSettings();
    OnVolumeChanged(EAudioType::Master, AudioSettings.MasterVolume);
}

void AAudioManager::SetMusicVolume(float Volume)
{
    AudioSettings.MusicVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
    ApplyAudioSettings();
    OnVolumeChanged(EAudioType::Music, AudioSettings.MusicVolume);
}

void AAudioManager::SetSFXVolume(float Volume)
{
    AudioSettings.SFXVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
    ApplyAudioSettings();
    OnVolumeChanged(EAudioType::SFX, AudioSettings.SFXVolume);
}

void AAudioManager::SetVoiceVolume(float Volume)
{
    AudioSettings.VoiceVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
    ApplyAudioSettings();
    OnVolumeChanged(EAudioType::Voice, AudioSettings.VoiceVolume);
}

void AAudioManager::SetAmbientVolume(float Volume)
{
    AudioSettings.AmbientVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
    ApplyAudioSettings();
    OnVolumeChanged(EAudioType::Ambient, AudioSettings.AmbientVolume);
}

void AAudioManager::SetVolume(EAudioType AudioType, float Volume)
{
    switch (AudioType)
    {
        case EAudioType::Master:
            SetMasterVolume(Volume);
            break;
        case EAudioType::Music:
            SetMusicVolume(Volume);
            break;
        case EAudioType::SFX:
            SetSFXVolume(Volume);
            break;
        case EAudioType::Voice:
            SetVoiceVolume(Volume);
            break;
        case EAudioType::Ambient:
            SetAmbientVolume(Volume);
            break;
    }
}

float AAudioManager::GetVolume(EAudioType AudioType) const
{
    switch (AudioType)
    {
        case EAudioType::Master:
            return AudioSettings.MasterVolume;
        case EAudioType::Music:
            return AudioSettings.MusicVolume;
        case EAudioType::SFX:
            return AudioSettings.SFXVolume;
        case EAudioType::Voice:
            return AudioSettings.VoiceVolume;
        case EAudioType::Ambient:
            return AudioSettings.AmbientVolume;
        default:
            return 1.0f;
    }
}

void AAudioManager::MuteAll(bool bMute)
{
    AudioSettings.bMuteAll = bMute;
    ApplyAudioSettings();
}

void AAudioManager::MuteAudioType(EAudioType AudioType, bool bMute)
{
    switch (AudioType)
    {
        case EAudioType::Master:
            AudioSettings.bMuteAll = bMute;
            break;
        case EAudioType::Music:
            AudioSettings.bMuteMusic = bMute;
            break;
        case EAudioType::SFX:
            AudioSettings.bMuteSFX = bMute;
            break;
        case EAudioType::Voice:
            AudioSettings.bMuteVoice = bMute;
            break;
        case EAudioType::Ambient:
            AudioSettings.bMuteAmbient = bMute;
            break;
    }
    
    ApplyAudioSettings();
    OnMuteChanged(AudioType, bMute);
}

bool AAudioManager::IsMuted(EAudioType AudioType) const
{
    if (AudioSettings.bMuteAll)
    {
        return true;
    }
    
    switch (AudioType)
    {
        case EAudioType::Master:
            return AudioSettings.bMuteAll;
        case EAudioType::Music:
            return AudioSettings.bMuteMusic;
        case EAudioType::SFX:
            return AudioSettings.bMuteSFX;
        case EAudioType::Voice:
            return AudioSettings.bMuteVoice;
        case EAudioType::Ambient:
            return AudioSettings.bMuteAmbient;
        default:
            return false;
    }
}

void AAudioManager::LoadAudioSettings()
{
    // TODO: Load settings from config file
    // For now, use default settings
    bSettingsLoaded = true;
}

void AAudioManager::SaveAudioSettings()
{
    // TODO: Save settings to config file
}

void AAudioManager::ApplyAudioSettings()
{
    UpdateAudioVolumes();
}

void AAudioManager::ResetAudioSettings()
{
    AudioSettings = FAudioSettings();
    ApplyAudioSettings();
}

void AAudioManager::RegisterVehicle(AActor* Vehicle, const FString& VehicleType)
{
    if (Vehicle)
    {
        RegisteredVehicles.Add(Vehicle, VehicleType);
        
        // Initialize audio components for this vehicle
        TMap<FString, UAudioComponent*>& VehicleAudioMap = VehicleAudioComponents.Add(Vehicle);
        
        // Create audio components for different sound types
        VehicleAudioMap.Add("EngineIdle", GetOrCreateAudioComponent(Vehicle, "EngineIdle"));
        VehicleAudioMap.Add("EngineRev", GetOrCreateAudioComponent(Vehicle, "EngineRev"));
        VehicleAudioMap.Add("EngineHighRPM", GetOrCreateAudioComponent(Vehicle, "EngineHighRPM"));
        VehicleAudioMap.Add("TireScreech", GetOrCreateAudioComponent(Vehicle, "TireScreech"));
        VehicleAudioMap.Add("Brake", GetOrCreateAudioComponent(Vehicle, "Brake"));
        VehicleAudioMap.Add("Crash", GetOrCreateAudioComponent(Vehicle, "Crash"));
        VehicleAudioMap.Add("Horn", GetOrCreateAudioComponent(Vehicle, "Horn"));
    }
}

void AAudioManager::UnregisterVehicle(AActor* Vehicle)
{
    if (Vehicle)
    {
        RegisteredVehicles.Remove(Vehicle);
        
        TMap<FString, UAudioComponent*>* VehicleAudioMap = VehicleAudioComponents.Find(Vehicle);
        if (VehicleAudioMap)
        {
            for (auto& AudioPair : *VehicleAudioMap)
            {
                if (AudioPair.Value)
                {
                    AudioPair.Value->Stop();
                    AudioPair.Value->DestroyComponent();
                }
            }
            VehicleAudioComponents.Remove(Vehicle);
        }
    }
}

void AAudioManager::UpdateVehicleAudio(AActor* Vehicle, float Speed, float RPM, bool bIsBraking, bool bIsScreeching)
{
    if (!Vehicle)
    {
        return;
    }
    
    // Update engine sound based on RPM
    if (RPM < 0.3f)
    {
        PlayVehicleSound(Vehicle, "EngineIdle", 1.0f, 1.0f);
        StopVehicleSound(Vehicle, "EngineRev");
        StopVehicleSound(Vehicle, "EngineHighRPM");
    }
    else if (RPM < 0.7f)
    {
        StopVehicleSound(Vehicle, "EngineIdle");
        PlayVehicleSound(Vehicle, "EngineRev", 0.5f + RPM, 1.0f);
        StopVehicleSound(Vehicle, "EngineHighRPM");
    }
    else
    {
        StopVehicleSound(Vehicle, "EngineIdle");
        StopVehicleSound(Vehicle, "EngineRev");
        PlayVehicleSound(Vehicle, "EngineHighRPM", 0.8f + RPM * 0.2f, 1.0f);
    }
    
    // Update tire screech sound
    if (bIsScreeching)
    {
        PlayVehicleSound(Vehicle, "TireScreech", 1.0f, 1.0f);
    }
    else
    {
        StopVehicleSound(Vehicle, "TireScreech");
    }
    
    // Update brake sound
    if (bIsBraking)
    {
        PlayVehicleSound(Vehicle, "Brake", 1.0f, 1.0f);
    }
    else
    {
        StopVehicleSound(Vehicle, "Brake");
    }
}

void AAudioManager::OnWeaponFired(AActor* Weapon, FVector Location)
{
    PlaySFX(WeaponFireSound, Location);
}

void AAudioManager::OnPlayerKilled(AActor* Killer, AActor* Victim)
{
    PlaySFX(KillSound);
    PlaySFX(DeathSound);
}

void AAudioManager::OnPlayerRespawned(AActor* Player)
{
    PlaySFX(RespawnSound);
}

void AAudioManager::OnExplosion(FVector Location, float Intensity)
{
    PlaySFX(ExplosionSound, Location, Intensity);
}

void AAudioManager::OnPowerUpCollected(AActor* Player, const FString& PowerUpType)
{
    PlaySFX(PowerUpSound);
}

void AAudioManager::InitializeAudioComponents()
{
    // Create music audio component
    MusicAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MusicAudioComponent"));
    MusicAudioComponent->bAutoActivate = false;
    MusicAudioComponent->bLooping = true;
    
    // Create ambient audio component
    AmbientAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AmbientAudioComponent"));
    AmbientAudioComponent->bAutoActivate = false;
    AmbientAudioComponent->bLooping = true;
    
    // Create voice audio component
    VoiceAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("VoiceAudioComponent"));
    VoiceAudioComponent->bAutoActivate = false;
    VoiceAudioComponent->bLooping = false;
}

void AAudioManager::UpdateAudioVolumes()
{
    if (MusicAudioComponent)
    {
        float MusicVolume = AudioSettings.bMuteAll || AudioSettings.bMuteMusic ? 0.0f : AudioSettings.MasterVolume * AudioSettings.MusicVolume;
        MusicAudioComponent->SetVolumeMultiplier(MusicVolume);
    }
    
    if (AmbientAudioComponent)
    {
        float AmbientVolume = AudioSettings.bMuteAll || AudioSettings.bMuteAmbient ? 0.0f : AudioSettings.MasterVolume * AudioSettings.AmbientVolume;
        AmbientAudioComponent->SetVolumeMultiplier(AmbientVolume);
    }
    
    if (VoiceAudioComponent)
    {
        float VoiceVolume = AudioSettings.bMuteAll || AudioSettings.bMuteVoice ? 0.0f : AudioSettings.MasterVolume * AudioSettings.VoiceVolume;
        VoiceAudioComponent->SetVolumeMultiplier(VoiceVolume);
    }
}

void AAudioManager::UpdateVehicleAudioComponents()
{
    for (auto& VehiclePair : VehicleAudioComponents)
    {
        AActor* Vehicle = VehiclePair.Key;
        TMap<FString, UAudioComponent*>& AudioMap = VehiclePair.Value;
        
        for (auto& AudioPair : AudioMap)
        {
            UAudioComponent* AudioComp = AudioPair.Value;
            if (AudioComp)
            {
                float SFXVolume = AudioSettings.bMuteAll || AudioSettings.bMuteSFX ? 0.0f : AudioSettings.MasterVolume * AudioSettings.SFXVolume;
                AudioComp->SetVolumeMultiplier(SFXVolume);
            }
        }
    }
}

UAudioComponent* AAudioManager::GetOrCreateAudioComponent(AActor* Owner, const FString& ComponentName)
{
    if (!Owner)
    {
        return nullptr;
    }
    
    // Check if component already exists
    UAudioComponent* ExistingComp = Owner->FindComponentByClass<UAudioComponent>();
    if (ExistingComp && ExistingComp->GetName() == ComponentName)
    {
        return ExistingComp;
    }
    
    // Create new audio component
    UAudioComponent* NewComp = NewObject<UAudioComponent>(Owner);
    NewComp->SetName(FName(*ComponentName));
    NewComp->bAutoActivate = false;
    NewComp->bLooping = false;
    NewComp->AttachToComponent(Owner->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
    NewComp->RegisterComponent();
    
    return NewComp;
}