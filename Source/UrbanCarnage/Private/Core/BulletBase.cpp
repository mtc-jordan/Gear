#include "Core/BulletBase.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/Engine.h"

ABulletBase::ABulletBase()
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	Mesh->SetNotifyRigidBodyCollision(true);
	Mesh->SetGenerateOverlapEvents(true);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->InitialSpeed = InitialSpeed;
	ProjectileMovementComponent->MaxSpeed = MaxSpeed;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
}

void ABulletBase::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSeconds);
	if (ProjectileMovementComponent)
	{
		ProjectileMovementComponent->Velocity = GetActorForwardVector() * InitialSpeed;
	}
	if (Mesh)
	{
		Mesh->OnComponentHit.AddDynamic(this, &ABulletBase::OnHit);
	}
}

void ABulletBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	// Minimal behavior: destroy on any hit
	Destroy();
}

void ABulletBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
