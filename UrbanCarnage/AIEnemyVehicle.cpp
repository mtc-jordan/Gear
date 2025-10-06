// Copyright Epic Games, Inc. All Rights Reserved.

#include "AIEnemyVehicle.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Components/HealthComponent.h"
#include "DrawDebugHelpers.h"

AAIEnemyVehicle::AAIEnemyVehicle()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
}

void AAIEnemyVehicle::BeginPlay()
{
    Super::BeginPlay();
    
    if (HasAuthority())
    {
        InitializeAI();
    }
}

void AAIEnemyVehicle::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (HasAuthority())
    {
        UpdateAI(DeltaTime);
    }
}

void AAIEnemyVehicle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(AAIEnemyVehicle, CurrentBehaviorState);
    DOREPLIFETIME(AAIEnemyVehicle, CurrentTarget);
    DOREPLIFETIME(AAIEnemyVehicle, LastKnownTargetLocation);
    DOREPLIFETIME(AAIEnemyVehicle, CurrentPatrolIndex);
    DOREPLIFETIME(AAIEnemyVehicle, PatrolCenter);
}

void AAIEnemyVehicle::SetBehaviorState(EAIBehaviorState NewState)
{
    if (HasAuthority())
    {
        Server_SetBehaviorState(NewState);
    }
}

void AAIEnemyVehicle::SetTarget(AActor* NewTarget)
{
    if (HasAuthority())
    {
        Server_SetTarget(NewTarget);
    }
}

void AAIEnemyVehicle::ClearTarget()
{
    SetTarget(nullptr);
}

void AAIEnemyVehicle::SetPatrolPoints(const TArray<FVector>& NewPatrolPoints)
{
    PatrolPoints = NewPatrolPoints;
    CurrentPatrolIndex = 0;
}

void AAIEnemyVehicle::SetPatrolCenter(const FVector& Center, float Radius)
{
    PatrolCenter = Center;
    BehaviorSettings.PatrolRadius = Radius;
    bUseRandomPatrol = true;
}

void AAIEnemyVehicle::UpdateAI(float DeltaTime)
{
    if (bIsDead)
    {
        return;
    }

    UpdateTargetDetection();
    UpdateBehaviorState();
    UpdateMovement();
    UpdateWeaponTargeting();
}

void AAIEnemyVehicle::UpdateBehaviorState()
{
    EAIBehaviorState OldState = CurrentBehaviorState;
    EAIBehaviorState NewState = CurrentBehaviorState;

    // Check if we have a valid target
    bool bHasValidTarget = IsTargetValid(CurrentTarget);
    
    if (bHasValidTarget)
    {
        float DistanceToTarget = GetDistanceToTarget(CurrentTarget);
        
        // Check if we should flee
        if (BehaviorSettings.bCanFlee && CurrentBehaviorState != EAIBehaviorState::Flee)
        {
            UHealthComponent* HealthComp = GetComponentByClass<UHealthComponent>();
            if (HealthComp && HealthComp->GetHealthPercentage() <= BehaviorSettings.FleeHealthThreshold)
            {
                NewState = EAIBehaviorState::Flee;
            }
        }
        
        // Determine behavior based on distance and current state
        if (NewState != EAIBehaviorState::Flee)
        {
            if (DistanceToTarget <= BehaviorSettings.AttackRange)
            {
                NewState = EAIBehaviorState::Attack;
            }
            else if (DistanceToTarget <= BehaviorSettings.DetectionRange)
            {
                NewState = EAIBehaviorState::Chase;
            }
            else
            {
                NewState = EAIBehaviorState::Search;
            }
        }
    }
    else
    {
        // No valid target, go back to patrol or idle
        if (PatrolPoints.Num() > 0 || bUseRandomPatrol)
        {
            NewState = EAIBehaviorState::Patrol;
        }
        else
        {
            NewState = EAIBehaviorState::Idle;
        }
    }

    // Update state if changed
    if (NewState != CurrentBehaviorState)
    {
        SetBehaviorState(NewState);
    }
}

void AAIEnemyVehicle::UpdateMovement()
{
    switch (CurrentBehaviorState)
    {
        case EAIBehaviorState::Patrol:
            UpdatePatrol();
            break;
        case EAIBehaviorState::Chase:
            UpdateChase();
            break;
        case EAIBehaviorState::Attack:
            UpdateAttack();
            break;
        case EAIBehaviorState::Flee:
            UpdateFlee();
            break;
        case EAIBehaviorState::Search:
            UpdateSearch();
            break;
        default:
            StopMovement();
            break;
    }
}

