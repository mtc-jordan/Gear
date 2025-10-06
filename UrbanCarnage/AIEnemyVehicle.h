// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UrbanCarnagePawn.h"
#include "AIEnemyVehicle.generated.h"

UENUM(BlueprintType)
enum class EAIBehaviorState : uint8
{
    Idle        UMETA(DisplayName = "Idle"),
    Patrol      UMETA(DisplayName = "Patrol"),
    Chase       UMETA(DisplayName = "Chase"),
    Attack      UMETA(DisplayName = "Attack"),
    Flee        UMETA(DisplayName = "Flee"),
    Search      UMETA(DisplayName = "Search")
};

UENUM(BlueprintType)
enum class EAIDifficulty : uint8
{
    Easy        UMETA(DisplayName = "Easy"),
    Medium      UMETA(DisplayName = "Medium"),
    Hard        UMETA(DisplayName = "Hard"),
    Expert      UMETA(DisplayName = "Expert")
};

USTRUCT(BlueprintType)
struct FAIBehaviorSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Behavior")
    float DetectionRange = 2000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Behavior")
    float AttackRange = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Behavior")
    float PatrolRadius = 5000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Behavior")
    float MaxSpeed = 800.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Behavior")
    float ReactionTime = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Behavior")
    float Accuracy = 0.7f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Behavior")
    bool bCanUseSecondaryWeapons = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Behavior")
    bool bCanFlee = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Behavior")
    float FleeHealthThreshold = 0.3f;

    FAIBehaviorSettings()
    {
        DetectionRange = 2000.0f;
        AttackRange = 1000.0f;
        PatrolRadius = 5000.0f;
        MaxSpeed = 800.0f;
        ReactionTime = 0.5f;
        Accuracy = 0.7f;
        bCanUseSecondaryWeapons = true;
        bCanFlee = true;
        FleeHealthThreshold = 0.3f;
    }
};

UCLASS()
class URBANCARNAGE_API AAIEnemyVehicle : public AUrbanCarnagePawn
{
    GENERATED_BODY()

public:
    AAIEnemyVehicle();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
    // AI Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
    EAIDifficulty Difficulty = EAIDifficulty::Medium;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
    FAIBehaviorSettings BehaviorSettings;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
    TArray<FVector> PatrolPoints;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
    bool bUseRandomPatrol = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
    float PatrolWaitTime = 3.0f;

