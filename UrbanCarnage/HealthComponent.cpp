// Copyright Epic Games, Inc. All Rights Reserved.

#include "HealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"

UHealthComponent::UHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UHealthComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Initialize health
    CurrentHealth = MaxHealth;
    Armor = 0.0f;
    bIsDead = false;
    
    // Start regeneration if enabled
    if (bCanRegenerate)
    {
        StartRegeneration();
    }
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(UHealthComponent, MaxHealth);
    DOREPLIFETIME(UHealthComponent, CurrentHealth);
    DOREPLIFETIME(UHealthComponent, Armor);
    DOREPLIFETIME(UHealthComponent, bIsDead);
}

void UHealthComponent::TakeDamage(const FDamageInfo& DamageInfo)
{
    if (HasAuthority())
    {
        Server_TakeDamage(DamageInfo);
    }
}

void UHealthComponent::Heal(float HealAmount)
{
    if (HasAuthority())
    {
        Server_Heal(HealAmount);
    }
}

void UHealthComponent::AddArmor(float ArmorAmount)
{
    if (HasAuthority())
    {
        Server_AddArmor(ArmorAmount);
    }
}

void UHealthComponent::SetMaxHealth(float NewMaxHealth)
{
    if (HasAuthority())
    {
        MaxHealth = FMath::Max(1.0f, NewMaxHealth);
        CurrentHealth = FMath::Min(CurrentHealth, MaxHealth);
        Multicast_OnHealthChanged(CurrentHealth, MaxHealth);
    }
}

void UHealthComponent::SetInvulnerable(bool bInvulnerable)
{
    bIsInvulnerable = bInvulnerable;
}

void UHealthComponent::Revive(float ReviveHealth)
{
    if (HasAuthority())
    {
        Server_Revive(ReviveHealth);
    }
}

float UHealthComponent::CalculateDamageResistance(EDamageType DamageType) const
{
    switch (DamageType)
    {
        case EDamageType::Projectile:
            return ProjectileResistance;
        case EDamageType::Explosive:
            return ExplosiveResistance;
        case EDamageType::Collision:
            return CollisionResistance;
        default:
            return 0.0f;
    }
}

float UHealthComponent::CalculateFinalDamage(const FDamageInfo& DamageInfo) const
{
    if (bIsInvulnerable)
    {
        return 0.0f;
    }

    float FinalDamage = DamageInfo.DamageAmount;
    
    // Apply damage resistance
    float Resistance = CalculateDamageResistance(DamageInfo.DamageType);
    FinalDamage *= (1.0f - FMath::Clamp(Resistance, 0.0f, 0.95f)); // Cap resistance at 95%
    
    // Apply critical hit multiplier
    if (DamageInfo.bIsCriticalHit)
    {
        FinalDamage *= 1.5f; // 50% bonus damage for critical hits
    }
    
    return FMath::Max(0.0f, FinalDamage);
}

void UHealthComponent::StartRegeneration()
{
    if (bCanRegenerate && !bIsDead)
    {
        GetWorld()->GetTimerManager().SetTimer(RegenerationTimerHandle, this, &UHealthComponent::RegenerateHealth, 1.0f, true);
    }
}

void UHealthComponent::StopRegeneration()
{
    GetWorld()->GetTimerManager().ClearTimer(RegenerationTimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(RegenerationDelayTimerHandle);
}

void UHealthComponent::RegenerateHealth()
{
    if (bIsDead || IsAtFullHealth())
    {
        return;
    }

    if (CurrentHealth < MaxHealth)
    {
        float HealAmount = RegenerationRate * GetWorld()->GetDeltaSeconds();
        CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + HealAmount);
        Multicast_OnHealthChanged(CurrentHealth, MaxHealth);
    }
}

void UHealthComponent::Die()
{
    if (bIsDead)
    {
        return;
    }

    bIsDead = true;
    CurrentHealth = 0.0f;
    StopRegeneration();
    
    Multicast_OnDeath();
    OnDeath.Broadcast(GetOwner());
    OnDeathBP();
}

