// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Project1GameMode.h"
#include "Project1Enemy.h"
#include "EnemySpawner.generated.h"

UCLASS()
class PROJECT1_API AEnemySpawner : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AEnemySpawner();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

private:
    UPROPERTY(EditAnywhere)
        TSubclassOf<AProject1Enemy> EnemyClass;

    float SpawnInterval;
    float CurrentSpawnTime;
};