void AAIEnemyVehicle::UpdateWeaponTargeting()
{
    if (CurrentTarget && ShouldFire())
    {
        FVector TargetLocation = PredictTargetLocation(CurrentTarget);
        FireAtLocation(TargetLocation);
    }
}

void AAIEnemyVehicle::UpdatePatrol()
{
    if (bIsWaitingAtPatrolPoint)
    {
        return;
    }

    FVector NextPoint = GetNextPatrolPoint();
    if (HasReachedDestination(NextPoint))
    {
        bIsWaitingAtPatrolPoint = true;
        GetWorld()->GetTimerManager().SetTimer(PatrolWaitTimerHandle, this, &AAIEnemyVehicle::OnPatrolWaitComplete, PatrolWaitTime, false);
        OnPatrolPointReached(CurrentPatrolIndex);
    }
    else
    {
        MoveToLocation(NextPoint);
    }
}

void AAIEnemyVehicle::UpdateChase()
{
    if (CurrentTarget)
    {
        MoveToTarget(CurrentTarget);
        LastKnownTargetLocation = CurrentTarget->GetActorLocation();
    }
}

void AAIEnemyVehicle::UpdateAttack()
{
    if (CurrentTarget)
    {
        // Move to maintain attack range
        float DistanceToTarget = GetDistanceToTarget(CurrentTarget);
        if (DistanceToTarget > BehaviorSettings.AttackRange * 0.8f)
        {
            MoveToTarget(CurrentTarget);
        }
        else
        {
            // Circle around target
            FVector DirectionToTarget = (CurrentTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
            FVector RightVector = FVector::CrossProduct(DirectionToTarget, FVector::UpVector);
            FVector CirclePosition = CurrentTarget->GetActorLocation() + RightVector * BehaviorSettings.AttackRange * 0.5f;
            MoveToLocation(CirclePosition);
        }
    }
}

void AAIEnemyVehicle::UpdateFlee()
{
    if (CurrentTarget)
    {
        FVector DirectionAwayFromTarget = (GetActorLocation() - CurrentTarget->GetActorLocation()).GetSafeNormal();
        FVector FleeLocation = GetActorLocation() + DirectionAwayFromTarget * BehaviorSettings.DetectionRange * 2.0f;
        MoveToLocation(FleeLocation);
    }
}

void AAIEnemyVehicle::UpdateSearch()
{
    if (LastKnownTargetLocation != FVector::ZeroVector)
    {
        MoveToLocation(LastKnownTargetLocation);
        
        if (HasReachedDestination(LastKnownTargetLocation))
        {
            // Start searching in a pattern
            GetWorld()->GetTimerManager().SetTimer(SearchTimerHandle, this, &AAIEnemyVehicle::OnSearchTimerComplete, 2.0f, false);
        }
    }
    else
    {
        UpdatePatrol();
    }
}

AActor* AAIEnemyVehicle::FindNearestTarget()
{
    TArray<AActor*> AllPlayers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController::StaticClass(), AllPlayers);
    
    AActor* NearestTarget = nullptr;
    float NearestDistance = FLT_MAX;
    
    for (AActor* Player : AllPlayers)
    {
        if (IsTargetValid(Player))
        {
            float Distance = GetDistanceToTarget(Player);
            if (Distance < NearestDistance && Distance <= BehaviorSettings.DetectionRange)
            {
                NearestDistance = Distance;
                NearestTarget = Player;
            }
        }
    }
    
    return NearestTarget;
}

bool AAIEnemyVehicle::CanSeeTarget(AActor* Target) const
{
    if (!Target)
    {
        return false;
    }

    FVector Start = GetActorLocation();
    FVector End = Target->GetActorLocation();
    FHitResult HitResult;
    
    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        Start,
        End,
        ECollisionChannel::ECC_Visibility
    );
    
    return !bHit || HitResult.GetActor() == Target;
}

bool AAIEnemyVehicle::IsTargetInRange(AActor* Target, float Range) const
{
    if (!Target)
    {
        return false;
    }
    
    return GetDistanceToTarget(Target) <= Range;
}

float AAIEnemyVehicle::GetDistanceToTarget(AActor* Target) const
{
    if (!Target)
    {
        return FLT_MAX;
    }
    
    return FVector::Dist(GetActorLocation(), Target->GetActorLocation());
}

