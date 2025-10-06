// Copyright Epic Games, Inc. All Rights Reserved.

#include "EffectsManager.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

AEffectsManager::AEffectsManager()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = false;
}

void AEffectsManager::BeginPlay()
{
    Super::BeginPlay();
    
    InitializeEffectDatabase();
    
    // Set up timers
    GetWorldTimerManager().SetTimer(EffectUpdateTimerHandle, this, &AEffectsManager::UpdateActiveEffects, 0.1f, true);
    GetWorldTimerManager().SetTimer(EffectCullTimerHandle, this, &AEffectsManager::CullDistantEffects, 1.0f, true);
    GetWorldTimerManager().SetTimer(EffectCleanupTimerHandle, this, &AEffectsManager::CleanupFinishedEffects, 5.0f, true);
}

void AEffectsManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    UpdateActiveEffects();
}

void AEffectsManager::PlayEffect(EEffectType EffectType, FVector Location, FRotator Rotation, AActor* AttachTo, FName SocketName)
{
    if (!bEnableEffects)
    {
        return;
    }
    
    if (FEffectData* EffectData = EffectDatabase.Find(EffectType))
    {
        PlayCustomEffect(EffectType.ToString(), Location, Rotation, AttachTo, SocketName);
    }
}

void AEffectsManager::PlayCustomEffect(const FString& EffectName, FVector Location, FRotator Rotation, AActor* AttachTo, FName SocketName)
{
    if (!bEnableEffects)
    {
        return;
    }
    
    // Check if we've reached max active effects
    if (ActiveEffects.Num() >= MaxActiveEffects)
    {
        OnMaxEffectsReached();
        return;
    }
    
    FEffectData* EffectData = nullptr;
    
    // Try to find in custom effects first
    if (FEffectData* CustomEffect = CustomEffects.Find(EffectName))
    {
        EffectData = CustomEffect;
    }
    else
    {
        // Try to find by effect type
        for (const auto& EffectPair : EffectDatabase)
        {
            if (EffectPair.Key.ToString() == EffectName)
            {
                EffectData = const_cast<FEffectData*>(&EffectPair.Value);
                break;
            }
        }
    }
    
    if (!EffectData)
    {
        return;
    }
    
    // Create effect actor
    AActor* EffectActor = CreateEffectActor(*EffectData, Location, Rotation, AttachTo, SocketName);
    if (!EffectActor)
    {
        return;
    }
    
    // Add to active effects
    ActiveEffects.Add(EffectActor);
    EffectTypes.Add(EffectActor, EffectData->EffectType);
    EffectStartTimes.Add(EffectActor, GetWorld()->GetTimeSeconds());
    EffectDurations.Add(EffectActor, EffectData->Duration);
    
    if (AttachTo)
    {
        EffectAttachments.Add(EffectActor, AttachTo);
    }
    
    // Update statistics
    TotalEffectsPlayed++;
    
    OnEffectPlayed(EffectData->EffectType, Location, Rotation);
}

void AEffectsManager::StopEffect(AActor* EffectActor)
{
    if (!EffectActor)
    {
        return;
    }
    
    // Stop particle system
    if (UParticleSystemComponent** ParticleComp = EffectParticleComponents.Find(EffectActor))
    {
        if (*ParticleComp)
        {
            (*ParticleComp)->Deactivate();
        }
    }
    
    // Stop audio
    if (UAudioComponent** AudioComp = EffectAudioComponents.Find(EffectActor))
    {
        if (*AudioComp)
        {
            (*AudioComp)->Stop();
        }
    }
    
    // Detach from actor
    DetachEffectFromActor(EffectActor);
    
    // Remove from active effects
    ActiveEffects.Remove(EffectActor);
    EffectTypes.Remove(EffectActor);
    EffectAttachments.Remove(EffectActor);
    EffectStartTimes.Remove(EffectActor);
    EffectDurations.Remove(EffectActor);
    EffectParticleComponents.Remove(EffectActor);
    EffectAudioComponents.Remove(EffectActor);
    EffectMeshComponents.Remove(EffectActor);
    
    // Update statistics
    TotalEffectsStopped++;
    
    OnEffectStopped(EffectActor);
    
    // Destroy actor
    EffectActor->Destroy();
}

void AEffectsManager::StopAllEffects()
{
    TArray<AActor*> EffectsToStop = ActiveEffects;
    
    for (AActor* EffectActor : EffectsToStop)
    {
        StopEffect(EffectActor);
    }
}

