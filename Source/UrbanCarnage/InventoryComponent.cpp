// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


void UInventoryComponent::AddItem(FName ItemID, int32 Amount)
{
    if (!GetOwner()->HasAuthority()) return;

    for (FInventoryItem& Item : Inventory)
    {
        if (Item.ItemID == ItemID)
        {
            Item.Quantity += Amount;
            return;
        }
    }

    Inventory.Add(FInventoryItem(ItemID, Amount));
}

bool UInventoryComponent::ConsumeItem(FName ItemID, int32 Amount)
{
    if (!GetOwner()->HasAuthority()) return false;

    for (FInventoryItem& Item : Inventory)
    {
        if (Item.ItemID == ItemID && Item.Quantity >= Amount)
        {
            Item.Quantity -= Amount;
            if (Item.Quantity <= 0)
            {
                Inventory.Remove(Item);
            }
            return true;
        }
    }
    return false;
}

int32 UInventoryComponent::GetItemQuantity(FName ItemID) const
{
    for (const FInventoryItem& Item : Inventory)
    {
        if (Item.ItemID == ItemID)
        {
            return Item.Quantity;
        }
    }
    return 0;
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UInventoryComponent, Inventory);
}