// Copyright Epic Games, Inc. All Rights Reserved.

#include "UrbanCarnagePawn.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "UrbanCarnageWheelFront.h"
#include "UrbanCarnageWheelRear.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "UrbanCarnagePlayerController.h"
#include "Components/ArrowComponent.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"

#define LOCTEXT_NAMESPACE "VehiclePawn"

DEFINE_LOG_CATEGORY(LogTemplateVehicle);

AUrbanCarnagePawn::AUrbanCarnagePawn()
{


	// construct the back camera boom
	BackSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Back Spring Arm"));
	BackSpringArm->SetupAttachment(GetMesh());
	BackSpringArm->TargetArmLength = 650.0f;
	BackSpringArm->SocketOffset.Z = 150.0f;
	BackSpringArm->bDoCollisionTest = false;
	BackSpringArm->bInheritPitch = false;
	BackSpringArm->bInheritRoll = false;
	BackSpringArm->bEnableCameraRotationLag = true;
	BackSpringArm->CameraRotationLagSpeed = 2.0f;
	BackSpringArm->CameraLagMaxDistance = 50.0f;

	BackCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Back Camera"));
	BackCamera->SetupAttachment(BackSpringArm);

	AbilitySystemComponent=CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	
	// Configure the car mesh
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(FName("Vehicle"));

	// get the Chaos Wheeled movement component
	ChaosVehicleMovement = CastChecked<UChaosWheeledVehicleMovementComponent>(GetVehicleMovement());

	PrimaryWeaponSlot = CreateDefaultSubobject<USceneComponent>(TEXT("PrimaryWeaponSlot"));
	PrimaryWeaponSlot->SetupAttachment(GetMesh());
	SecondaryWeaponSlot1 = CreateDefaultSubobject<USceneComponent>(TEXT("SecondaryWeaponSlot1"));
	SecondaryWeaponSlot1->SetupAttachment(GetMesh());
	SecondaryWeaponSlot2 = CreateDefaultSubobject<USceneComponent>(TEXT("SecondaryWeaponSlot2"));
	SecondaryWeaponSlot2->SetupAttachment(GetMesh());
	
	
}

void AUrbanCarnagePawn::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AUrbanCarnagePawn,bIsInAir);
	DOREPLIFETIME(AUrbanCarnagePawn,AirSpeedMultiplier);
	DOREPLIFETIME(AUrbanCarnagePawn,AirTurnMultipler);
	DOREPLIFETIME(AUrbanCarnagePawn,IsParachuting);
	DOREPLIFETIME(AUrbanCarnagePawn,PrimaryWeapon_Ref)
	DOREPLIFETIME(AUrbanCarnagePawn,SecondaryWeapon_Ref1)
	DOREPLIFETIME(AUrbanCarnagePawn,SecondaryWeapon_Ref2)
	DOREPLIFETIME(AUrbanCarnagePawn,AimPoint);
	
}


void AUrbanCarnagePawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// steering
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &AUrbanCarnagePawn::Steering);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &AUrbanCarnagePawn::Steering);

		// throttle 
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &AUrbanCarnagePawn::Throttle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &AUrbanCarnagePawn::Throttle);

		// break 
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &AUrbanCarnagePawn::Brake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Started, this, &AUrbanCarnagePawn::StartBrake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Completed, this, &AUrbanCarnagePawn::StopBrake);

		// handbrake 
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Started, this, &AUrbanCarnagePawn::StartHandbrake);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &AUrbanCarnagePawn::StopHandbrake);

		// look around 
		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Triggered, this, &AUrbanCarnagePawn::LookAround);
		
		// reset the vehicle 
		EnhancedInputComponent->BindAction(ResetVehicleAction, ETriggerEvent::Triggered, this, &AUrbanCarnagePawn::ResetVehicle);
		// Fire
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &AUrbanCarnagePawn::Fire);
	}
	else
	{
		UE_LOG(LogTemplateVehicle, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void AUrbanCarnagePawn::Fire(const FInputActionValue& Value)
{
	// call the server fire function
	if (IsLocallyControlled())
	{
		Server_Fire();
	}
}

void AUrbanCarnagePawn::SetDeployMode(bool bDeploy)
{
	bIsInAir=bDeploy;
	DeployEffect_MC(bIsInAir);
	if (!bIsInAir) OpenParachutEffect_MC(false);
	GetMesh()->SetEnableGravity(!bDeploy);
	GetMesh()->SetLinearDamping(bIsInAir?1.0f:0.1f);
	GetMesh()->SetAngularDamping(bIsInAir?1.0f:0.1f);
	
}

void AUrbanCarnagePawn::CheckForGround()
{
	//linetrace down to check if we are on ground or not 1000 units
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation - FVector(0,0,1000);
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, CollisionParams);
	if (bHit)
	{
		bIsInAir=false;
		GetMesh()->SetEnableGravity(true);
		GetMesh()->SetLinearDamping(0.1f);
		GetMesh()->SetAngularDamping(0.1f);
		DeployEffect_MC(false);
		OpenParachutEffect_MC(false);
	}
	else
	{
		bIsInAir=true;
	}
}

void AUrbanCarnagePawn::OpenParachutEffect_MC_Implementation(bool Start)
{
	if (Start)
	{
		OpenParachutEffect_BP();
	}
	else
	{
		StopParachutEffect_BP();
	}
}

void AUrbanCarnagePawn::DeployEffect_MC_Implementation(bool Start)
{
	if (Start)
	{
		deployEffect_BP();
	}
	else
	{
		StopDeployEffect_BP();
	}
	
}

void AUrbanCarnagePawn::CalculateAimLocation()
{
	if (!IsLocallyControlled()) return;
	FVector StartLocation = BackCamera->GetComponentLocation();
	FVector EndLocation = StartLocation + (BackCamera->GetForwardVector() * 90000.0f);

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, CollisionParams);
	if (bHit)
	{
		AimPoint = HitResult.ImpactPoint;	
	}
	else
	{
		AimPoint = EndLocation;
	}
	
	Server_SetAimLocation(AimPoint);
	
}

void AUrbanCarnagePawn::Death()
{
	if (isDead) return;
	if (HasAuthority())
	{
		
		DestroyEffect_MC();
		//launch car upwards and add random touqe
		GetMesh()->AddImpulse(FVector(0,0,6000));
		GetMesh()->AddTorqueInRadians(FVector(FMath::RandRange(-500,500),FMath::RandRange(-500,500),FMath::RandRange(-500,500)),"None",true);
		//Destroy();
		isDead=true;
		//unpossess
		AController* _Controller = GetController();
		if (_Controller)
		{
			_Controller->UnPossess();
		}
	}
		
	/*if (AbilitySystemComponent)
	{
	    FGameplayTag DeathAbilityTag = FGameplayTag::RequestGameplayTag(FName("Ability.Death"));
	    bool HasAbility = AbilitySystemComponent->HasMatchingGameplayTag(DeathAbilityTag);
	    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Has Ability with tag 'Ability.Death': %d"), HasAbility));
		//activate
			    bool CanDo= AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(DeathAbilityTag));
	    //print can do to screen
	    if (GEngine)
	    {
	        GEngine->AddOnScreenDebugMessage(89, 2.0f, FColor::Red, FString::Printf(TEXT("Can do: %d"), CanDo));
	    }
	}*/
}

void AUrbanCarnagePawn::Death_Implementation()
{
    Death();
}

void AUrbanCarnagePawn::Destroyed()
{
	Super::Destroyed();
	if (!HasAuthority())return;
	//destroy all weapons
	if (PrimaryWeapon_Ref)
	{
		PrimaryWeapon_Ref->Destroy();
	}
	if (SecondaryWeapon_Ref1)
	{
		SecondaryWeapon_Ref1->Destroy();
	}
	if (SecondaryWeapon_Ref2)
	{
		SecondaryWeapon_Ref2->Destroy();
	}
	
}

