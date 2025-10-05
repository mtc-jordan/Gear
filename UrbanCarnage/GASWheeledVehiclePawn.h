// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "AbilitySystemInterface.h"
#include "CarAttributeSet.h"
#include "InventoryComponent.h"
#include "GASWheeledVehiclePawn.generated.h"

/**
 * 
 */
UCLASS()
class URBANCARNAGE_API AGASWheeledVehiclePawn : public AWheeledVehiclePawn, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    AGASWheeledVehiclePawn();

    // Implement the Ability System Interface
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;



protected:


    // Ability System Component for GAS
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
    UAbilitySystemComponent* AbilitySystemComponent;

    //inventory component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UInventoryComponent* InventoryComponent;

    // Attribute Set for storing car stats
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
    UCarAttributeSet* AttributeSet;
};