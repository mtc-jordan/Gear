// Copyright Epic Games, Inc. All Rights Reserved.

#include "UrbanCarnageWheelFront.h"
#include "UObject/ConstructorHelpers.h"

UUrbanCarnageWheelFront::UUrbanCarnageWheelFront()
{
	AxleType = EAxleType::Front;
	bAffectedBySteering = true;
	MaxSteerAngle = 40.f;
}