void AEffectsManager::StopEffectsByType(EEffectType EffectType)
{
    TArray<AActor*> EffectsToStop;
    
    for (const auto& EffectPair : EffectTypes)
    {
        if (EffectPair.Value == EffectType)
        {
            EffectsToStop.Add(EffectPair.Key);
        }
    }
    
    for (AActor* EffectActor : EffectsToStop)
    {
        StopEffect(EffectActor);
    }
}

void AEffectsManager::StopEffectsAttachedTo(AActor* AttachedActor)
{
    TArray<AActor*> EffectsToStop;
    
    for (const auto& AttachmentPair : EffectAttachments)
    {
        if (AttachmentPair.Value == AttachedActor)
        {
            EffectsToStop.Add(AttachmentPair.Key);
        }
    }
    
    for (AActor* EffectActor : EffectsToStop)
    {
        StopEffect(EffectActor);
    }
}

void AEffectsManager::PlayMuzzleFlash(FVector Location, FRotator Rotation, AActor* Weapon)
{
    PlayEffect(EEffectType::MuzzleFlash, Location, Rotation, Weapon);
}

void AEffectsManager::PlayBulletTrail(FVector StartLocation, FVector EndLocation, float Speed)
{
    FVector Direction = (EndLocation - StartLocation).GetSafeNormal();
    FRotator Rotation = Direction.Rotation();
    
    PlayEffect(EEffectType::Trail, StartLocation, Rotation);
}

void AEffectsManager::PlayImpactEffect(FVector Location, FVector Normal, const FString& SurfaceType)
{
    FRotator Rotation = Normal.Rotation();
    
    // Choose effect based on surface type
    EEffectType EffectType = EEffectType::Impact;
    if (SurfaceType == "Metal")
    {
        EffectType = EEffectType::Sparks;
    }
    else if (SurfaceType == "Water")
    {
        EffectType = EEffectType::Water;
    }
    else if (SurfaceType == "Dirt")
    {
        EffectType = EEffectType::Dust;
    }
    
    PlayEffect(EffectType, Location, Rotation);
}

void AEffectsManager::PlayRicochetEffect(FVector Location, FVector Direction)
{
    FRotator Rotation = Direction.Rotation();
    PlayEffect(EEffectType::Sparks, Location, Rotation);
}

void AEffectsManager::PlayEngineSmoke(FVector Location, FRotator Rotation, AActor* Vehicle)
{
    PlayEffect(EEffectType::Smoke, Location, Rotation, Vehicle);
}

void AEffectsManager::PlayTireSmoke(FVector Location, FRotator Rotation, AActor* Vehicle)
{
    PlayEffect(EEffectType::Smoke, Location, Rotation, Vehicle);
}

void AEffectsManager::PlayExhaustFlame(FVector Location, FRotator Rotation, AActor* Vehicle)
{
    PlayEffect(EEffectType::Fire, Location, Rotation, Vehicle);
}

void AEffectsManager::PlayCrashEffect(FVector Location, FRotator Rotation, float Intensity)
{
    PlayEffect(EEffectType::Explosion, Location, Rotation);
    PlayEffect(EEffectType::Debris, Location, Rotation);
    PlayEffect(EEffectType::Sparks, Location, Rotation);
}

void AEffectsManager::PlaySkidMarks(FVector StartLocation, FVector EndLocation, float Width)
{
    // This would create a decal or mesh for skid marks
    // Implementation would depend on the specific requirements
}

void AEffectsManager::PlayExplosion(FVector Location, float Intensity, float Radius)
{
    PlayEffect(EEffectType::Explosion, Location, FRotator::ZeroRotator);
    PlayEffect(EEffectType::Fire, Location, FRotator::ZeroRotator);
    PlayEffect(EEffectType::Smoke, Location, FRotator::ZeroRotator);
    PlayEffect(EEffectType::Debris, Location, FRotator::ZeroRotator);
}

void AEffectsManager::PlayFire(FVector Location, FRotator Rotation, bool bLooping)
{
    PlayEffect(EEffectType::Fire, Location, Rotation);
}

void AEffectsManager::PlaySmoke(FVector Location, FRotator Rotation, bool bLooping)
{
    PlayEffect(EEffectType::Smoke, Location, Rotation);
}

void AEffectsManager::PlayDust(FVector Location, FRotator Rotation, float Intensity)
{
    PlayEffect(EEffectType::Dust, Location, Rotation);
}

void AEffectsManager::PlayWaterSplash(FVector Location, FRotator Rotation, float Intensity)
{
    PlayEffect(EEffectType::Water, Location, Rotation);
}

