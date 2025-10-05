// Fill out your copyright notice in the Description page of Project Settings.

#include "GASWheeledVehiclePawn.h"

#include "AbilitySystemComponent.h"
#include "CarAttributeSet.h"

AGASWheeledVehiclePawn::AGASWheeledVehiclePawn()
{
    //create the Inventory Component
    InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
    // Create the Ability System Component

    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true); // Enable replication

    // Create the Car Attribute Set
    AttributeSet = CreateDefaultSubobject<UCarAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AGASWheeledVehiclePawn::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}