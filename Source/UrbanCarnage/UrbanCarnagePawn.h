// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "WheeledVehiclePawn.h"
#include "Core/BulletBase.h"
#include "Core/DamageInterface_BASE.h"
#include "UrbanCarnagePawn.generated.h"

class UArrowComponent;
class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UChaosWheeledVehicleMovementComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateVehicle, Log, All);

/**
 *  Vehicle Pawn class
 *  Handles common functionality for all vehicle types,
 *  including input handling and camera management.
 *  
 *  Specific vehicle configurations are handled in subclasses.
 */
UCLASS(abstract)
class AUrbanCarnagePawn : public AWheeledVehiclePawn, public IDamageInterface_BASE
{
	GENERATED_BODY()

	
	/** Spring Arm for the back camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* BackSpringArm;

	/** Back Camera component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* BackCamera;

	/** Cast pointer to the Chaos Vehicle movement component */
	TObjectPtr<UChaosWheeledVehicleMovementComponent> ChaosVehicleMovement;

	//add abilitysystem componenet
	

protected:

	/** Steering Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* SteeringAction;

	/** Throttle Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ThrottleAction;

	/** Brake Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* BrakeAction;

	/** Handbrake Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* HandbrakeAction;

	/** Look Around Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAroundAction;

	/** Toggle Camera Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ToggleCameraAction;

	/** Reset Vehicle Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ResetVehicleAction;

	
public:
	AUrbanCarnagePawn();

	// Begin Pawn interface
	void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	// End Pawn interface

	// Begin Actor interface

	virtual void Tick(float Delta) override;

	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	class UAbilitySystemComponent* AbilitySystemComponent;
	//add initial abilities
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	TArray<TSubclassOf<class UGameplayAbility>> InitialAbilities;
	UFUNCTION(BlueprintCallable)
	AWeaponBase* EquipWeapon(TSubclassOf<AWeaponBase> WeaponClass, bool PrimaryWeapon);
    
    // Damage interface implementation
    virtual void Death_Implementation() override;
	

protected:

	/** Handles steering input */
	void Steering(const FInputActionValue& Value);

	/** Handles throttle input */
	void Throttle(const FInputActionValue& Value);

	/** Handles brake input */
	void Brake(const FInputActionValue& Value);

	/** Handles brake start/stop inputs */
	void StartBrake(const FInputActionValue& Value);
	void StopBrake(const FInputActionValue& Value);

	/** Handles handbrake start/stop inputs */
	void StartHandbrake(const FInputActionValue& Value);
	void StopHandbrake(const FInputActionValue& Value);

	/** Handles look around input */
	void LookAround(const FInputActionValue& Value);
	
	/** Handles reset vehicle input */
	void ResetVehicle(const FInputActionValue& Value);

	/** Called when the brake lights are turned on or off */
	UFUNCTION(BlueprintImplementableEvent, Category="Vehicle")
	void BrakeLights(bool bBraking);

public:
	/** Returns the back spring arm subobject */
	FORCEINLINE USpringArmComponent* GetBackSpringArm() const { return BackSpringArm; }
	/** Returns the back camera subobject */
	FORCEINLINE UCameraComponent* GetBackCamera() const { return BackCamera; }
	/** Returns the cast Chaos Vehicle Movement subobject */
	FORCEINLINE const TObjectPtr<UChaosWheeledVehicleMovementComponent>& GetChaosVehicleMovement() const { return ChaosVehicleMovement; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Vehicle",Replicated)
	TSubclassOf<ABulletBase> BulletClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Vehicle")
	UInputAction* FireAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Vehicle")
	TSubclassOf<AWeaponBase> PrimaryWeaponClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Vehicle")
	TSubclassOf<AWeaponBase> SecondaryWeaponClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Vehicle")
	USceneComponent* PrimaryWeaponSlot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Vehicle")
	USceneComponent* SecondaryWeaponSlot1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Vehicle")
	USceneComponent* SecondaryWeaponSlot2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Vehicle",Replicated)
	AWeaponBase* PrimaryWeapon_Ref;
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Vehicle",Replicated)
	AWeaponBase* SecondaryWeapon_Ref1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Vehicle",Replicated)
	AWeaponBase* SecondaryWeapon_Ref2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Vehicle",Replicated)
	bool bIsInAir;
	
	UFUNCTION()
	void Fire(const FInputActionValue& Value);
	UFUNCTION(Server, Reliable)
	void Server_Fire();

	
	UFUNCTION(BlueprintCallable)
	void SetDeployMode(bool bDeploy);
	UPROPERTY(Replicated)
	float AirSpeedMultiplier=1.0f;
	UPROPERTY(Replicated)
	float AirTurnMultipler=0.0f;
	UFUNCTION(Server,Unreliable)
	void Server_SetAirSpeedMultiplier(float _AirSpeedMultiplier);
	UFUNCTION(Server,Unreliable)
	void Server_SetAirTurnMultiplier(float _AirTurnMultiplier);

	void CheckForGround();
	UPROPERTY(Replicated)
	bool IsParachuting=false;
	UFUNCTION(Server,Unreliable)
	void Server_SetParachuting(bool bParachuting);
	UFUNCTION(NetMulticast,Unreliable)
	void OpenParachutEffect_MC(bool Start);
	UFUNCTION(BlueprintImplementableEvent)
	void OpenParachutEffect_BP();
	UFUNCTION(BlueprintImplementableEvent)
	void StopParachutEffect_BP();
	UFUNCTION(NetMulticast,Unreliable)
	void DeployEffect_MC(bool Start);
	UFUNCTION(BlueprintImplementableEvent)
	void deployEffect_BP();
	UFUNCTION(BlueprintImplementableEvent)
	void StopDeployEffect_BP();
	//Dynamic Aiming sys------------------------
	UFUNCTION()
	void CalculateAimLocation();
	
	UPROPERTY(Replicated)
	FVector AimPoint;
	
	UFUNCTION(Server,Reliable)
	void Server_SetAimLocation(FVector _AimPoint);
	
	//-------------------------------------------
	void Death();
	virtual void Destroyed() override;
	UFUNCTION(NetMulticast,Unreliable)
	void DestroyEffect_MC();
	UFUNCTION(BlueprintImplementableEvent)
	void DestroyEffect_BP();
	bool isDead=false;
	UPROPERTY(BlueprintReadWrite)
	bool b_CanAim=true;
};





	
