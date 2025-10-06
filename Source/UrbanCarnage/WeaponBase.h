// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "Core/BulletBase.h"
#include "WeaponBase.generated.h"

//make a blueprint struct for cannon aim rotation and turret aim rotation
USTRUCT(BlueprintType)
struct FWeaponAimRotation
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator CannonAimRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator TurretAimRotation;
};

UCLASS()
class URBANCARNAGE_API AWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWeaponBase();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	USceneComponent * WeaponBaseRoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Weapon")
	USceneComponent* TurretBase;
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Weapon")
	UStaticMeshComponent* TurretMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Weapon")
	USceneComponent* CannonBase;
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Weapon")
	UStaticMeshComponent* CannonMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Weapon")
	UArrowComponent* Muzzle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Weapon")
	bool PrimaryWeapon = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Weapon")
	bool SecondaryWeapon = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Weapon")
	bool CanRotate = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Weapon")
	float FireRate = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Weapon")
	float WeaponRotationSpeed=15.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Weapon")
	float FireRateMultiplier = 1.0f;
	
	
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	//virtual void SetupPlayerInputComponent(UInputComponent* InputComponent);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	FTimerHandle FireRateTimer;

	UPROPERTY()
	bool bReadyToFire=true;

	UFUNCTION()
	void SetReadyToFire();
	

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	

	UFUNCTION(BlueprintCallable)
	void Aim(FVector _AimPoint);
	UFUNCTION(NetMulticast, Unreliable)
	void AimRotation(FRotator TurretRotation, FRotator CannonRotation);
	UPROPERTY(Replicated)
	FWeaponAimRotation AimRotationStruct;
	UFUNCTION(BlueprintCallable,BlueprintPure)
	FWeaponAimRotation GetAimRotation();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Default")
	TSubclassOf<ABulletBase> BulletClass;
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Vehicle")
	UInputAction* FireAction;*/
	UFUNCTION(BlueprintCallable)
	void Shoot();
	UFUNCTION(Client,Unreliable)
	void ShotFired();
	UFUNCTION(BlueprintImplementableEvent)
	void ShotFiredEffect();

	UFUNCTION(NetMulticast, Unreliable)
	void PlayEffect();
	UFUNCTION(BlueprintImplementableEvent)
	void PlayEffectBP();
	
};

 
