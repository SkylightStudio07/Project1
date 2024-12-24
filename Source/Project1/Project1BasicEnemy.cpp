#include "Project1BasicEnemy.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"

AProject1BasicEnemy::AProject1BasicEnemy()
{
    PrimaryActorTick.bCanEverTick = true;
    EnemyMoveSpeed = 50.0f;
}

void AProject1BasicEnemy::BeginPlay()
{
    Super::BeginPlay();

    // 플레이어 캐릭터 참조
    PlayerActor = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

    if (!PlayerActor)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerActor not found!"));
    }

    if (PlayerActor) {
        UCharacterMovementComponent* MovementComp = FindComponentByClass<UCharacterMovementComponent>();
        if (MovementComp)
        {
            MovementComp->MaxWalkSpeed = EnemyMoveSpeed;
            UE_LOG(LogTemp, Warning, TEXT("AI Max Walk Speed set to: %f"), MovementComp->MaxWalkSpeed);
        }
    }
}

void AProject1BasicEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (PlayerActor)
    {
        MoveToPlayer();
    }
}

void AProject1BasicEnemy::MoveToPlayer()
{
    AAIController* AIController = Cast<AAIController>(GetController());
    if (!AIController)
    {
        UE_LOG(LogTemp, Error, TEXT("AIController is NULL."));
        return;
    }

    // 플레이어 위치로 이동
    FVector PlayerLocation = PlayerActor->GetActorLocation();
    EPathFollowingRequestResult::Type Result = AIController->MoveToLocation(PlayerLocation, 1.0f); // AcceptanceRadius = 5.0f

    if (Result == EPathFollowingRequestResult::Failed)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to move to player."));
    }
    else if (Result == EPathFollowingRequestResult::AlreadyAtGoal)
    {
        UE_LOG(LogTemp, Warning, TEXT("Already at player location: %s"), *PlayerLocation.ToString());
    }
}
