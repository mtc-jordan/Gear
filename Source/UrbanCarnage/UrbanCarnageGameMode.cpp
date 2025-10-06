// Copyright Epic Games, Inc. All Rights Reserved.

#include "UrbanCarnageGameMode.h"
#include "UrbanCarnagePlayerController.h"

AUrbanCarnageGameMode::AUrbanCarnageGameMode()
{
	PlayerControllerClass = AUrbanCarnagePlayerController::StaticClass();
}
