// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner_Trigger.h"

AEnemySpawner_Trigger::AEnemySpawner_Trigger() {

    PrimaryActorTick.bCanEverTick = true;

    SpawnInterval = 3.0f;
    CurrentSpawnTime = 0.0f;
    SpawnCount = 0;

    IsTriggerSpawn = true;
}

void AEnemySpawner_Trigger::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    AProject1GameMode* GameMode = GetWorld()->GetAuthGameMode<AProject1GameMode>();
    if (GameMode && GameMode->GetIsTriggerSpawn())
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