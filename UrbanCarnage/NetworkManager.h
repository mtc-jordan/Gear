// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "NetworkManager.generated.h"

UENUM(BlueprintType)
enum class ENetworkQuality : uint8
{
    Poor      UMETA(DisplayName = "Poor"),
    Fair      UMETA(DisplayName = "Fair"),
    Good      UMETA(DisplayName = "Good"),
    Excellent UMETA(DisplayName = "Excellent")
};

USTRUCT(BlueprintType)
struct FNetworkStats
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Network Stats")
    float Ping = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Network Stats")
    float PacketLoss = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Network Stats")
    float Jitter = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Network Stats")
    int32 PacketsPerSecond = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Network Stats")
    int32 BytesPerSecond = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Network Stats")
    ENetworkQuality Quality = ENetworkQuality::Good;

    FNetworkStats()
    {
        Ping = 0.0f;
        PacketLoss = 0.0f;
        Jitter = 0.0f;
        PacketsPerSecond = 0;
        BytesPerSecond = 0;
        Quality = ENetworkQuality::Good;
    }
};

USTRUCT(BlueprintType)
struct FClientInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Client Info")
    FString PlayerName;

    UPROPERTY(BlueprintReadOnly, Category = "Client Info")
    FString PlayerID;

    UPROPERTY(BlueprintReadOnly, Category = "Client Info")
    FString IPAddress;

    UPROPERTY(BlueprintReadOnly, Category = "Client Info")
    FNetworkStats NetworkStats;

    UPROPERTY(BlueprintReadOnly, Category = "Client Info")
    float ConnectionTime = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Client Info")
    bool bIsValidated = false;

    UPROPERTY(BlueprintReadOnly, Category = "Client Info")
    int32 SuspiciousActions = 0;

    FClientInfo()
    {
        PlayerName = "";
        PlayerID = "";
        IPAddress = "";
        ConnectionTime = 0.0f;
        bIsValidated = false;
        SuspiciousActions = 0;
    }
};

USTRUCT(BlueprintType)
struct FReplicationSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Replication")
    float PositionUpdateRate = 20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Replication")
    float RotationUpdateRate = 20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Replication")
    float VelocityUpdateRate = 30.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Replication")
    float HealthUpdateRate = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Replication")
    float WeaponUpdateRate = 30.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Replication")
    float GameStateUpdateRate = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Replication")
    float MaxUpdateDistance = 10000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Replication")
    bool bUseLagCompensation = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Replication")
    float LagCompensationTime = 0.2f;

    FReplicationSettings()
    {
        PositionUpdateRate = 20.0f;
        RotationUpdateRate = 20.0f;
        VelocityUpdateRate = 30.0f;
        HealthUpdateRate = 10.0f;
        WeaponUpdateRate = 30.0f;
        GameStateUpdateRate = 5.0f;
        MaxUpdateDistance = 10000.0f;
        bUseLagCompensation = true;
        LagCompensationTime = 0.2f;
    }
};

UCLASS()
class URBANCARNAGE_API ANetworkManager : public AActor
{
    GENERATED_BODY()

public:
    ANetworkManager();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
    // Network Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network Settings")
    FReplicationSettings ReplicationSettings;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network Settings")
    bool bEnableAntiCheat = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network Settings")
    bool bEnableLagCompensation = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network Settings")
    bool bEnableBandwidthOptimization = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network Settings")
    float MaxPing = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network Settings")
    float MaxPacketLoss = 0.05f; // 5%

    // Client Management
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Client Management")
    TMap<FString, FClientInfo> ConnectedClients;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Client Management")
    int32 MaxClients = 8;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Client Management")
    int32 CurrentClients = 0;

    // Network Functions
    UFUNCTION(BlueprintCallable, Category = "Network Management")
    void RegisterClient(const FString& PlayerID, const FString& PlayerName, const FString& IPAddress);

    UFUNCTION(BlueprintCallable, Category = "Network Management")
    void UnregisterClient(const FString& PlayerID);

    UFUNCTION(BlueprintCallable, Category = "Network Management")
    void UpdateClientNetworkStats(const FString& PlayerID, const FNetworkStats& Stats);

    UFUNCTION(BlueprintCallable, Category = "Network Management")
    bool IsClientValid(const FString& PlayerID) const;

    UFUNCTION(BlueprintCallable, Category = "Network Management")
    FClientInfo GetClientInfo(const FString& PlayerID) const;

    // Anti-Cheat Functions
    UFUNCTION(BlueprintCallable, Category = "Anti-Cheat")
    bool ValidatePlayerAction(const FString& PlayerID, const FString& ActionType, const FVector& Location, const FRotator& Rotation);

