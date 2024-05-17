// Fill out your copyright notice in the Description page of Project Settings.


#include "WanderingEnemy.h"

AWanderingEnemy::AWanderingEnemy()
{
    UpdateInterval = 5.0f;
    WanderRadius = 500.0f;
}

// Called when the game starts or when spawned
void AWanderingEnemy::BeginPlay()
{
    Super::BeginPlay();

    GetWorldTimerManager().SetTimer(UpdateTimerHandle, this, &AWanderingEnemy::UpdateTargetLocation, UpdateInterval, true);
}

// Called every frame
void AWanderingEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!IsChasing)
    {
        // Calculate direction to the target location
        FVector Direction = (TargetLocation - GetActorLocation()).GetSafeNormal();

        // Rotate the enemy to face the direction of movement
        FRotator NewRotation = Direction.Rotation();
        SetActorRotation(NewRotation);

        // Move towards the target location
        MoveToTarget(TargetLocation);

        if (AnimInstance)
        {
            AnimInstance->IsMoving = true; // 이동 중임을 설정
        }
    }
}

// Update target location for wandering
void AWanderingEnemy::UpdateTargetLocation()
{
    // Generate a random location within the wander radius
    FVector NewLocation = GetActorLocation() + FVector(FMath::FRandRange(-WanderRadius, WanderRadius), FMath::FRandRange(-WanderRadius, WanderRadius), 0.0f);

    // Set the new target location
    TargetLocation = NewLocation;
}
