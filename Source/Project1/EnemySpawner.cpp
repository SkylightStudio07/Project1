// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemySpawner.h"
#include "Project1Enemy.h"

AEnemySpawner::AEnemySpawner()
{
    // Set this actor to call Tick() every frame
    PrimaryActorTick.bCanEverTick = true;

    SpawnInterval = 3.0f;
    CurrentSpawnTime = 0.0f;
    SpawnCount = 0;
    IsTriggerSpawn = false;
}

void AEnemySpawner::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!IsTriggerSpawn) {
        AProject1GameMode* GameMode = GetWorld()->GetAuthGameMode<AProject1GameMode>();
        if (GameMode && GameMode->CurrentWorldStatus == WorldStatus::Warning)
        {
            CurrentSpawnTime += DeltaTime;
            if (CurrentSpawnTime >= SpawnInterval && SpawnCount < MaxSpawnCount)
            {
                CurrentSpawnTime = 0.0f;

                FVector SpawnLocation = GetActorLocation();
                FRotator SpawnRotation = GetActorRotation();
                GetWorld()->SpawnActor<AProject1Enemy>(EnemyClass, SpawnLocation, SpawnRotation);

                SpawnCount++;
            }
        }
    }
}