void AUrbanCarnagePawn::DestroyEffect_MC_Implementation()
{
	DestroyEffect_BP();
}

void AUrbanCarnagePawn::Server_SetAimLocation_Implementation(FVector _AimPoint)
{
	AimPoint=_AimPoint;
	if (PrimaryWeapon_Ref)
		PrimaryWeapon_Ref->Aim(AimPoint);
	if (SecondaryWeapon_Ref1)
		SecondaryWeapon_Ref1->Aim(AimPoint);
	if (SecondaryWeapon_Ref2)
		SecondaryWeapon_Ref2->Aim(AimPoint);
}

void AUrbanCarnagePawn::Server_SetParachuting_Implementation(bool bParachuting)
{
	if (!bIsInAir) return;
	IsParachuting=bParachuting;
	if (IsParachuting)
	{
		OpenParachutEffect_MC(true);
	}
}

void AUrbanCarnagePawn::Server_Fire_Implementation()
{

	if (HasAuthority())
	{
		//call shoot function of weaponbase
		if (PrimaryWeapon_Ref)
		{
			PrimaryWeapon_Ref->Shoot();
		}
		if (SecondaryWeapon_Ref1)
		{
			SecondaryWeapon_Ref1->Shoot();
		}
		if (SecondaryWeapon_Ref2)
		{
			SecondaryWeapon_Ref2->Shoot();
		}
	}
}

void AUrbanCarnagePawn::Tick(float Delta)
{
	Super::Tick(Delta);
	
	// add some angular damping if the vehicle is in midair
	bool bMovingOnGround = ChaosVehicleMovement->IsMovingOnGround();
	GetMesh()->SetAngularDamping(bMovingOnGround ? 0.0f : 3.0f);

	/*// realign the camera yaw to face front
	float CameraYaw = BackSpringArm->GetRelativeRotation().Yaw;
	CameraYaw = FMath::FInterpTo(CameraYaw, 0.0f, Delta, 1.0f);

	BackSpringArm->SetRelativeRotation(FRotator(0.0f, CameraYaw, 0.0f));*/

	
	//in air controls---------------------------
	if (HasAuthority()&&bIsInAir)
	{
		//Add Force Forward where actor face but only forward world not if it looks down
		FVector ForwardVector = GetActorForwardVector();
		ForwardVector.Z = 0; // Zero out the Z component to ensure the force is only applied in the horizontal plane
		ForwardVector.Normalize(); // Normalize the vector to maintain direction
		ForwardVector*=AirSpeedMultiplier;
		FVector DownForce=FVector(0,0,IsParachuting?-0.3f:-1.0f);
		FVector Force = (ForwardVector +DownForce) * 3000000.0f; // Adjust the force magnitude as needed
		GetMesh()->AddForce(Force);
		//Add Torque on Z axis
		FVector Torque = FVector(0.0f, 0.0f, 100.0f*Delta*AirTurnMultipler); // Adjust the torque magnitude as needed
		GetMesh()->AddTorqueInRadians(Torque, NAME_None, true);
		GEngine->AddOnScreenDebugMessage(89, 0.1f, FColor::Red, FString::Printf(TEXT("AirSpeedMultiplier: %f"), AirSpeedMultiplier));
		GEngine->AddOnScreenDebugMessage(98, 0.1f, FColor::Red, FString::Printf(TEXT("AirTurnMultipler: %f"), AirTurnMultipler));
		CheckForGround();
	}
	//--------------------------
	CalculateAimLocation();
	
}

