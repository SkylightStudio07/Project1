// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"
#include "Project1Enemy.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
    // Set this actor to call Tick() every frame
    PrimaryActorTick.bCanEverTick = true;

    // Set default values for spawn interval and current spawn time
    SpawnInterval = 3.0f;
    CurrentSpawnTime = 0.0f;
}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Check if the game mode is in danger status
    AProject1GameMode* GameMode = GetWorld()->GetAuthGameMode<AProject1GameMode>();
    if (GameMode && GameMode->CurrentWorldStatus == WorldStatus::Warning)
    {
        // Increase the current spawn time
        CurrentSpawnTime += DeltaTime;

        // Check if it's time to spawn a new enemy
        if (CurrentSpawnTime >= SpawnInterval)
        {
            // Reset the current spawn time
            CurrentSpawnTime = 0.0f;

            // Spawn a new enemy
            FVector SpawnLocation = GetActorLocation();
            FRotator SpawnRotation = GetActorRotation();
            GetWorld()->SpawnActor<AProject1Enemy>(EnemyClass, SpawnLocation, SpawnRotation);
        }
    }
}