void UHealthComponent::UpdateHealthUI()
{
    Multicast_OnHealthChanged(CurrentHealth, MaxHealth);
}

// Server Functions
void UHealthComponent::Server_TakeDamage_Implementation(const FDamageInfo& DamageInfo)
{
    if (bIsDead || bIsInvulnerable)
    {
        return;
    }

    float FinalDamage = CalculateFinalDamage(DamageInfo);
    
    if (FinalDamage <= 0.0f)
    {
        return;
    }

    // Apply damage to armor first
    if (Armor > 0.0f)
    {
        float ArmorDamage = FMath::Min(Armor, FinalDamage * 0.5f); // Armor absorbs 50% of damage
        Armor -= ArmorDamage;
        FinalDamage -= ArmorDamage;
    }

    // Apply remaining damage to health
    CurrentHealth = FMath::Max(0.0f, CurrentHealth - FinalDamage);
    
    // Broadcast events
    Multicast_OnDamageTaken(FinalDamage, DamageInfo.DamageCauser, DamageInfo.DamageInstigator);
    Multicast_OnHealthChanged(CurrentHealth, MaxHealth);
    
    // Check for death
    if (CurrentHealth <= 0.0f)
    {
        Die();
    }
    else
    {
        // Start regeneration delay
        if (bCanRegenerate)
        {
            GetWorld()->GetTimerManager().ClearTimer(RegenerationDelayTimerHandle);
            GetWorld()->GetTimerManager().SetTimer(RegenerationDelayTimerHandle, this, &UHealthComponent::StartRegeneration, RegenerationDelay, false);
            StopRegeneration();
        }
    }
}

void UHealthComponent::Server_Heal_Implementation(float HealAmount)
{
    if (bIsDead || HealAmount <= 0.0f)
    {
        return;
    }

    float OldHealth = CurrentHealth;
    CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + HealAmount);
    
    if (CurrentHealth != OldHealth)
    {
        Multicast_OnHealed(CurrentHealth - OldHealth);
        Multicast_OnHealthChanged(CurrentHealth, MaxHealth);
    }
}

void UHealthComponent::Server_AddArmor_Implementation(float ArmorAmount)
{
    if (ArmorAmount <= 0.0f)
    {
        return;
    }

    float OldArmor = Armor;
    Armor = FMath::Min(MaxArmor, Armor + ArmorAmount);
    
    if (Armor != OldArmor)
    {
        Multicast_OnArmorAdded(Armor - OldArmor);
    }
}

void UHealthComponent::Server_Revive_Implementation(float ReviveHealth)
{
    if (!bIsDead)
    {
        return;
    }

    bIsDead = false;
    CurrentHealth = FMath::Clamp(ReviveHealth, 1.0f, MaxHealth);
    Armor = 0.0f; // Reset armor on revive
    
    Multicast_OnHealthChanged(CurrentHealth, MaxHealth);
    
    if (bCanRegenerate)
    {
        StartRegeneration();
    }
}

// Multicast Functions
void UHealthComponent::Multicast_OnHealthChanged_Implementation(float NewHealth, float NewMaxHealth)
{
    OnHealthChanged.Broadcast(NewHealth, NewMaxHealth);
    OnHealthChangedBP(NewHealth, NewMaxHealth);
}

void UHealthComponent::Multicast_OnDeath_Implementation()
{
    OnDeath.Broadcast(GetOwner());
    OnDeathBP();
}

void UHealthComponent::Multicast_OnDamageTaken_Implementation(float DamageAmount, AActor* DamageCauser, AActor* DamageInstigator)
{
    OnDamageTaken.Broadcast(DamageAmount, DamageCauser, DamageInstigator);
    OnDamageTakenBP(DamageAmount, DamageCauser);
}

void UHealthComponent::Multicast_OnHealed_Implementation(float HealAmount)
{
    OnHealedBP(HealAmount);
}

void UHealthComponent::Multicast_OnArmorAdded_Implementation(float ArmorAmount)
{
    OnArmorAddedBP(ArmorAmount);
}