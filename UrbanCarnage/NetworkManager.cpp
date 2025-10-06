// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetworkManager.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

const float ANetworkManager::MAX_MOVEMENT_SPEED = 2000.0f; // cm/s
const float ANetworkManager::MAX_ROTATION_SPEED = 720.0f; // degrees/s
const float ANetworkManager::MAX_WEAPON_FIRE_RATE = 10.0f; // shots per second
const int32 ANetworkManager::MAX_SUSPICIOUS_ACTIONS = 5;
const float ANetworkManager::VALIDATION_TOLERANCE = 0.1f;

ANetworkManager::ANetworkManager()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
}

void ANetworkManager::BeginPlay()
{
    Super::BeginPlay();
    
    InitializeNetworkManager();
}

void ANetworkManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    UpdateClientConnections();
    ValidateAllClients();
    OptimizeBandwidth();
}

void ANetworkManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(ANetworkManager, ConnectedClients);
    DOREPLIFETIME(ANetworkManager, MaxClients);
    DOREPLIFETIME(ANetworkManager, CurrentClients);
}

void ANetworkManager::RegisterClient(const FString& PlayerID, const FString& PlayerName, const FString& IPAddress)
{
    if (HasAuthority())
    {
        Server_RegisterClient(PlayerID, PlayerName, IPAddress);
    }
}

void ANetworkManager::UnregisterClient(const FString& PlayerID)
{
    if (HasAuthority())
    {
        Server_UnregisterClient(PlayerID);
    }
}

void ANetworkManager::UpdateClientNetworkStats(const FString& PlayerID, const FNetworkStats& Stats)
{
    if (HasAuthority())
    {
        Server_UpdateNetworkStats(PlayerID, Stats);
    }
}

bool ANetworkManager::IsClientValid(const FString& PlayerID) const
{
    if (const FClientInfo* ClientInfo = ConnectedClients.Find(PlayerID))
    {
        return ClientInfo->bIsValidated && ClientInfo->SuspiciousActions < MAX_SUSPICIOUS_ACTIONS;
    }
    return false;
}

FClientInfo ANetworkManager::GetClientInfo(const FString& PlayerID) const
{
    if (const FClientInfo* ClientInfo = ConnectedClients.Find(PlayerID))
    {
        return *ClientInfo;
    }
    return FClientInfo();
}

bool ANetworkManager::ValidatePlayerAction(const FString& PlayerID, const FString& ActionType, const FVector& Location, const FRotator& Rotation)
{
    if (!bEnableAntiCheat)
    {
        return true;
    }
    
    // Check if player is banned
    if (BannedPlayers.Contains(PlayerID))
    {
        return false;
    }
    
    // Check if player is validated
    if (!IsClientValid(PlayerID))
    {
        return false;
    }
    
    // Validate action based on type
    if (ActionType == "Movement")
    {
        return ValidatePlayerMovement(PlayerID, Location, FVector::ZeroVector, 0.0f);
    }
    else if (ActionType == "Weapon")
    {
        return ValidatePlayerWeapon(PlayerID, "Unknown", 1.0f);
    }
    
    return true;
}

bool ANetworkManager::ValidatePlayerMovement(const FString& PlayerID, const FVector& NewLocation, const FVector& OldLocation, float DeltaTime)
{
    if (!bEnableAntiCheat)
    {
        return true;
    }
    
    // Calculate movement speed
    float MovementDistance = FVector::Dist(NewLocation, OldLocation);
    float MovementSpeed = DeltaTime > 0.0f ? MovementDistance / DeltaTime : 0.0f;
    
    // Check if movement speed is within reasonable limits
    if (MovementSpeed > MAX_MOVEMENT_SPEED)
    {
        ReportSuspiciousActivity(PlayerID, "SpeedHack", FString::Printf(TEXT("Speed: %.2f cm/s"), MovementSpeed));
        return false;
    }
    
    // Check for teleportation (instant movement over large distance)
    if (MovementDistance > 1000.0f && DeltaTime < 0.1f)
    {
        ReportSuspiciousActivity(PlayerID, "TeleportHack", FString::Printf(TEXT("Distance: %.2f cm in %.3f s"), MovementDistance, DeltaTime));
        return false;
    }
    
    // Store position history for further validation
    if (TArray<FVector>* PositionHistory = PlayerPositionHistory.Find(PlayerID))
    {
        PositionHistory->Add(NewLocation);
        
        // Keep only last 10 positions
        if (PositionHistory->Num() > 10)
        {
            PositionHistory->RemoveAt(0);
        }
    }
    else
    {
        TArray<FVector> NewHistory;
        NewHistory.Add(NewLocation);
        PlayerPositionHistory.Add(PlayerID, NewHistory);
    }
    
    return true;
}

