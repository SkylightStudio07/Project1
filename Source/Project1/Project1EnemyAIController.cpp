// Fill out your copyright notice in the Description page of Project Settings.


#include "Project1EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "Project1Character.h"

void AProject1EnemyAIController::BeginPlay()
{
    Super::BeginPlay();
    GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AProject1EnemyAIController::ChasePlayer);
}

void AProject1EnemyAIController::ChasePlayer()
{
    AProject1Character* PlayerCharacter = Cast<AProject1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (PlayerCharacter)
    {
        MoveToActor(PlayerCharacter, 5.0f, true);  // 지속적으로 추적
    }

    // 1초마다 다시 추적 명령을 실행
    GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AProject1EnemyAIController::ChasePlayer);
}