    // AI State
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "AI State")
    EAIBehaviorState CurrentBehaviorState = EAIBehaviorState::Idle;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "AI State")
    AActor* CurrentTarget = nullptr;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "AI State")
    FVector LastKnownTargetLocation = FVector::ZeroVector;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "AI State")
    int32 CurrentPatrolIndex = 0;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "AI State")
    FVector PatrolCenter = FVector::ZeroVector;

    // AI Functions
    UFUNCTION(BlueprintCallable, Category = "AI")
    void SetBehaviorState(EAIBehaviorState NewState);

    UFUNCTION(BlueprintCallable, Category = "AI")
    void SetTarget(AActor* NewTarget);

    UFUNCTION(BlueprintCallable, Category = "AI")
    void ClearTarget();

    UFUNCTION(BlueprintCallable, Category = "AI")
    void SetPatrolPoints(const TArray<FVector>& NewPatrolPoints);

    UFUNCTION(BlueprintCallable, Category = "AI")
    void SetPatrolCenter(const FVector& Center, float Radius);

    // AI Behavior Functions
    UFUNCTION(BlueprintCallable, Category = "AI Behavior")
    void UpdateAI(float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "AI Behavior")
    void UpdateBehaviorState();

    UFUNCTION(BlueprintCallable, Category = "AI Behavior")
    void UpdateMovement();

    UFUNCTION(BlueprintCallable, Category = "AI Behavior")
    void UpdateWeaponTargeting();

    UFUNCTION(BlueprintCallable, Category = "AI Behavior")
    void UpdatePatrol();

    UFUNCTION(BlueprintCallable, Category = "AI Behavior")
    void UpdateChase();

    UFUNCTION(BlueprintCallable, Category = "AI Behavior")
    void UpdateAttack();

    UFUNCTION(BlueprintCallable, Category = "AI Behavior")
    void UpdateFlee();

    UFUNCTION(BlueprintCallable, Category = "AI Behavior")
    void UpdateSearch();

    // Target Detection
    UFUNCTION(BlueprintCallable, Category = "AI Detection")
    AActor* FindNearestTarget();

    UFUNCTION(BlueprintCallable, Category = "AI Detection")
    bool CanSeeTarget(AActor* Target) const;

    UFUNCTION(BlueprintCallable, Category = "AI Detection")
    bool IsTargetInRange(AActor* Target, float Range) const;

    UFUNCTION(BlueprintCallable, Category = "AI Detection")
    float GetDistanceToTarget(AActor* Target) const;

    // Movement Functions
    UFUNCTION(BlueprintCallable, Category = "AI Movement")
    void MoveToLocation(const FVector& TargetLocation);

    UFUNCTION(BlueprintCallable, Category = "AI Movement")
    void MoveToTarget(AActor* Target);

    UFUNCTION(BlueprintCallable, Category = "AI Movement")
    void StopMovement();

    UFUNCTION(BlueprintCallable, Category = "AI Movement")
    FVector GetNextPatrolPoint();

    UFUNCTION(BlueprintCallable, Category = "AI Movement")
    bool HasReachedDestination(const FVector& Destination, float Tolerance = 100.0f) const;

    // Weapon Functions
    UFUNCTION(BlueprintCallable, Category = "AI Weapons")
    void FireAtTarget(AActor* Target);

    UFUNCTION(BlueprintCallable, Category = "AI Weapons")
    void FireAtLocation(const FVector& TargetLocation);

    UFUNCTION(BlueprintCallable, Category = "AI Weapons")
    bool ShouldFire() const;

    UFUNCTION(BlueprintCallable, Category = "AI Weapons")
    FVector PredictTargetLocation(AActor* Target) const;

    // Utility Functions
    UFUNCTION(BlueprintCallable, Category = "AI Utility")
    FVector GetRandomPatrolPoint() const;

    UFUNCTION(BlueprintCallable, Category = "AI Utility")
    bool IsTargetValid(AActor* Target) const;

    UFUNCTION(BlueprintCallable, Category = "AI Utility")
    void UpdateDifficultySettings();

    // Getters
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI")
    EAIBehaviorState GetCurrentBehaviorState() const { return CurrentBehaviorState; }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI")
    AActor* GetCurrentTarget() const { return CurrentTarget; }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI")
    bool HasTarget() const { return CurrentTarget != nullptr; }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI")
    bool IsInCombat() const { return CurrentBehaviorState == EAIBehaviorState::Chase || CurrentBehaviorState == EAIBehaviorState::Attack; }

    // Blueprint Events
    UFUNCTION(BlueprintImplementableEvent, Category = "AI Events")
    void OnBehaviorStateChanged(EAIBehaviorState NewState, EAIBehaviorState OldState);

    UFUNCTION(BlueprintImplementableEvent, Category = "AI Events")
    void OnTargetAcquired(AActor* NewTarget);

    UFUNCTION(BlueprintImplementableEvent, Category = "AI Events")
    void OnTargetLost();

    UFUNCTION(BlueprintImplementableEvent, Category = "AI Events")
    void OnPatrolPointReached(int32 PointIndex);

    UFUNCTION(BlueprintImplementableEvent, Category = "AI Events")
    void OnAttackStarted(AActor* Target);

    UFUNCTION(BlueprintImplementableEvent, Category = "AI Events")
    void OnFleeStarted();

protected:
    // Timer Handles
    FTimerHandle ReactionTimerHandle;
    FTimerHandle PatrolWaitTimerHandle;
    FTimerHandle SearchTimerHandle;

    // Internal Variables
    FVector LastKnownLocation = FVector::ZeroVector;
    float LastTargetUpdateTime = 0.0f;
    float LastAttackTime = 0.0f;
    bool bIsWaitingAtPatrolPoint = false;

    // Internal Functions
    void InitializeAI();
    void UpdateTargetDetection();
    void CalculateMovementInput();
    void CalculateWeaponInput();
    void OnReactionTimerComplete();
    void OnPatrolWaitComplete();
    void OnSearchTimerComplete();

    // Server Functions
    UFUNCTION(Server, Reliable, Category = "Server")
    void Server_SetBehaviorState(EAIBehaviorState NewState);

    UFUNCTION(Server, Reliable, Category = "Server")
    void Server_SetTarget(AActor* NewTarget);

    UFUNCTION(Server, Reliable, Category = "Server")
    void Server_UpdateAI(float DeltaTime);

    // Multicast Functions
    UFUNCTION(NetMulticast, Reliable, Category = "Multicast")
    void Multicast_OnBehaviorStateChanged(EAIBehaviorState NewState, EAIBehaviorState OldState);

    UFUNCTION(NetMulticast, Reliable, Category = "Multicast")
    void Multicast_OnTargetAcquired(AActor* NewTarget);

    UFUNCTION(NetMulticast, Reliable, Category = "Multicast")
    void Multicast_OnTargetLost();
};