bool ANetworkManager::ValidatePlayerWeapon(const FString& PlayerID, const FString& WeaponType, float FireRate)
{
    if (!bEnableAntiCheat)
    {
        return true;
    }
    
    // Check if fire rate is within reasonable limits
    if (FireRate > MAX_WEAPON_FIRE_RATE)
    {
        ReportSuspiciousActivity(PlayerID, "RapidFireHack", FString::Printf(TEXT("Fire Rate: %.2f shots/s"), FireRate));
        return false;
    }
    
    return true;
}

void ANetworkManager::ReportSuspiciousActivity(const FString& PlayerID, const FString& ActivityType, const FString& Details)
{
    if (HasAuthority())
    {
        Server_ReportSuspiciousActivity(PlayerID, ActivityType, Details);
    }
}

void ANetworkManager::KickPlayer(const FString& PlayerID, const FString& Reason)
{
    if (HasAuthority())
    {
        Server_KickPlayer(PlayerID, Reason);
    }
}

void ANetworkManager::BanPlayer(const FString& PlayerID, const FString& Reason, int32 DurationMinutes)
{
    if (HasAuthority())
    {
        Server_BanPlayer(PlayerID, Reason, DurationMinutes);
    }
}

FVector ANetworkManager::CompensatePosition(const FVector& Position, float ClientPing)
{
    if (!bEnableLagCompensation)
    {
        return Position;
    }
    
    // Simple lag compensation - move position back in time
    float CompensationTime = ClientPing * 0.001f; // Convert ms to seconds
    return Position; // This would need actual velocity data for proper compensation
}

FRotator ANetworkManager::CompensateRotation(const FRotator& Rotation, float ClientPing)
{
    if (!bEnableLagCompensation)
    {
        return Rotation;
    }
    
    // Simple lag compensation for rotation
    float CompensationTime = ClientPing * 0.001f;
    return Rotation; // This would need actual angular velocity data for proper compensation
}

bool ANetworkManager::IsHitValid(const FVector& HitLocation, const FVector& ShooterLocation, const FVector& TargetLocation, float ClientPing)
{
    if (!bEnableLagCompensation)
    {
        return true;
    }
    
    // Compensate for lag
    FVector CompensatedTargetLocation = CompensatePosition(TargetLocation, ClientPing);
    
    // Check if hit is within reasonable distance
    float HitDistance = FVector::Dist(HitLocation, CompensatedTargetLocation);
    return HitDistance < 200.0f; // 2 meter tolerance
}

void ANetworkManager::OptimizeReplication()
{
    if (!bEnableBandwidthOptimization)
    {
        return;
    }
    
    // Update replication rates based on network conditions
    for (const auto& ClientPair : ConnectedClients)
    {
        const FClientInfo& ClientInfo = ClientPair.Value;
        ENetworkQuality Quality = ClientInfo.NetworkStats.Quality;
        
        // Adjust replication rates based on network quality
        switch (Quality)
        {
            case ENetworkQuality::Poor:
                // Reduce replication rates for poor connections
                SetReplicationRate("Position", 10.0f);
                SetReplicationRate("Rotation", 10.0f);
                SetReplicationRate("Velocity", 15.0f);
                break;
                
            case ENetworkQuality::Fair:
                // Moderate replication rates
                SetReplicationRate("Position", 15.0f);
                SetReplicationRate("Rotation", 15.0f);
                SetReplicationRate("Velocity", 20.0f);
                break;
                
            case ENetworkQuality::Good:
                // Standard replication rates
                SetReplicationRate("Position", 20.0f);
                SetReplicationRate("Rotation", 20.0f);
                SetReplicationRate("Velocity", 30.0f);
                break;
                
            case ENetworkQuality::Excellent:
                // High replication rates for excellent connections
                SetReplicationRate("Position", 30.0f);
                SetReplicationRate("Rotation", 30.0f);
                SetReplicationRate("Velocity", 60.0f);
                break;
        }
    }
}

void ANetworkManager::SetReplicationRate(const FString& ComponentType, float Rate)
{
    if (ComponentType == "Position")
    {
        ReplicationSettings.PositionUpdateRate = Rate;
    }
    else if (ComponentType == "Rotation")
    {
        ReplicationSettings.RotationUpdateRate = Rate;
    }
    else if (ComponentType == "Velocity")
    {
        ReplicationSettings.VelocityUpdateRate = Rate;
    }
    else if (ComponentType == "Health")
    {
        ReplicationSettings.HealthUpdateRate = Rate;
    }
    else if (ComponentType == "Weapon")
    {
        ReplicationSettings.WeaponUpdateRate = Rate;
    }
    else if (ComponentType == "GameState")
    {
        ReplicationSettings.GameStateUpdateRate = Rate;
    }
}