void AEffectsManager::PlayRain(FVector Location, float Intensity)
{
    // This would create a rain particle system
    // Implementation would depend on the specific requirements
}

void AEffectsManager::PlaySnow(FVector Location, float Intensity)
{
    // This would create a snow particle system
    // Implementation would depend on the specific requirements
}

void AEffectsManager::PlayLightning(FVector Location, FRotator Rotation)
{
    PlayEffect(EEffectType::Lightning, Location, Rotation);
}

void AEffectsManager::PlayWind(FVector Location, FVector Direction, float Strength)
{
    // This would create a wind effect
    // Implementation would depend on the specific requirements
}

void AEffectsManager::PlayHitMarker(FVector2D ScreenPosition)
{
    // This would create a UI hit marker effect
    // Implementation would depend on the specific requirements
}

void AEffectsManager::PlayDamageIndicator(FVector2D ScreenPosition, float Damage)
{
    // This would create a UI damage indicator effect
    // Implementation would depend on the specific requirements
}

void AEffectsManager::PlayKillFeed(FString KillerName, FString VictimName, FString WeaponName)
{
    // This would create a UI kill feed effect
    // Implementation would depend on the specific requirements
}

void AEffectsManager::PlayNotification(FString Message, FLinearColor Color)
{
    // This would create a UI notification effect
    // Implementation would depend on the specific requirements
}

void AEffectsManager::SetEffectQuality(float Quality)
{
    EffectQuality = FMath::Clamp(Quality, 0.0f, 1.0f);
    OnEffectQualityChanged(EffectQuality);
}

void AEffectsManager::EnableEffects(bool bEnable)
{
    bEnableEffects = bEnable;
    
    if (!bEnable)
    {
        StopAllEffects();
    }
}

void AEffectsManager::EnableParticles(bool bEnable)
{
    bEnableParticles = bEnable;
}

void AEffectsManager::EnableSounds(bool bEnable)
{
    bEnableSounds = bEnable;
}

void AEffectsManager::EnableMeshes(bool bEnable)
{
    bEnableMeshes = bEnable;
}

void AEffectsManager::SetMaxActiveEffects(int32 MaxEffects)
{
    MaxActiveEffects = FMath::Max(1, MaxEffects);
    
    // Stop excess effects if necessary
    while (ActiveEffects.Num() > MaxActiveEffects)
    {
        StopEffect(ActiveEffects[0]);
    }
}

void AEffectsManager::SetEffectCullDistance(float Distance)
{
    EffectCullDistance = FMath::Max(0.0f, Distance);
}

int32 AEffectsManager::GetActiveEffectCount() const
{
    return ActiveEffects.Num();
}

int32 AEffectsManager::GetActiveEffectCountByType(EEffectType EffectType) const
{
    int32 Count = 0;
    
    for (const auto& EffectPair : EffectTypes)
    {
        if (EffectPair.Value == EffectType)
        {
            Count++;
        }
    }
    
    return Count;
}

TArray<AActor*> AEffectsManager::GetActiveEffects() const
{
    return ActiveEffects;
}

TArray<AActor*> AEffectsManager::GetActiveEffectsByType(EEffectType EffectType) const
{
    TArray<AActor*> Effects;
    
    for (const auto& EffectPair : EffectTypes)
    {
        if (EffectPair.Value == EffectType)
        {
            Effects.Add(EffectPair.Key);
        }
    }
    
    return Effects;
}

TArray<AActor*> AEffectsManager::GetActiveEffectsAttachedTo(AActor* AttachedActor) const
{
    TArray<AActor*> Effects;
    
    for (const auto& AttachmentPair : EffectAttachments)
    {
        if (AttachmentPair.Value == AttachedActor)
        {
            Effects.Add(AttachmentPair.Key);
        }
    }
    
    return Effects;
}

