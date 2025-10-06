// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "EffectsManager.generated.h"

UENUM(BlueprintType)
enum class EEffectType : uint8
{
    Explosion     UMETA(DisplayName = "Explosion"),
    MuzzleFlash   UMETA(DisplayName = "Muzzle Flash"),
    Impact        UMETA(DisplayName = "Impact"),
    Trail         UMETA(DisplayName = "Trail"),
    Smoke         UMETA(DisplayName = "Smoke"),
    Fire          UMETA(DisplayName = "Fire"),
    Sparks        UMETA(DisplayName = "Sparks"),
    Debris        UMETA(DisplayName = "Debris"),
    Blood         UMETA(DisplayName = "Blood"),
    Water         UMETA(DisplayName = "Water"),
    Dust          UMETA(DisplayName = "Dust"),
    Lightning     UMETA(DisplayName = "Lightning")
};

USTRUCT(BlueprintType)
struct FEffectData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Data")
    EEffectType EffectType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Data")
    UParticleSystem* ParticleSystem;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Data")
    USoundCue* SoundCue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Data")
    UStaticMesh* Mesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Data")
    FLinearColor Color = FLinearColor::White;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Data")
    float Scale = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Data")
    float Duration = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Data")
    bool bLooping = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Data")
    bool bAttachToActor = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Data")
    FName AttachSocket = NAME_None;

    FEffectData()
    {
        EffectType = EEffectType::Explosion;
        ParticleSystem = nullptr;
        SoundCue = nullptr;
        Mesh = nullptr;
        Color = FLinearColor::White;
        Scale = 1.0f;
        Duration = 1.0f;
        bLooping = false;
        bAttachToActor = false;
        AttachSocket = NAME_None;
    }
};

UCLASS()
class URBANCARNAGE_API AEffectsManager : public AActor
{
    GENERATED_BODY()

public:
    AEffectsManager();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
    // Effect Data
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Management")
    TMap<EEffectType, FEffectData> EffectDatabase;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Management")
    TMap<FString, FEffectData> CustomEffects;

    // Effect Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Settings")
    bool bEnableEffects = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Settings")
    bool bEnableParticles = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Settings")
    bool bEnableSounds = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Settings")
    bool bEnableMeshes = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Settings")
    float EffectQuality = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Settings")
    int32 MaxActiveEffects = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Settings")
    float EffectCullDistance = 5000.0f;

    // Effect Functions
    UFUNCTION(BlueprintCallable, Category = "Effect Management")
    void PlayEffect(EEffectType EffectType, FVector Location, FRotator Rotation = FRotator::ZeroRotator, AActor* AttachTo = nullptr, FName SocketName = NAME_None);

    UFUNCTION(BlueprintCallable, Category = "Effect Management")
    void PlayCustomEffect(const FString& EffectName, FVector Location, FRotator Rotation = FRotator::ZeroRotator, AActor* AttachTo = nullptr, FName SocketName = NAME_None);

    UFUNCTION(BlueprintCallable, Category = "Effect Management")
    void StopEffect(AActor* EffectActor);

    UFUNCTION(BlueprintCallable, Category = "Effect Management")
    void StopAllEffects();

    UFUNCTION(BlueprintCallable, Category = "Effect Management")
    void StopEffectsByType(EEffectType EffectType);

    UFUNCTION(BlueprintCallable, Category = "Effect Management")
    void StopEffectsAttachedTo(AActor* AttachedActor);

    // Weapon Effects
    UFUNCTION(BlueprintCallable, Category = "Weapon Effects")
    void PlayMuzzleFlash(FVector Location, FRotator Rotation, AActor* Weapon);

    UFUNCTION(BlueprintCallable, Category = "Weapon Effects")
    void PlayBulletTrail(FVector StartLocation, FVector EndLocation, float Speed = 1000.0f);

    UFUNCTION(BlueprintCallable, Category = "Weapon Effects")
    void PlayImpactEffect(FVector Location, FVector Normal, const FString& SurfaceType = "Default");

    UFUNCTION(BlueprintCallable, Category = "Weapon Effects")
    void PlayRicochetEffect(FVector Location, FVector Direction);

    // Vehicle Effects
    UFUNCTION(BlueprintCallable, Category = "Vehicle Effects")
    void PlayEngineSmoke(FVector Location, FRotator Rotation, AActor* Vehicle);

    UFUNCTION(BlueprintCallable, Category = "Vehicle Effects")
    void PlayTireSmoke(FVector Location, FRotator Rotation, AActor* Vehicle);

    UFUNCTION(BlueprintCallable, Category = "Vehicle Effects")
    void PlayExhaustFlame(FVector Location, FRotator Rotation, AActor* Vehicle);

