#pragma once

#include "UObject/Interface.h"
#include "DamageInterface_BASE.generated.h"

UINTERFACE(BlueprintType)
class URBANCARNAGE_API UDamageInterface_BASE : public UInterface
{
	GENERATED_BODY()
};

class URBANCARNAGE_API IDamageInterface_BASE
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Damage")
	void Death();
};
