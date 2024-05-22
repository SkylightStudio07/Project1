// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemySpawner.h"
#include "EnemySpawner_Trigger.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT1_API AEnemySpawner_Trigger : public AEnemySpawner
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;

	AEnemySpawner_Trigger();

private:


};
