// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct FInventoryItem
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    FName ItemID;  // Unique Item Name (e.g., "Medkit", "Nitro", "Shield")

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 Quantity;  // Amount of this item

    FInventoryItem()
        : ItemID(NAME_None), Quantity(0) {
    }

    FInventoryItem(FName InItemID, int32 InQuantity)
        : ItemID(InItemID), Quantity(InQuantity) {
    }
    bool operator==(const FInventoryItem& Other) const
    {
        return ItemID == Other.ItemID && Quantity == Other.Quantity;
    }
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class URBANCARNAGE_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInventoryComponent();

protected:
    virtual void BeginPlay() override;

public:
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AddItem(FName ItemID, int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool ConsumeItem(FName ItemID, int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 GetItemQuantity(FName ItemID) const;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Inventory")
    TArray<FInventoryItem> Inventory;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};