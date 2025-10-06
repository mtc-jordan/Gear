// Copyright Epic Games, Inc. All Rights Reserved.

#include "UrbanCarnageWheelRear.h"
#include "UObject/ConstructorHelpers.h"

UUrbanCarnageWheelRear::UUrbanCarnageWheelRear()
{
	AxleType = EAxleType::Rear;
	bAffectedByHandbrake = true;
	bAffectedByEngine = true;
}