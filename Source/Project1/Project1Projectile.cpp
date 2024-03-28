// Project1Projectile.cpp

#include "Project1Projectile.h"
#include "Project1Enemy.h"
#include "Components/SphereComponent.h"

// Sets default values
AProject1Projectile::AProject1Projectile()
{
    // Set this actor to call Tick() every frame
    PrimaryActorTick.bCanEverTick = true;

    // Create a sphere component as the root component
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    CollisionComponent->InitSphereRadius(15.0f);
    RootComponent = CollisionComponent; // Adjust the radius as needed

    // Set collision profile for the sphere component
    CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
    // Create a static mesh component
    UStaticMeshComponent* MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(RootComponent); // Attach the mesh component to the root component

    // Load the mesh from file
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/MilitaryWeapSilver/Weapons/AssaultRifleA_Ammo.AssaultRifleA_Ammo"));
    if (MeshAsset.Succeeded())
    {
        MeshComponent->SetStaticMesh(MeshAsset.Object);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load mesh!"));
    }

    // Create a projectile movement component
    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
    ProjectileMovementComponent->UpdatedComponent = RootComponent;
    ProjectileMovementComponent->InitialSpeed = 3000.f;
    ProjectileMovementComponent->MaxSpeed = 3000.f;
    ProjectileMovementComponent->bRotationFollowsVelocity = true;
    ProjectileMovementComponent->bShouldBounce = false;

    // Set up collision function to handle projectile hits
    CollisionComponent->OnComponentHit.AddDynamic(this, &AProject1Projectile::OnHit);
    // Set default damage amount
    DamageAmount = 50.0f; // Adjust as needed
}

// Called when the game starts or when spawned
void AProject1Projectile::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void AProject1Projectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AProject1Projectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    FString OtherActorName = OtherActor ? OtherActor->GetName() : FString(TEXT("Unknown"));

    // Log the name of the other actor
    UE_LOG(LogTemp, Warning, TEXT("Projectile has hit an enemy: %s"), *OtherActorName);
    if (OtherActor != this)
    {
        UE_LOG(LogTemp, Warning, TEXT("Best!"));
        AProject1Enemy* Enemy = Cast<AProject1Enemy>(OtherActor);
        // Apply damage to the enemy
        if (Enemy) {
            Enemy->TakeDamage(DamageAmount, FDamageEvent(), GetInstigatorController(), this);
        }

        // Destroy the projectile upon hitting the enemy
        Destroy();
    }
}

void AProject1Projectile::FireInDirection(const FVector& ShootDirection)
{
    // Set the direction of the projectile
    SetActorRotation(ShootDirection.Rotation());

    // Set the velocity of the projectile
    ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}