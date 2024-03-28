// Project1Projectile.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h" // Sphere 컴포넌트 헤더 추가
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

    // Collision component.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
        USphereComponent* CollisionComponent; // RootComponent로 사용할 구체 컴포넌트

        // Function to handle projectile collision.

    UFUNCTION()
        void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

    // Damage amount to apply to the enemy
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
        float DamageAmount;

};