void ANetworkManager::EnableDistanceCulling(bool bEnable)
{
    // Enable/disable distance-based replication culling
    // This would be implemented in the replication system
}

void ANetworkManager::SetMaxUpdateDistance(float Distance)
{
    ReplicationSettings.MaxUpdateDistance = Distance;
}

ENetworkQuality ANetworkManager::CalculateNetworkQuality(float Ping, float PacketLoss, float Jitter)
{
    // Calculate network quality based on ping, packet loss, and jitter
    float QualityScore = 100.0f;
    
    // Penalize high ping
    if (Ping > 100.0f)
    {
        QualityScore -= (Ping - 100.0f) * 0.5f;
    }
    
    // Penalize packet loss
    QualityScore -= PacketLoss * 1000.0f;
    
    // Penalize high jitter
    if (Jitter > 50.0f)
    {
        QualityScore -= (Jitter - 50.0f) * 0.2f;
    }
    
    // Clamp score
    QualityScore = FMath::Clamp(QualityScore, 0.0f, 100.0f);
    
    // Convert to quality enum
    if (QualityScore >= 80.0f)
    {
        return ENetworkQuality::Excellent;
    }
    else if (QualityScore >= 60.0f)
    {
        return ENetworkQuality::Good;
    }
    else if (QualityScore >= 40.0f)
    {
        return ENetworkQuality::Fair;
    }
    else
    {
        return ENetworkQuality::Poor;
    }
}

void ANetworkManager::UpdateNetworkQuality()
{
    // Update network quality for all clients
    for (auto& ClientPair : ConnectedClients)
    {
        FClientInfo& ClientInfo = ClientPair.Value;
        ENetworkQuality OldQuality = ClientInfo.NetworkStats.Quality;
        
        ClientInfo.NetworkStats.Quality = CalculateNetworkQuality(
            ClientInfo.NetworkStats.Ping,
            ClientInfo.NetworkStats.PacketLoss,
            ClientInfo.NetworkStats.Jitter
        );
        
        if (OldQuality != ClientInfo.NetworkStats.Quality)
        {
            OnNetworkQualityChanged(ClientInfo.NetworkStats.Quality);
        }
    }
}

FNetworkStats ANetworkManager::GetNetworkStats() const
{
    return CurrentNetworkStats;
}

// Server Functions
void ANetworkManager::Server_RegisterClient_Implementation(const FString& PlayerID, const FString& PlayerName, const FString& IPAddress)
{
    if (CurrentClients >= MaxClients)
    {
        return; // Server full
    }
    
    FClientInfo NewClient;
    NewClient.PlayerID = PlayerID;
    NewClient.PlayerName = PlayerName;
    NewClient.IPAddress = IPAddress;
    NewClient.ConnectionTime = GetWorld()->GetTimeSeconds();
    NewClient.bIsValidated = true; // For now, auto-validate
    
    ConnectedClients.Add(PlayerID, NewClient);
    CurrentClients++;
    
    Multicast_ClientJoined(PlayerID, PlayerName);
    OnClientJoined(PlayerID, PlayerName);
}

void ANetworkManager::Server_UnregisterClient_Implementation(const FString& PlayerID)
{
    if (FClientInfo* ClientInfo = ConnectedClients.Find(PlayerID))
    {
        FString PlayerName = ClientInfo->PlayerName;
        ConnectedClients.Remove(PlayerID);
        CurrentClients--;
        
        Multicast_ClientLeft(PlayerID, PlayerName);
        OnClientLeft(PlayerID, PlayerName);
    }
}

void ANetworkManager::Server_UpdateNetworkStats_Implementation(const FString& PlayerID, const FNetworkStats& Stats)
{
    if (FClientInfo* ClientInfo = ConnectedClients.Find(PlayerID))
    {
        ClientInfo->NetworkStats = Stats;
    }
}

void ANetworkManager::Server_ReportSuspiciousActivity_Implementation(const FString& PlayerID, const FString& ActivityType, const FString& Details)
{
    if (FClientInfo* ClientInfo = ConnectedClients.Find(PlayerID))
    {
        ClientInfo->SuspiciousActions++;
        
        OnSuspiciousActivity(PlayerID, ActivityType, Details);
        
        // Kick player if too many suspicious actions
        if (ClientInfo->SuspiciousActions >= MAX_SUSPICIOUS_ACTIONS)
        {
            KickPlayer(PlayerID, "Too many suspicious activities");
        }
    }
}

void ANetworkManager::Server_KickPlayer_Implementation(const FString& PlayerID, const FString& Reason)
{
    if (FClientInfo* ClientInfo = ConnectedClients.Find(PlayerID))
    {
        FString PlayerName = ClientInfo->PlayerName;
        
        // Remove from connected clients
        ConnectedClients.Remove(PlayerID);
        CurrentClients--;
        
        Multicast_PlayerKicked(PlayerID, Reason);
        OnPlayerKicked(PlayerID, Reason);
        
        // TODO: Actually kick the player from the server
    }
}