void AEffectsManager::InitializeEffectDatabase()
{
    // Initialize default effects
    // This would be populated with actual particle systems, sounds, and meshes
    
    // Explosion Effect
    FEffectData ExplosionEffect;
    ExplosionEffect.EffectType = EEffectType::Explosion;
    ExplosionEffect.Duration = 3.0f;
    ExplosionEffect.Scale = 1.0f;
    ExplosionEffect.Color = FLinearColor::Red;
    EffectDatabase.Add(EEffectType::Explosion, ExplosionEffect);
    
    // Muzzle Flash Effect
    FEffectData MuzzleFlashEffect;
    MuzzleFlashEffect.EffectType = EEffectType::MuzzleFlash;
    MuzzleFlashEffect.Duration = 0.1f;
    MuzzleFlashEffect.Scale = 0.5f;
    MuzzleFlashEffect.Color = FLinearColor::Yellow;
    EffectDatabase.Add(EEffectType::MuzzleFlash, MuzzleFlashEffect);
    
    // Impact Effect
    FEffectData ImpactEffect;
    ImpactEffect.EffectType = EEffectType::Impact;
    ImpactEffect.Duration = 1.0f;
    ImpactEffect.Scale = 0.3f;
    ImpactEffect.Color = FLinearColor::Orange;
    EffectDatabase.Add(EEffectType::Impact, ImpactEffect);
    
    // Trail Effect
    FEffectData TrailEffect;
    TrailEffect.EffectType = EEffectType::Trail;
    TrailEffect.Duration = 2.0f;
    TrailEffect.Scale = 0.1f;
    TrailEffect.Color = FLinearColor::White;
    EffectDatabase.Add(EEffectType::Trail, TrailEffect);
    
    // Smoke Effect
    FEffectData SmokeEffect;
    SmokeEffect.EffectType = EEffectType::Smoke;
    SmokeEffect.Duration = 5.0f;
    SmokeEffect.Scale = 1.0f;
    SmokeEffect.Color = FLinearColor(0.5f, 0.5f, 0.5f, 0.8f);
    EffectDatabase.Add(EEffectType::Smoke, SmokeEffect);
    
    // Fire Effect
    FEffectData FireEffect;
    FireEffect.EffectType = EEffectType::Fire;
    FireEffect.Duration = 10.0f;
    FireEffect.Scale = 1.0f;
    FireEffect.Color = FLinearColor::Red;
    FireEffect.bLooping = true;
    EffectDatabase.Add(EEffectType::Fire, FireEffect);
    
    // Sparks Effect
    FEffectData SparksEffect;
    SparksEffect.EffectType = EEffectType::Sparks;
    SparksEffect.Duration = 0.5f;
    SparksEffect.Scale = 0.2f;
    SparksEffect.Color = FLinearColor::Yellow;
    EffectDatabase.Add(EEffectType::Sparks, SparksEffect);
    
    // Debris Effect
    FEffectData DebrisEffect;
    DebrisEffect.EffectType = EEffectType::Debris;
    DebrisEffect.Duration = 3.0f;
    DebrisEffect.Scale = 0.5f;
    DebrisEffect.Color = FLinearColor::Brown;
    EffectDatabase.Add(EEffectType::Debris, DebrisEffect);
    
    // Water Effect
    FEffectData WaterEffect;
    WaterEffect.EffectType = EEffectType::Water;
    WaterEffect.Duration = 1.0f;
    WaterEffect.Scale = 0.8f;
    WaterEffect.Color = FLinearColor::Blue;
    EffectDatabase.Add(EEffectType::Water, WaterEffect);
    
    // Dust Effect
    FEffectData DustEffect;
    DustEffect.EffectType = EEffectType::Dust;
    DustEffect.Duration = 2.0f;
    DustEffect.Scale = 0.6f;
    DustEffect.Color = FLinearColor(0.8f, 0.6f, 0.4f, 0.6f);
    EffectDatabase.Add(EEffectType::Dust, DustEffect);
    
    // Lightning Effect
    FEffectData LightningEffect;
    LightningEffect.EffectType = EEffectType::Lightning;
    LightningEffect.Duration = 0.2f;
    LightningEffect.Scale = 2.0f;
    LightningEffect.Color = FLinearColor::White;
    EffectDatabase.Add(EEffectType::Lightning, LightningEffect);
}

void AEffectsManager::UpdateActiveEffects()
{
    // Update effect positions if attached to actors
    for (const auto& AttachmentPair : EffectAttachments)
    {
        AActor* EffectActor = AttachmentPair.Key;
        AActor* AttachedActor = AttachmentPair.Value;
        
        if (EffectActor && AttachedActor)
        {
            EffectActor->SetActorLocation(AttachedActor->GetActorLocation());
            EffectActor->SetActorRotation(AttachedActor->GetActorRotation());
        }
    }
}

