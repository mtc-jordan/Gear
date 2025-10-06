// Copyright Epic Games, Inc. All Rights Reserved.


#include "UrbanCarnagePlayerController.h"
#include "UrbanCarnagePawn.h"
#include "UrbanCarnageUI.h"
#include "EnhancedInputSubsystems.h"
#include "ChaosWheeledVehicleMovementComponent.h"

void AUrbanCarnagePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController())
	return;
	// spawn the UI widget and add it to the viewport
	//VehicleUI = CreateWidget<UUrbanCarnageUI>(this, VehicleUIClass);

	//check(VehicleUI);

	//VehicleUI->AddToViewport();
}

void AUrbanCarnagePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	// get the enhanced input subsystem
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
		// add the mapping context so we get controls
		Subsystem->AddMappingContext(InputMappingContext, 0);

		// optionally add the steering wheel context
		if (bUseSteeringWheelControls && SteeringWheelInputMappingContext)
		{
			Subsystem->AddMappingContext(SteeringWheelInputMappingContext, 1);
		}
	}
}

void AUrbanCarnagePlayerController::Tick(float Delta)
{
	Super::Tick(Delta);

	
}

void AUrbanCarnagePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (Cast<AUrbanCarnagePawn>(InPawn) == nullptr)
	{
		return;
	}
	// get a pointer to the controlled pawn
	//VehiclePawn = CastChecked<AUrbanCarnagePawn>(InPawn);
	SetupInputComponent();
}