    UFUNCTION(BlueprintCallable, Category = "Anti-Cheat")
    bool ValidatePlayerMovement(const FString& PlayerID, const FVector& NewLocation, const FVector& OldLocation, float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Anti-Cheat")
    bool ValidatePlayerWeapon(const FString& PlayerID, const FString& WeaponType, float FireRate);

    UFUNCTION(BlueprintCallable, Category = "Anti-Cheat")
    void ReportSuspiciousActivity(const FString& PlayerID, const FString& ActivityType, const FString& Details);

    UFUNCTION(BlueprintCallable, Category = "Anti-Cheat")
    void KickPlayer(const FString& PlayerID, const FString& Reason);

    UFUNCTION(BlueprintCallable, Category = "Anti-Cheat")
    void BanPlayer(const FString& PlayerID, const FString& Reason, int32 DurationMinutes = 0);

    // Lag Compensation
    UFUNCTION(BlueprintCallable, Category = "Lag Compensation")
    FVector CompensatePosition(const FVector& Position, float ClientPing);

    UFUNCTION(BlueprintCallable, Category = "Lag Compensation")
    FRotator CompensateRotation(const FRotator& Rotation, float ClientPing);

    UFUNCTION(BlueprintCallable, Category = "Lag Compensation")
    bool IsHitValid(const FVector& HitLocation, const FVector& ShooterLocation, const FVector& TargetLocation, float ClientPing);

    // Bandwidth Optimization
    UFUNCTION(BlueprintCallable, Category = "Bandwidth Optimization")
    void OptimizeReplication();

    UFUNCTION(BlueprintCallable, Category = "Bandwidth Optimization")
    void SetReplicationRate(const FString& ComponentType, float Rate);

    UFUNCTION(BlueprintCallable, Category = "Bandwidth Optimization")
    void EnableDistanceCulling(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "Bandwidth Optimization")
    void SetMaxUpdateDistance(float Distance);

    // Network Quality
    UFUNCTION(BlueprintCallable, Category = "Network Quality")
    ENetworkQuality CalculateNetworkQuality(float Ping, float PacketLoss, float Jitter);

    UFUNCTION(BlueprintCallable, Category = "Network Quality")
    void UpdateNetworkQuality();

    UFUNCTION(BlueprintCallable, Category = "Network Quality")
    FNetworkStats GetNetworkStats() const;

    // Server Functions
    UFUNCTION(Server, Reliable, Category = "Server")
    void Server_RegisterClient(const FString& PlayerID, const FString& PlayerName, const FString& IPAddress);

    UFUNCTION(Server, Reliable, Category = "Server")
    void Server_UnregisterClient(const FString& PlayerID);

    UFUNCTION(Server, Reliable, Category = "Server")
    void Server_UpdateNetworkStats(const FString& PlayerID, const FNetworkStats& Stats);

    UFUNCTION(Server, Reliable, Category = "Server")
    void Server_ReportSuspiciousActivity(const FString& PlayerID, const FString& ActivityType, const FString& Details);

    UFUNCTION(Server, Reliable, Category = "Server")
    void Server_KickPlayer(const FString& PlayerID, const FString& Reason);

    UFUNCTION(Server, Reliable, Category = "Server")
    void Server_BanPlayer(const FString& PlayerID, const FString& Reason, int32 DurationMinutes);

    // Multicast Functions
    UFUNCTION(NetMulticast, Reliable, Category = "Multicast")
    void Multicast_ClientJoined(const FString& PlayerID, const FString& PlayerName);

    UFUNCTION(NetMulticast, Reliable, Category = "Multicast")
    void Multicast_ClientLeft(const FString& PlayerID, const FString& PlayerName);

    UFUNCTION(NetMulticast, Reliable, Category = "Multicast")
    void Multicast_PlayerKicked(const FString& PlayerID, const FString& Reason);

    UFUNCTION(NetMulticast, Reliable, Category = "Multicast")
    void Multicast_PlayerBanned(const FString& PlayerID, const FString& Reason, int32 DurationMinutes);

    // Blueprint Events
    UFUNCTION(BlueprintImplementableEvent, Category = "Network Events")
    void OnClientJoined(const FString& PlayerID, const FString& PlayerName);

    UFUNCTION(BlueprintImplementableEvent, Category = "Network Events")
    void OnClientLeft(const FString& PlayerID, const FString& PlayerName);

    UFUNCTION(BlueprintImplementableEvent, Category = "Network Events")
    void OnSuspiciousActivity(const FString& PlayerID, const FString& ActivityType, const FString& Details);

    UFUNCTION(BlueprintImplementableEvent, Category = "Network Events")
    void OnPlayerKicked(const FString& PlayerID, const FString& Reason);

    UFUNCTION(BlueprintImplementableEvent, Category = "Network Events")
    void OnPlayerBanned(const FString& PlayerID, const FString& Reason, int32 DurationMinutes);

    UFUNCTION(BlueprintImplementableEvent, Category = "Network Events")
    void OnNetworkQualityChanged(ENetworkQuality NewQuality);

protected:
    // Internal Functions
    void InitializeNetworkManager();
    void UpdateClientConnections();
    void ValidateAllClients();
    void OptimizeBandwidth();
    void UpdateReplicationRates();
    void CleanupDisconnectedClients();

    // Anti-Cheat Variables
    UPROPERTY()
    TMap<FString, TArray<FVector>> PlayerPositionHistory;

    UPROPERTY()
    TMap<FString, float> PlayerLastUpdateTime;

    UPROPERTY()
    TMap<FString, int32> PlayerSuspiciousCount;

    UPROPERTY()
    TArray<FString> BannedPlayers;

    // Network Statistics
    UPROPERTY()
    FNetworkStats CurrentNetworkStats;

    UPROPERTY()
    float LastNetworkUpdateTime = 0.0f;

    UPROPERTY()
    float NetworkUpdateInterval = 1.0f;

    // Timer Handles
    FTimerHandle NetworkUpdateTimerHandle;
    FTimerHandle AntiCheatTimerHandle;
    FTimerHandle BandwidthOptimizationTimerHandle;

    // Constants
    static const float MAX_MOVEMENT_SPEED;
    static const float MAX_ROTATION_SPEED;
    static const float MAX_WEAPON_FIRE_RATE;
    static const int32 MAX_SUSPICIOUS_ACTIONS;
    static const float VALIDATION_TOLERANCE;
};