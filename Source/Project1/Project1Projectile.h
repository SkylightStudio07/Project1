// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Project1Projectile.generated.h"

UCLASS()
class PROJECT1_API AProject1Projectile : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AProject1Projectile();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Function to initialize the projectile's velocity in the shoot direction.
    void FireInDirection(const FVector& ShootDirection);

    // Projectile movement component.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
        UProjectileMovementComponent* ProjectileMovementComponent;

    void FireInDirection(const FVector& ShootDirection);

};