void AUrbanCarnagePawn::Server_SetAirSpeedMultiplier_Implementation(float _AirSpeedMultiplier)
{
	AirSpeedMultiplier=_AirSpeedMultiplier;
}
void AUrbanCarnagePawn::Server_SetAirTurnMultiplier_Implementation(float _AirTurnMultiplier)
{
	AirTurnMultipler=_AirTurnMultiplier;
}

void AUrbanCarnagePawn::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsLocallyControlled())
	{
		//get controller and call setup input
		AUrbanCarnagePlayerController* PlayerController = Cast<AUrbanCarnagePlayerController>(GetController());
		if (PlayerController)
		{
			PlayerController->setupContext();
		}
	}
	if (HasAuthority()&&!IsLocallyControlled())
	{
		/*
		 //spawn a primary weapon class in primaryweaponslot
		PrimaryWeapon_Ref = GetWorld()->SpawnActor<AWeaponBase>(PrimaryWeaponClass, PrimaryWeaponSlot->GetComponentLocation(), PrimaryWeaponSlot->GetComponentRotation());
		//attach primaryweapon_ref to the primaryweaponslot
		PrimaryWeapon_Ref->AttachToComponent(PrimaryWeaponSlot, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		PrimaryWeapon_Ref->SetOwner(this);
		//spawn a secondary weapon class in secondaryweaponslot1
		SecondaryWeapon_Ref1 = GetWorld()->SpawnActor<AWeaponBase>(SecondaryWeaponClass, SecondaryWeaponSlot1->GetComponentLocation(), SecondaryWeaponSlot1->GetComponentRotation());
		//attach secondaryweapon_ref1 to the secondaryweaponslot1
		SecondaryWeapon_Ref1->AttachToComponent(SecondaryWeaponSlot1, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		SecondaryWeapon_Ref1->SetOwner(this);
		//spawn a secondary weapon class in secondaryweaponslot2
		SecondaryWeapon_Ref2 = GetWorld()->SpawnActor<AWeaponBase>(SecondaryWeaponClass, SecondaryWeaponSlot2->GetComponentLocation(), SecondaryWeaponSlot2->GetComponentRotation());
		//attach secondaryweapon_ref2 to the secondaryweaponslot2
		SecondaryWeapon_Ref2->AttachToComponent(SecondaryWeaponSlot2, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		SecondaryWeapon_Ref2->SetOwner(this);
		*/
	} 
	//add abilities from InitialAbilities
	if (AbilitySystemComponent)
	{
		for (TSubclassOf<UGameplayAbility>& StartupAbility : InitialAbilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility.GetDefaultObject(), 1, 0));
		}
	}
	
}

AWeaponBase* AUrbanCarnagePawn::EquipWeapon(TSubclassOf<AWeaponBase> WeaponClass, bool PrimaryWeapon)
{
	if (PrimaryWeapon)
	{
		if (PrimaryWeapon_Ref)return nullptr;
		
		PrimaryWeapon_Ref = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, PrimaryWeaponSlot->GetComponentLocation(), PrimaryWeaponSlot->GetComponentRotation());
		//attach primaryweapon_ref to the primaryweaponslot
		PrimaryWeapon_Ref->AttachToComponent(PrimaryWeaponSlot, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		PrimaryWeapon_Ref->SetOwner(this);
		return PrimaryWeapon_Ref;
	}
	else
	{
	//check if we have any secondary weapon and attach to the available slot else return false
		if (!SecondaryWeapon_Ref2)
		{
			SecondaryWeapon_Ref2 = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, SecondaryWeaponSlot2->GetComponentLocation(), SecondaryWeaponSlot2->GetComponentRotation());
			//attach secondaryweapon_ref2 to the secondaryweaponslot2
			SecondaryWeapon_Ref2->AttachToComponent(SecondaryWeaponSlot2, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			SecondaryWeapon_Ref2->SetOwner(this);
			return SecondaryWeapon_Ref2;
		}
		else if (!SecondaryWeapon_Ref1)
		{
			SecondaryWeapon_Ref1 = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, SecondaryWeaponSlot1->GetComponentLocation(), SecondaryWeaponSlot1->GetComponentRotation());
			//attach secondaryweapon_ref1 to the secondaryweaponslot1
			SecondaryWeapon_Ref1->AttachToComponent(SecondaryWeaponSlot1, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			SecondaryWeapon_Ref1->SetOwner(this);
			return SecondaryWeapon_Ref1;
		}
		return nullptr;
	}
	
}

