// Fill out your copyright notice in the Description page of Project Settings.


#include "CarAttributeSet.h"


#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "GameplayTagContainer.h"
#include "UrbanCarnagePawn.h"
#include "Core/DamageInterface_BASE.h"

UCarAttributeSet::UCarAttributeSet()
{
    // Default values for attributes
    Health = FGameplayAttributeData(100.0f);
	Shield = FGameplayAttributeData(30.0f);
        //Medkit25 = FGameplayAttributeData(0.0f);
        //Medkit75 = FGameplayAttributeData(0.0f);
        //Nitro = FGameplayAttributeData(0.0f);


}
void UCarAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        const float NewHealth = Health.GetCurrentValue();
        if (NewHealth <= 0.0f)
        {
            AActor* Owner = GetOwningActor();
            if (Owner && Owner->HasAuthority())
            {
                //check if owner has damage interface and Call event death
                if (Owner && Owner->Implements<UDamageInterface_BASE>())
                {
                    IDamageInterface_BASE::Execute_Death(Owner);
                }
                else
                {
                    // Handle the case where the owner does not implement the interface
                    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Owner does not implement IDamageInterface_BASE"));
                }
            }
           
        }
    }
}


void UCarAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{

    
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCarAttributeSet, Health, OldHealth);
   //print current health to screen
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Health: %f"), Health.GetCurrentValue()));
    }
    
    if (Health.GetCurrentValue() <= 0.0f)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Health is 0"));
        AActor* Owner = GetOwningActor();
        if (Owner&& Owner->HasAuthority())
        {
            //cast to Urbancarnagepawn and call the death function
             AUrbanCarnagePawn* Pawn = Cast<AUrbanCarnagePawn>(Owner);
            if (Pawn) {
          //      Pawn->Death();
            }

        }
    }
}
void UCarAttributeSet::OnRep_Shield(const FGameplayAttributeData& OldShield)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCarAttributeSet, Shield, OldShield);
    if (Shield.GetCurrentValue() <= 0.0f)
    {
        AActor* Owner = GetOwningActor();
        if (Owner)
        {
            UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);
            if (ASC)
            {
                // Remove the shield tag
                FGameplayTag ShieldTag = FGameplayTag::RequestGameplayTag(FName("Ability.Shield"));
                ASC->RemoveLooseGameplayTag(ShieldTag);
                


            }
            
        }
		
    }
}
/*
void UCarAttributeSet::OnRep_Medkit25(const FGameplayAttributeData& OldMedkit25)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCarAttributeSet, Medkit25, OldMedkit25);
}

void UCarAttributeSet::OnRep_Medkit75(const FGameplayAttributeData& OldMedkit75)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCarAttributeSet, Medkit75, OldMedkit75);
}

void UCarAttributeSet::OnRep_Nitro(const FGameplayAttributeData& OldNitro)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCarAttributeSet, Nitro, OldNitro);
}
*/



void UCarAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UCarAttributeSet, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UCarAttributeSet, Shield, COND_None, REPNOTIFY_Always);
     // DOREPLIFETIME_CONDITION_NOTIFY(UCarAttributeSet, Medkit75, COND_None, REPNOTIFY_Always);
     // DOREPLIFETIME_CONDITION_NOTIFY(UCarAttributeSet, Medkit25, COND_None, REPNOTIFY_Always);
      //DOREPLIFETIME_CONDITION_NOTIFY(UCarAttributeSet, Nitro, COND_None, REPNOTIFY_Always);

}