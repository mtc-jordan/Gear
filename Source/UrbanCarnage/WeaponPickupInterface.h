#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponPickupInterface.generated.h"

// Forward declaration of AActor to avoid circular dependencies
class AActor;

// Declare the Interface
UINTERFACE(Blueprintable)
class URBANCARNAGE_API UWeaponPickupInterface : public UInterface
{
    GENERATED_BODY()
};

class URBANCARNAGE_API IWeaponPickupInterface
{
    GENERATED_BODY()

public:
   
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon")
    void OnWeaponPickedUp(int32 WeaponID, const FString& WeaponName, const FString& WeaponType, AActor* PickupActor);
};
