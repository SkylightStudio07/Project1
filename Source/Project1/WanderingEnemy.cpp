// Fill out your copyright notice in the Description page of Project Settings.


#include "WanderingEnemy.h"

AWanderingEnemy::AWanderingEnemy()
{
    // Set default values
    UpdateInterval = 5.0f;
    WanderRadius = 500.0f;
}

// Called when the game starts or when spawned
void AWanderingEnemy::BeginPlay()
{
    Super::BeginPlay();

    // Start timer to update target location
    GetWorldTimerManager().SetTimer(UpdateTimerHandle, this, &AWanderingEnemy::UpdateTargetLocation, UpdateInterval, true);
}

// Called every frame
void AWanderingEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Move towards the target location
    MoveToTarget(TargetLocation);
}

// Update target location for wandering
void AWanderingEnemy::UpdateTargetLocation()
{
    // Generate a random location within the wander radius
    FVector NewLocation = GetActorLocation() + FVector(FMath::FRandRange(-WanderRadius, WanderRadius), FMath::FRandRange(-WanderRadius, WanderRadius), 0.0f);

    // Set the new target location
    TargetLocation = NewLocation;
}
