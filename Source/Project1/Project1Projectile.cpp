// Fill out your copyright notice in the Description page of Project Settings.

#include "Project1Projectile.h"
#include "Components/SphereComponent.h"

// Sets default values
AProject1Projectile::AProject1Projectile()
{
    // Set this actor to call Tick() every frame
    PrimaryActorTick.bCanEverTick = true;

    // Create a sphere component as the root component
    USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
    RootComponent = SphereComponent;
    SphereComponent->InitSphereRadius(5.0f); // Adjust the radius as needed

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

// Function to initialize the projectile's velocity in the shoot direction.
void AProject1Projectile::FireInDirection(const FVector& ShootDirection)
{
    ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}

void AProject1Projectile::FireInDirection(const FVector& ShootDirection)
{
    // 총알의 방향을 설정합니다.
    SetActorRotation(ShootDirection.Rotation());

    // 총알의 속도를 설정합니다.
    ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}