void AUrbanCarnagePawn::Steering(const FInputActionValue& Value)
{
	// get the input magnitude for steering
	float SteeringValue = Value.Get<float>();

	// add the input
	ChaosVehicleMovement->SetSteeringInput(SteeringValue);
	Server_SetAirTurnMultiplier(SteeringValue);
}

void AUrbanCarnagePawn::Throttle(const FInputActionValue& Value)
{
	// get the input magnitude for the throttle
	float ThrottleValue = Value.Get<float>();

	// add the input
	ChaosVehicleMovement->SetThrottleInput(ThrottleValue);
	float AirMulti=1;
	if (ThrottleValue>0)
	{
		AirMulti=1.5f;
	}
	else if (ThrottleValue==0)
	{
		AirMulti=1;
	}
	
	Server_SetAirSpeedMultiplier(AirMulti);
}

void AUrbanCarnagePawn::Brake(const FInputActionValue& Value)
{
	// get the input magnitude for the brakes
	float BreakValue = Value.Get<float>();

	// add the input
	ChaosVehicleMovement->SetBrakeInput(BreakValue);
	Server_SetAirSpeedMultiplier(0.5f);
}

void AUrbanCarnagePawn::StartBrake(const FInputActionValue& Value)
{
	float VehicleSpeed = ChaosVehicleMovement->GetForwardSpeed();
	
	
	if ( VehicleSpeed > 0.0f)
	{
		BrakeLights(true);
	}
	if ( VehicleSpeed <= 0.0f)
	{
		BrakeLights(false);
	}
	
	// call the Blueprint hook for the break lights

}

void AUrbanCarnagePawn::StopBrake(const FInputActionValue& Value)
{
	// call the Blueprint hook for the break lights
	BrakeLights(false);

	// reset brake input to zero
	ChaosVehicleMovement->SetBrakeInput(0.0f);
}

void AUrbanCarnagePawn::StartHandbrake(const FInputActionValue& Value)
{
	// add the input
	ChaosVehicleMovement->SetHandbrakeInput(true);

	// call the Blueprint hook for the break lights
	//BrakeLights(true);
	Server_SetParachuting(true);
}

void AUrbanCarnagePawn::StopHandbrake(const FInputActionValue& Value)
{
	// add the input
	ChaosVehicleMovement->SetHandbrakeInput(false);

	// call the Blueprint hook for the break lights
	//BrakeLights(false);
}

void AUrbanCarnagePawn::LookAround(const FInputActionValue& Value)
{
	// get value for looking around as 2D vector
	
	FVector2D LookValue = Value.Get<FVector2D>();

	// add the input
	AddControllerPitchInput(-LookValue.Y);
	AddControllerYawInput(LookValue.X);

}


void AUrbanCarnagePawn::ResetVehicle(const FInputActionValue& Value)
{
	// reset to a location slightly above our current one
	FVector ResetLocation = GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);

	// reset to our yaw. Ignore pitch and roll
	FRotator ResetRotation = GetActorRotation();
	ResetRotation.Pitch = 0.0f;
	ResetRotation.Roll = 0.0f;
	
	// teleport the actor to the reset spot and reset physics
	SetActorTransform(FTransform(ResetRotation, ResetLocation, FVector::OneVector), false, nullptr, ETeleportType::TeleportPhysics);

	GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);

	UE_LOG(LogTemplateVehicle, Error, TEXT("Reset Vehicle"));
}

#undef LOCTEXT_NAMESPACE