// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Project1GameMode.h"
#include "EnemyAnimInstance.generated.h"

UCLASS()
class PROJECT1_API UEnemyAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    // Constructor
    UEnemyAnimInstance();

    // Called when the animation begins
    virtual void NativeInitializeAnimation() override;

    // Called every frame
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
        bool IsMoving;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
        bool IsAlert;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
        bool IsAttacking;

    UFUNCTION(BlueprintCallable, Category = "Animation")
        void GetCurrentWorldStatus_Idle();

protected:
    // Reference to the owning pawn
    UPROPERTY(BlueprintReadOnly, Category = "Enemy")
        APawn* OwningPawn;

    // Reference to the owning character
    UPROPERTY(BlueprintReadOnly, Category = "Enemy")
        class AProject1Enemy* OwningEnemy;



};