void AAIEnemyVehicle::MoveToLocation(const FVector& TargetLocation)
{
    FVector Direction = (TargetLocation - GetActorLocation()).GetSafeNormal();
    FVector RightVector = FVector::CrossProduct(Direction, FVector::UpVector);
    
    // Calculate steering input
    float SteeringInput = FVector::DotProduct(GetActorRightVector(), Direction);
    
    // Calculate throttle input
    float ThrottleInput = FVector::DotProduct(GetActorForwardVector(), Direction);
    
    // Apply movement
    if (ChaosVehicleMovement)
    {
        ChaosVehicleMovement->SetThrottleInput(ThrottleInput);
        ChaosVehicleMovement->SetSteeringInput(SteeringInput);
    }
}

void AAIEnemyVehicle::MoveToTarget(AActor* Target)
{
    if (Target)
    {
        MoveToLocation(Target->GetActorLocation());
    }
}

void AAIEnemyVehicle::StopMovement()
{
    if (ChaosVehicleMovement)
    {
        ChaosVehicleMovement->SetThrottleInput(0.0f);
        ChaosVehicleMovement->SetSteeringInput(0.0f);
        ChaosVehicleMovement->SetBrakeInput(1.0f);
    }
}

FVector AAIEnemyVehicle::GetNextPatrolPoint()
{
    if (bUseRandomPatrol)
    {
        return GetRandomPatrolPoint();
    }
    else if (PatrolPoints.Num() > 0)
    {
        FVector Point = PatrolPoints[CurrentPatrolIndex];
        CurrentPatrolIndex = (CurrentPatrolIndex + 1) % PatrolPoints.Num();
        return Point;
    }
    
    return GetActorLocation();
}

bool AAIEnemyVehicle::HasReachedDestination(const FVector& Destination, float Tolerance) const
{
    return FVector::Dist(GetActorLocation(), Destination) <= Tolerance;
}

void AAIEnemyVehicle::FireAtTarget(AActor* Target)
{
    if (Target)
    {
        FVector TargetLocation = PredictTargetLocation(Target);
        FireAtLocation(TargetLocation);
    }
}

void AAIEnemyVehicle::FireAtLocation(const FVector& TargetLocation)
{
    // Calculate aim direction
    FVector AimDirection = (TargetLocation - GetActorLocation()).GetSafeNormal();
    FRotator AimRotation = AimDirection.Rotation();
    
    // Apply accuracy modifier
    float AccuracyModifier = BehaviorSettings.Accuracy;
    if (Difficulty == EAIDifficulty::Easy)
    {
        AccuracyModifier *= 0.7f;
    }
    else if (Difficulty == EAIDifficulty::Hard)
    {
        AccuracyModifier *= 1.2f;
    }
    else if (Difficulty == EAIDifficulty::Expert)
    {
        AccuracyModifier *= 1.5f;
    }
    
    // Add some randomness based on accuracy
    float RandomYaw = FMath::RandRange(-(1.0f - AccuracyModifier) * 30.0f, (1.0f - AccuracyModifier) * 30.0f);
    float RandomPitch = FMath::RandRange(-(1.0f - AccuracyModifier) * 15.0f, (1.0f - AccuracyModifier) * 15.0f);
    
    AimRotation.Yaw += RandomYaw;
    AimRotation.Pitch += RandomPitch;
    
    // Set aim point and fire
    AimPoint = GetActorLocation() + AimRotation.Vector() * 1000.0f;
    Server_SetAimLocation(AimPoint);
    
    if (PrimaryWeapon_Ref)
    {
        PrimaryWeapon_Ref->Aim(AimPoint);
        PrimaryWeapon_Ref->Shoot();
    }
}

bool AAIEnemyVehicle::ShouldFire() const
{
    if (!CurrentTarget || !PrimaryWeapon_Ref)
    {
        return false;
    }
    
    float DistanceToTarget = GetDistanceToTarget(CurrentTarget);
    return DistanceToTarget <= BehaviorSettings.AttackRange && CanSeeTarget(CurrentTarget);
}

FVector AAIEnemyVehicle::PredictTargetLocation(AActor* Target) const
{
    if (!Target)
    {
        return FVector::ZeroVector;
    }
    
    // Simple prediction based on target velocity
    FVector TargetLocation = Target->GetActorLocation();
    FVector TargetVelocity = Target->GetVelocity();
    
    // Predict where target will be in 0.5 seconds
    float PredictionTime = 0.5f;
    FVector PredictedLocation = TargetLocation + TargetVelocity * PredictionTime;
    
    return PredictedLocation;
}

FVector AAIEnemyVehicle::GetRandomPatrolPoint() const
{
    if (PatrolCenter == FVector::ZeroVector)
    {
        return GetActorLocation();
    }
    
    float RandomAngle = FMath::RandRange(0.0f, 360.0f);
    float RandomDistance = FMath::RandRange(0.0f, BehaviorSettings.PatrolRadius);
    
    FVector RandomDirection = FVector(FMath::Cos(FMath::DegreesToRadians(RandomAngle)), FMath::Sin(FMath::DegreesToRadians(RandomAngle)), 0.0f);
    return PatrolCenter + RandomDirection * RandomDistance;
}