    UFUNCTION(BlueprintCallable, Category = "Vehicle Effects")
    void PlayCrashEffect(FVector Location, FRotator Rotation, float Intensity = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Vehicle Effects")
    void PlaySkidMarks(FVector StartLocation, FVector EndLocation, float Width = 20.0f);

    // Environmental Effects
    UFUNCTION(BlueprintCallable, Category = "Environmental Effects")
    void PlayExplosion(FVector Location, float Intensity = 1.0f, float Radius = 500.0f);

    UFUNCTION(BlueprintCallable, Category = "Environmental Effects")
    void PlayFire(FVector Location, FRotator Rotation, bool bLooping = true);

    UFUNCTION(BlueprintCallable, Category = "Environmental Effects")
    void PlaySmoke(FVector Location, FRotator Rotation, bool bLooping = true);

    UFUNCTION(BlueprintCallable, Category = "Environmental Effects")
    void PlayDust(FVector Location, FRotator Rotation, float Intensity = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Environmental Effects")
    void PlayWaterSplash(FVector Location, FRotator Rotation, float Intensity = 1.0f);

    // Weather Effects
    UFUNCTION(BlueprintCallable, Category = "Weather Effects")
    void PlayRain(FVector Location, float Intensity = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Weather Effects")
    void PlaySnow(FVector Location, float Intensity = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Weather Effects")
    void PlayLightning(FVector Location, FRotator Rotation);

    UFUNCTION(BlueprintCallable, Category = "Weather Effects")
    void PlayWind(FVector Location, FVector Direction, float Strength = 1.0f);

    // UI Effects
    UFUNCTION(BlueprintCallable, Category = "UI Effects")
    void PlayHitMarker(FVector2D ScreenPosition);

    UFUNCTION(BlueprintCallable, Category = "UI Effects")
    void PlayDamageIndicator(FVector2D ScreenPosition, float Damage);

    UFUNCTION(BlueprintCallable, Category = "UI Effects")
    void PlayKillFeed(FString KillerName, FString VictimName, FString WeaponName);

    UFUNCTION(BlueprintCallable, Category = "UI Effects")
    void PlayNotification(FString Message, FLinearColor Color = FLinearColor::White);

    // Effect Management
    UFUNCTION(BlueprintCallable, Category = "Effect Management")
    void SetEffectQuality(float Quality);

    UFUNCTION(BlueprintCallable, Category = "Effect Management")
    void EnableEffects(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "Effect Management")
    void EnableParticles(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "Effect Management")
    void EnableSounds(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "Effect Management")
    void EnableMeshes(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "Effect Management")
    void SetMaxActiveEffects(int32 MaxEffects);

    UFUNCTION(BlueprintCallable, Category = "Effect Management")
    void SetEffectCullDistance(float Distance);

    // Effect Queries
    UFUNCTION(BlueprintCallable, Category = "Effect Queries")
    int32 GetActiveEffectCount() const;

    UFUNCTION(BlueprintCallable, Category = "Effect Queries")
    int32 GetActiveEffectCountByType(EEffectType EffectType) const;

    UFUNCTION(BlueprintCallable, Category = "Effect Queries")
    TArray<AActor*> GetActiveEffects() const;

    UFUNCTION(BlueprintCallable, Category = "Effect Queries")
    TArray<AActor*> GetActiveEffectsByType(EEffectType EffectType) const;

    UFUNCTION(BlueprintCallable, Category = "Effect Queries")
    TArray<AActor*> GetActiveEffectsAttachedTo(AActor* AttachedActor) const;

    // Blueprint Events
    UFUNCTION(BlueprintImplementableEvent, Category = "Effect Events")
    void OnEffectPlayed(EEffectType EffectType, FVector Location, FRotator Rotation);

    UFUNCTION(BlueprintImplementableEvent, Category = "Effect Events")
    void OnEffectStopped(AActor* EffectActor);

    UFUNCTION(BlueprintImplementableEvent, Category = "Effect Events")
    void OnEffectQualityChanged(float NewQuality);

    UFUNCTION(BlueprintImplementableEvent, Category = "Effect Events")
    void OnMaxEffectsReached();

protected:
    // Internal Functions
    void InitializeEffectDatabase();
    void UpdateActiveEffects();
    void CullDistantEffects();
    void CleanupFinishedEffects();
    AActor* CreateEffectActor(const FEffectData& EffectData, FVector Location, FRotator Rotation, AActor* AttachTo = nullptr, FName SocketName = NAME_None);
    void AttachEffectToActor(AActor* EffectActor, AActor* AttachTo, FName SocketName);
    void DetachEffectFromActor(AActor* EffectActor);
    bool ShouldCullEffect(AActor* EffectActor) const;

    // Active Effects
    UPROPERTY()
    TArray<AActor*> ActiveEffects;

    UPROPERTY()
    TMap<AActor*, EEffectType> EffectTypes;

    UPROPERTY()
    TMap<AActor*, AActor*> EffectAttachments;

    UPROPERTY()
    TMap<AActor*, float> EffectStartTimes;

    UPROPERTY()
    TMap<AActor*, float> EffectDurations;

    // Effect Components
    UPROPERTY()
    TMap<AActor*, UParticleSystemComponent*> EffectParticleComponents;

    UPROPERTY()
    TMap<AActor*, UAudioComponent*> EffectAudioComponents;

    UPROPERTY()
    TMap<AActor*, UStaticMeshComponent*> EffectMeshComponents;

    // Timer Handles
    FTimerHandle EffectUpdateTimerHandle;
    FTimerHandle EffectCullTimerHandle;
    FTimerHandle EffectCleanupTimerHandle;

    // Statistics
    UPROPERTY(BlueprintReadOnly, Category = "Effect Statistics")
    int32 TotalEffectsPlayed = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Effect Statistics")
    int32 TotalEffectsStopped = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Effect Statistics")
    float AverageEffectDuration = 0.0f;
};