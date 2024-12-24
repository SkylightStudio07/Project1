#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Project1EnemyAIController.generated.h"

/**
 * Custom AI Controller for Enemy AI
 */
UCLASS()
class PROJECT1_API AProject1EnemyAIController : public AAIController
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

private:
    void MoveToRandomLocation(); // 무작위 위치로 이동

    FTimerHandle RandomMoveTimerHandle; // 타이머 핸들
};