void AEffectsManager::CullDistantEffects()
{
    if (EffectCullDistance <= 0.0f)
    {
        return;
    }
    
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC)
    {
        return;
    }
    
    FVector PlayerLocation = PC->GetPawn() ? PC->GetPawn()->GetActorLocation() : FVector::ZeroVector;
    
    TArray<AActor*> EffectsToCull;
    
    for (AActor* EffectActor : ActiveEffects)
    {
        if (ShouldCullEffect(EffectActor))
        {
            float Distance = FVector::Dist(EffectActor->GetActorLocation(), PlayerLocation);
            if (Distance > EffectCullDistance)
            {
                EffectsToCull.Add(EffectActor);
            }
        }
    }
    
    for (AActor* EffectActor : EffectsToCull)
    {
        StopEffect(EffectActor);
    }
}

void AEffectsManager::CleanupFinishedEffects()
{
    float CurrentTime = GetWorld()->GetTimeSeconds();
    TArray<AActor*> EffectsToCleanup;
    
    for (const auto& TimePair : EffectStartTimes)
    {
        AActor* EffectActor = TimePair.Key;
        float StartTime = TimePair.Value;
        
        if (float* Duration = EffectDurations.Find(EffectActor))
        {
            if (CurrentTime - StartTime >= *Duration)
            {
                EffectsToCleanup.Add(EffectActor);
            }
        }
    }
    
    for (AActor* EffectActor : EffectsToCleanup)
    {
        StopEffect(EffectActor);
    }
}

AActor* AEffectsManager::CreateEffectActor(const FEffectData& EffectData, FVector Location, FRotator Rotation, AActor* AttachTo, FName SocketName)
{
    // Create a new actor for the effect
    AActor* EffectActor = GetWorld()->SpawnActor<AActor>();
    if (!EffectActor)
    {
        return nullptr;
    }
    
    EffectActor->SetActorLocation(Location);
    EffectActor->SetActorRotation(Rotation);
    
    // Create particle system component
    if (bEnableParticles && EffectData.ParticleSystem)
    {
        UParticleSystemComponent* ParticleComp = NewObject<UParticleSystemComponent>(EffectActor);
        ParticleComp->SetTemplate(EffectData.ParticleSystem);
        ParticleComp->SetRelativeScale3D(FVector(EffectData.Scale));
        ParticleComp->SetColorParameter(FName("Color"), EffectData.Color);
        ParticleComp->Activate();
        ParticleComp->AttachToComponent(EffectActor->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
        
        EffectParticleComponents.Add(EffectActor, ParticleComp);
    }
    
    // Create audio component
    if (bEnableSounds && EffectData.SoundCue)
    {
        UAudioComponent* AudioComp = NewObject<UAudioComponent>(EffectActor);
        AudioComp->SetSound(EffectData.SoundCue);
        AudioComp->SetVolumeMultiplier(EffectQuality);
        AudioComp->Play();
        AudioComp->AttachToComponent(EffectActor->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
        
        EffectAudioComponents.Add(EffectActor, AudioComp);
    }
    
    // Create mesh component
    if (bEnableMeshes && EffectData.Mesh)
    {
        UStaticMeshComponent* MeshComp = NewObject<UStaticMeshComponent>(EffectActor);
        MeshComp->SetStaticMesh(EffectData.Mesh);
        MeshComp->SetRelativeScale3D(FVector(EffectData.Scale));
        MeshComp->SetMaterial(0, nullptr); // Set material based on color
        MeshComp->AttachToComponent(EffectActor->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
        
        EffectMeshComponents.Add(EffectActor, MeshComp);
    }
    
    // Attach to actor if specified
    if (AttachTo)
    {
        AttachEffectToActor(EffectActor, AttachTo, SocketName);
    }
    
    return EffectActor;
}

void AEffectsManager::AttachEffectToActor(AActor* EffectActor, AActor* AttachTo, FName SocketName)
{
    if (!EffectActor || !AttachTo)
    {
        return;
    }
    
    if (SocketName != NAME_None)
    {
        // Attach to specific socket
        EffectActor->AttachToActor(AttachTo, FAttachmentTransformRules::KeepWorldTransform, SocketName);
    }
    else
    {
        // Attach to root component
        EffectActor->AttachToActor(AttachTo, FAttachmentTransformRules::KeepWorldTransform);
    }
}

void AEffectsManager::DetachEffectFromActor(AActor* EffectActor)
{
    if (!EffectActor)
    {
        return;
    }
    
    EffectActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

bool AEffectsManager::ShouldCullEffect(AActor* EffectActor) const
{
    if (!EffectActor)
    {
        return false;
    }
    
    // Check if effect is attached to an actor
    if (EffectAttachments.Contains(EffectActor))
    {
        return false; // Don't cull attached effects
    }
    
    return true;
}