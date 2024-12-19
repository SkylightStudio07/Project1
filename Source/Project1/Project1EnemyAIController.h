// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Project1EnemyAIController.generated.h"

UCLASS()
class PROJECT1_API AProject1EnemyAIController : public AAIController
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    // 플레이어를 추적하는 함수
    void ChasePlayer();
};