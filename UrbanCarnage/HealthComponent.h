// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, AActor*, DeadActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDamageTaken, float, DamageAmount, AActor*, DamageCauser, AActor*, DamageInstigator);

UENUM(BlueprintType)
enum class EDamageType : uint8
{
    None        UMETA(DisplayName = "None"),
    Projectile  UMETA(DisplayName = "Projectile"),
    Explosive   UMETA(DisplayName = "Explosive"),
    Collision   UMETA(DisplayName = "Collision"),
    Environmental UMETA(DisplayName = "Environmental"),
    Fall        UMETA(DisplayName = "Fall")
};

USTRUCT(BlueprintType)
struct FDamageInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DamageAmount = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EDamageType DamageType = EDamageType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* DamageCauser = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* DamageInstigator = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector HitLocation = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector HitDirection = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCanBeBlocked = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsCriticalHit = false;

    FDamageInfo()
    {
        DamageAmount = 0.0f;
        DamageType = EDamageType::None;
        DamageCauser = nullptr;
        DamageInstigator = nullptr;
        HitLocation = FVector::ZeroVector;
        HitDirection = FVector::ZeroVector;
        bCanBeBlocked = true;
        bIsCriticalHit = false;
    }
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class URBANCARNAGE_API UHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UHealthComponent();

protected:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
    // Health Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Health")
    float MaxHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Health")
    float CurrentHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    float Armor = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    float MaxArmor = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    bool bCanRegenerate = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    float RegenerationRate = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    float RegenerationDelay = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    bool bIsInvulnerable = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    bool bIsDead = false;

    // Damage Resistance
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Resistance")
    float ProjectileResistance = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Resistance")
    float ExplosiveResistance = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Resistance")
    float CollisionResistance = 0.0f;

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Health Events")
    FOnHealthChanged OnHealthChanged;

    UPROPERTY(BlueprintAssignable, Category = "Health Events")
    FOnDeath OnDeath;

    UPROPERTY(BlueprintAssignable, Category = "Health Events")
    FOnDamageTaken OnDamageTaken;

    // Health Functions
    UFUNCTION(BlueprintCallable, Category = "Health")
    void TakeDamage(const FDamageInfo& DamageInfo);

    UFUNCTION(BlueprintCallable, Category = "Health")
    void Heal(float HealAmount);

    UFUNCTION(BlueprintCallable, Category = "Health")
    void AddArmor(float ArmorAmount);

    UFUNCTION(BlueprintCallable, Category = "Health")
    void SetMaxHealth(float NewMaxHealth);

    UFUNCTION(BlueprintCallable, Category = "Health")
    void SetInvulnerable(bool bInvulnerable);

    UFUNCTION(BlueprintCallable, Category = "Health")
    void Revive(float ReviveHealth = 50.0f);

    // Getters
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Health")
    float GetCurrentHealth() const { return CurrentHealth; }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Health")
    float GetMaxHealth() const { return MaxHealth; }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Health")
    float GetHealthPercentage() const { return MaxHealth > 0 ? CurrentHealth / MaxHealth : 0.0f; }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Health")
    float GetCurrentArmor() const { return Armor; }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Health")
    float GetMaxArmor() const { return MaxArmor; }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Health")
    bool IsDead() const { return bIsDead; }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Health")
    bool IsAlive() const { return !bIsDead; }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Health")
    bool IsAtFullHealth() const { return CurrentHealth >= MaxHealth; }

    // Damage Calculation
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Damage")
    float CalculateDamageResistance(EDamageType DamageType) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Damage")
    float CalculateFinalDamage(const FDamageInfo& DamageInfo) const;

    // Blueprint Events
    UFUNCTION(BlueprintImplementableEvent, Category = "Health Events")
    void OnHealthChangedBP(float NewHealth, float MaxHealth);

    UFUNCTION(BlueprintImplementableEvent, Category = "Health Events")
    void OnDeathBP();

    UFUNCTION(BlueprintImplementableEvent, Category = "Health Events")
    void OnDamageTakenBP(float DamageAmount, AActor* DamageCauser);

    UFUNCTION(BlueprintImplementableEvent, Category = "Health Events")
    void OnHealedBP(float HealAmount);

    UFUNCTION(BlueprintImplementableEvent, Category = "Health Events")
    void OnArmorAddedBP(float ArmorAmount);

protected:
    // Timer Handles
    FTimerHandle RegenerationTimerHandle;
    FTimerHandle RegenerationDelayTimerHandle;

    // Internal Functions
    void StartRegeneration();
    void StopRegeneration();
    void RegenerateHealth();
    void Die();
    void UpdateHealthUI();

    // Server Functions
    UFUNCTION(Server, Reliable, Category = "Server")
    void Server_TakeDamage(const FDamageInfo& DamageInfo);

    UFUNCTION(Server, Reliable, Category = "Server")
    void Server_Heal(float HealAmount);

    UFUNCTION(Server, Reliable, Category = "Server")
    void Server_AddArmor(float ArmorAmount);

    UFUNCTION(Server, Reliable, Category = "Server")
    void Server_Revive(float ReviveHealth);

    // Multicast Functions
    UFUNCTION(NetMulticast, Reliable, Category = "Multicast")
    void Multicast_OnHealthChanged(float NewHealth, float NewMaxHealth);

    UFUNCTION(NetMulticast, Reliable, Category = "Multicast")
    void Multicast_OnDeath();

    UFUNCTION(NetMulticast, Reliable, Category = "Multicast")
    void Multicast_OnDamageTaken(float DamageAmount, AActor* DamageCauser, AActor* DamageInstigator);

    UFUNCTION(NetMulticast, Reliable, Category = "Multicast")
    void Multicast_OnHealed(float HealAmount);

    UFUNCTION(NetMulticast, Reliable, Category = "Multicast")
    void Multicast_OnArmorAdded(float ArmorAmount);
};