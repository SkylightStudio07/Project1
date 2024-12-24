#include "Project1EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "Components/CapsuleComponent.h"
#include "Navigation/PathFollowingComponent.h"

void AProject1EnemyAIController::BeginPlay()
{
    Super::BeginPlay();

    // 3초마다 MoveToRandomLocation 호출
    GetWorldTimerManager().SetTimer(RandomMoveTimerHandle, this, &AProject1EnemyAIController::MoveToRandomLocation, 3.0f, true);
}

void AProject1EnemyAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 디버깅용 PathFollowingComponent 상태 확인
    UPathFollowingComponent* PathFollowingComp = GetPathFollowingComponent();
    if (PathFollowingComp)
    {
        int32 Status = (int32)PathFollowingComp->GetStatus();
        UE_LOG(LogTemp, Warning, TEXT("PathFollowingComponent Status: %d"), Status);
    }
}

void AProject1EnemyAIController::MoveToRandomLocation()
{
    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (!NavSys)
    {
        UE_LOG(LogTemp, Error, TEXT("Navigation System is NULL."));
        return;
    }

    FVector Origin = GetPawn()->GetActorLocation();
    FNavLocation RandomPoint;
    float Radius = 1000.0f;

    if (NavSys->GetRandomReachablePointInRadius(Origin, Radius, RandomPoint))
    {
        UE_LOG(LogTemp, Warning, TEXT("Random Point is valid on NavMesh: %s"), *RandomPoint.Location.ToString());

        // 이동 명령 실행
        EPathFollowingRequestResult::Type Result = MoveToLocation(RandomPoint.Location, 1.0f); // AcceptanceRadius = 1.0f
        if (Result == EPathFollowingRequestResult::RequestSuccessful)
        {
            UE_LOG(LogTemp, Warning, TEXT("MoveToLocation Successful: %s"), *RandomPoint.Location.ToString());
        }
        else if (Result == EPathFollowingRequestResult::Failed)
        {
            UE_LOG(LogTemp, Error, TEXT("MoveToLocation Failed: %s"), *RandomPoint.Location.ToString());
        }
        else if (Result == EPathFollowingRequestResult::AlreadyAtGoal)
        {
            UE_LOG(LogTemp, Warning, TEXT("Already at Location: %s"), *RandomPoint.Location.ToString());
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find a random reachable point on NavMesh."));
    }

    // 이동 속도 확인 및 설정
    UCharacterMovementComponent* MovementComp = GetPawn()->FindComponentByClass<UCharacterMovementComponent>();
    if (MovementComp)
    {
        UE_LOG(LogTemp, Warning, TEXT("AI Max Walk Speed: %f"), MovementComp->MaxWalkSpeed);
        MovementComp->MaxWalkSpeed = 300.0f;
    }

    // PathFollowingComponent 상태 초기화
    UPathFollowingComponent* PathFollowingComp = GetPathFollowingComponent();
    if (PathFollowingComp)
    {
        PathFollowingComp->AbortMove(*this, FPathFollowingResultFlags::None);
        UE_LOG(LogTemp, Warning, TEXT("PathFollowingComponent reset."));
    }
}