bool AAIEnemyVehicle::IsTargetValid(AActor* Target) const
{
    if (!Target)
    {
        return false;
    }
    
    // Check if target is alive
    UHealthComponent* HealthComp = Target->GetComponentByClass<UHealthComponent>();
    if (HealthComp && HealthComp->IsDead())
    {
        return false;
    }
    
    // Check if target is a player
    APlayerController* PlayerController = Cast<APlayerController>(Target);
    if (!PlayerController)
    {
        return false;
    }
    
    return true;
}

void AAIEnemyVehicle::UpdateDifficultySettings()
{
    switch (Difficulty)
    {
        case EAIDifficulty::Easy:
            BehaviorSettings.DetectionRange *= 0.7f;
            BehaviorSettings.Accuracy *= 0.6f;
            BehaviorSettings.ReactionTime *= 1.5f;
            break;
        case EAIDifficulty::Medium:
            // Use default settings
            break;
        case EAIDifficulty::Hard:
            BehaviorSettings.DetectionRange *= 1.3f;
            BehaviorSettings.Accuracy *= 1.2f;
            BehaviorSettings.ReactionTime *= 0.7f;
            break;
        case EAIDifficulty::Expert:
            BehaviorSettings.DetectionRange *= 1.5f;
            BehaviorSettings.Accuracy *= 1.4f;
            BehaviorSettings.ReactionTime *= 0.5f;
            BehaviorSettings.bCanUseSecondaryWeapons = true;
            break;
    }
}

void AAIEnemyVehicle::InitializeAI()
{
    UpdateDifficultySettings();
    
    if (bUseRandomPatrol && PatrolCenter == FVector::ZeroVector)
    {
        PatrolCenter = GetActorLocation();
    }
    
    SetBehaviorState(EAIBehaviorState::Patrol);
}

void AAIEnemyVehicle::UpdateTargetDetection()
{
    if (CurrentBehaviorState == EAIBehaviorState::Flee)
    {
        return;
    }
    
    AActor* NearestTarget = FindNearestTarget();
    
    if (NearestTarget != CurrentTarget)
    {
        if (NearestTarget)
        {
            SetTarget(NearestTarget);
        }
        else if (CurrentTarget)
        {
            ClearTarget();
        }
    }
}

void AAIEnemyVehicle::CalculateMovementInput()
{
    // This is handled in UpdateMovement
}

void AAIEnemyVehicle::CalculateWeaponInput()
{
    // This is handled in UpdateWeaponTargeting
}

void AAIEnemyVehicle::OnReactionTimerComplete()
{
    // Handle reaction delay
}

void AAIEnemyVehicle::OnPatrolWaitComplete()
{
    bIsWaitingAtPatrolPoint = false;
}

void AAIEnemyVehicle::OnSearchTimerComplete()
{
    // Stop searching and go back to patrol
    if (CurrentBehaviorState == EAIBehaviorState::Search)
    {
        SetBehaviorState(EAIBehaviorState::Patrol);
    }
}

// Server Functions
void AAIEnemyVehicle::Server_SetBehaviorState_Implementation(EAIBehaviorState NewState)
{
    EAIBehaviorState OldState = CurrentBehaviorState;
    CurrentBehaviorState = NewState;
    Multicast_OnBehaviorStateChanged(NewState, OldState);
}

void AAIEnemyVehicle::Server_SetTarget_Implementation(AActor* NewTarget)
{
    AActor* OldTarget = CurrentTarget;
    CurrentTarget = NewTarget;
    
    if (NewTarget && NewTarget != OldTarget)
    {
        Multicast_OnTargetAcquired(NewTarget);
    }
    else if (!NewTarget && OldTarget)
    {
        Multicast_OnTargetLost();
    }
}

void AAIEnemyVehicle::Server_UpdateAI_Implementation(float DeltaTime)
{
    UpdateAI(DeltaTime);
}

// Multicast Functions
void AAIEnemyVehicle::Multicast_OnBehaviorStateChanged_Implementation(EAIBehaviorState NewState, EAIBehaviorState OldState)
{
    OnBehaviorStateChanged(NewState, OldState);
}

void AAIEnemyVehicle::Multicast_OnTargetAcquired_Implementation(AActor* NewTarget)
{
    OnTargetAcquired(NewTarget);
}

void AAIEnemyVehicle::Multicast_OnTargetLost_Implementation()
{
    OnTargetLost();
}