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
    void MoveToRandomLocation(); // ������ ��ġ�� �̵�

    FTimerHandle RandomMoveTimerHandle; // Ÿ�̸� �ڵ�
};
