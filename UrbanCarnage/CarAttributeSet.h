// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CarAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


/**
 * 
 */
UCLASS()
class URBANCARNAGE_API UCarAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    UCarAttributeSet();
    void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data);

    // Attribute: Health
    UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UCarAttributeSet, Health)

        UFUNCTION()
    void OnRep_Health(const FGameplayAttributeData& OldHealth);

    // Shield
        UPROPERTY(BlueprintReadOnly, Category = "Shield", ReplicatedUsing = OnRep_Shield)
    FGameplayAttributeData Shield;
    ATTRIBUTE_ACCESSORS(UCarAttributeSet, Shield)
        UFUNCTION()
    void OnRep_Shield(const FGameplayAttributeData& OldShield);

    /*
    // Medkit 25%
    UPROPERTY(BlueprintReadOnly, Category = "Inventory", ReplicatedUsing = OnRep_Medkit25)
    FGameplayAttributeData Medkit25;
    ATTRIBUTE_ACCESSORS(UCarAttributeSet, Medkit25)
        UFUNCTION()
    void OnRep_Medkit25(const FGameplayAttributeData& OldMedkit25);

    // Medkit 75%
    UPROPERTY(BlueprintReadOnly, Category = "Inventory", ReplicatedUsing = OnRep_Medkit75)
    FGameplayAttributeData Medkit75;
    ATTRIBUTE_ACCESSORS(UCarAttributeSet, Medkit75)
        UFUNCTION()
    void OnRep_Medkit75(const FGameplayAttributeData& OldMedkit75);

    // Nitro
    UPROPERTY(BlueprintReadOnly, Category = "Inventory", ReplicatedUsing = OnRep_Nitro)
    FGameplayAttributeData Nitro;
    ATTRIBUTE_ACCESSORS(UCarAttributeSet, Nitro)
        UFUNCTION()
    void OnRep_Nitro(const FGameplayAttributeData& OldNitro);
    */


    // Required for Unreal Replication
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