void ANetworkManager::Server_BanPlayer_Implementation(const FString& PlayerID, const FString& Reason, int32 DurationMinutes)
{
    if (FClientInfo* ClientInfo = ConnectedClients.Find(PlayerID))
    {
        FString PlayerName = ClientInfo->PlayerName;
        
        // Add to banned list
        BannedPlayers.Add(PlayerID);
        
        // Remove from connected clients
        ConnectedClients.Remove(PlayerID);
        CurrentClients--;
        
        Multicast_PlayerBanned(PlayerID, Reason, DurationMinutes);
        OnPlayerBanned(PlayerID, Reason, DurationMinutes);
        
        // TODO: Actually ban the player from the server
    }
}

// Multicast Functions
void ANetworkManager::Multicast_ClientJoined_Implementation(const FString& PlayerID, const FString& PlayerName)
{
    OnClientJoined(PlayerID, PlayerName);
}

void ANetworkManager::Multicast_ClientLeft_Implementation(const FString& PlayerID, const FString& PlayerName)
{
    OnClientLeft(PlayerID, PlayerName);
}

void ANetworkManager::Multicast_PlayerKicked_Implementation(const FString& PlayerID, const FString& Reason)
{
    OnPlayerKicked(PlayerID, Reason);
}

void ANetworkManager::Multicast_PlayerBanned_Implementation(const FString& PlayerID, const FString& Reason, int32 DurationMinutes)
{
    OnPlayerBanned(PlayerID, Reason, DurationMinutes);
}

void ANetworkManager::InitializeNetworkManager()
{
    // Set up timers
    GetWorldTimerManager().SetTimer(NetworkUpdateTimerHandle, this, &ANetworkManager::UpdateNetworkQuality, NetworkUpdateInterval, true);
    GetWorldTimerManager().SetTimer(AntiCheatTimerHandle, this, &ANetworkManager::ValidateAllClients, 1.0f, true);
    GetWorldTimerManager().SetTimer(BandwidthOptimizationTimerHandle, this, &ANetworkManager::OptimizeReplication, 5.0f, true);
}

void ANetworkManager::UpdateClientConnections()
{
    // Update connection times and clean up disconnected clients
    float CurrentTime = GetWorld()->GetTimeSeconds();
    
    for (auto& ClientPair : ConnectedClients)
    {
        FClientInfo& ClientInfo = ClientPair.Value;
        ClientInfo.ConnectionTime = CurrentTime - ClientInfo.ConnectionTime;
    }
}

void ANetworkManager::ValidateAllClients()
{
    if (!bEnableAntiCheat)
    {
        return;
    }
    
    // Validate all connected clients
    for (const auto& ClientPair : ConnectedClients)
    {
        const FString& PlayerID = ClientPair.Key;
        const FClientInfo& ClientInfo = ClientPair.Value;
        
        // Check network quality
        if (ClientInfo.NetworkStats.Ping > MaxPing)
        {
            ReportSuspiciousActivity(PlayerID, "HighPing", FString::Printf(TEXT("Ping: %.2f ms"), ClientInfo.NetworkStats.Ping));
        }
        
        if (ClientInfo.NetworkStats.PacketLoss > MaxPacketLoss)
        {
            ReportSuspiciousActivity(PlayerID, "HighPacketLoss", FString::Printf(TEXT("Packet Loss: %.2f%%"), ClientInfo.NetworkStats.PacketLoss * 100.0f));
        }
    }
}

void ANetworkManager::OptimizeBandwidth()
{
    if (!bEnableBandwidthOptimization)
    {
        return;
    }
    
    // Optimize replication based on current network conditions
    OptimizeReplication();
}

void ANetworkManager::UpdateReplicationRates()
{
    // Update replication rates based on current network conditions
    OptimizeReplication();
}

void ANetworkManager::CleanupDisconnectedClients()
{
    // Remove clients that have been disconnected for too long
    float CurrentTime = GetWorld()->GetTimeSeconds();
    
    TArray<FString> ClientsToRemove;
    
    for (const auto& ClientPair : ConnectedClients)
    {
        const FString& PlayerID = ClientPair.Key;
        const FClientInfo& ClientInfo = ClientPair.Value;
        
        // Remove clients that haven't been updated in 30 seconds
        if (CurrentTime - ClientInfo.ConnectionTime > 30.0f)
        {
            ClientsToRemove.Add(PlayerID);
        }
    }
    
    for (const FString& PlayerID : ClientsToRemove)
    {
        UnregisterClient(PlayerID);
    }
}