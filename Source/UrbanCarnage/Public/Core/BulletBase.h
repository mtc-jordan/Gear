#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletBase.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class URBANCARNAGE_API ABulletBase : public AActor
{
	GENERATED_BODY()

public:
	ABulletBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Bullet")
	float InitialSpeed = 30000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Bullet")
	float MaxSpeed = 30000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Bullet")
	float LifeSeconds = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Bullet")
	float Damage = 10.0f;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);

public:
	virtual void Tick(float DeltaSeconds) override;
};
