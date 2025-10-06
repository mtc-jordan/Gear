// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "Net/UnrealNetwork.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "CoreMinimal.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "UrbanCarnagePawn.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "Core/BulletBase.h"


// Sets default values
AWeaponBase::AWeaponBase()
{
	//set replicated
	SetReplicates(true);
	
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponBaseRoot = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponBaseRoot"));
	RootComponent = WeaponBaseRoot;
	TurretBase = CreateDefaultSubobject<USceneComponent>(TEXT("TurretBase"));
	TurretBase->SetupAttachment(WeaponBaseRoot);
	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretMesh"));
	TurretMesh->SetupAttachment(TurretBase);
	CannonBase = CreateDefaultSubobject<USceneComponent>(TEXT("CannonBase"));
	CannonBase->SetupAttachment(TurretBase);
	CannonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CannonMesh"));
	CannonMesh->SetupAttachment(CannonBase);
	Muzzle = CreateDefaultSubobject<UArrowComponent>(TEXT("MuzzleLocation"));
	Muzzle->SetupAttachment(CannonBase);
	//set replicate movement to false
	
	
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeaponBase,AimRotationStruct);

	
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void AWeaponBase::Aim(FVector _AimPoint)
{
	if (!GetOwner())return;
	//if is locally controlled pawn, return 
	/*if (GetOwner()->GetLocalRole() == Role)
	{
		return;
	}*/
	// aim the turret and cannon at the aim point with interpolation
	FRotator TurretRotation = TurretBase->GetComponentRotation();
	TurretRotation.Roll=GetOwner()->GetActorRotation().Roll;
	TurretRotation.Pitch=GetOwner()->GetActorRotation().Pitch;
	FRotator CannonRotation = CannonBase->GetComponentRotation();
	CannonRotation.Yaw = TurretRotation.Yaw;
	CannonRotation.Roll = TurretRotation.Roll;
	FRotator AimRotationTurret = (_AimPoint - TurretBase->GetComponentLocation()).Rotation();
	FRotator AimRotationCannon = (_AimPoint - CannonBase->GetComponentLocation()).Rotation();
	// interpolate the turret rotation in yaw only relative to the car
	TurretRotation.Yaw = FMath::FInterpTo(TurretRotation.Yaw, AimRotationTurret.Yaw, GetWorld()->GetDeltaSeconds(), WeaponRotationSpeed);
	TurretBase->SetWorldRotation(TurretRotation);
	//TurretRotation.Pitch=GetActorRotation().Pitch;
	//TurretRotation.Roll=GetActorRotation().Roll;
	
	TurretBase->SetWorldRotation(TurretRotation);
	// interpolate the cannon rotation in pitch only relative to the turret
	CannonRotation.Pitch = FMath::FInterpTo(CannonRotation.Pitch, AimRotationCannon.Pitch, GetWorld()->GetDeltaSeconds(), WeaponRotationSpeed);
	//CannonRotation.Yaw=TurretRotation.Yaw;
	//CannonRotation.Roll=TurretRotation.Roll;
	CannonBase->SetWorldRotation(CannonRotation);
	// Debug messages to check the rotations
	
	AimRotation(TurretRotation, CannonRotation);
	AimRotationStruct.CannonAimRotation = CannonRotation;
	AimRotationStruct.TurretAimRotation = TurretRotation;
	
	//
	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(5687, 5.f, FColor::Green, FString::Printf(TEXT("Turret Yaw: %f"), TurretRotation.Yaw));
	// 	GEngine->AddOnScreenDebugMessage(5688, 5.f, FColor::Green, FString::Printf(TEXT("Cannon Pitch: %f"), CannonRotation.Pitch));
	// }
	
}

void AWeaponBase::AimRotation_Implementation(FRotator TurretRotation, FRotator CannonRotation)
{
	//only do this if not locally controlled pawn
	
	TurretBase->SetWorldRotation(TurretRotation);
	CannonBase->SetWorldRotation(CannonRotation);
}

FWeaponAimRotation AWeaponBase::GetAimRotation()
{
	return AimRotationStruct;
}

void AWeaponBase::Shoot()
{
	if (!HasAuthority()) return;
	if (!bReadyToFire) return;
    
	AActor* bullet = GetWorld()->SpawnActor<AActor>(BulletClass, Muzzle->GetComponentLocation(), Muzzle->GetComponentRotation());
	if (bullet)
	{
		// set the owner of the bullet to this pawn
		bullet->SetOwner(GetOwner());
		//Set velocity
		//Cast<ABulletBase>(bullet)->ProjectileMovementComponent->Velocity=bullet->GetActorForwardVector()*72000.f;
		PlayEffect();
		// Debug message to check if the bullet is spawned
		//GEngine->AddOnScreenDebugMessage(5689, 5.f, FColor::Green, FString::Printf(TEXT("Bullet Spawned")));
		ShotFired();
	}
	GetWorld()->GetTimerManager().SetTimer(FireRateTimer, this, &AWeaponBase::SetReadyToFire, (1/FireRate)/FireRateMultiplier, false);
	//print (1/FireRate)*FireRateMultiplier to screen
	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(5689, 5.f, FColor::Green, FString::Printf(TEXT("FireRate: %f"), (1/FireRate)/FireRateMultiplier));
	}
	bReadyToFire = false;
}

void AWeaponBase::ShotFired_Implementation()
{
	ShotFiredEffect();
}

void AWeaponBase::SetReadyToFire()
{
	bReadyToFire = true;
}
void AWeaponBase::PlayEffect_Implementation()
{
	PlayEffectBP();
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

