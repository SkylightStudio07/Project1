// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include "Project1Enemy.h"

UEnemyAnimInstance::UEnemyAnimInstance()
{
    // Default constructor
}

void UEnemyAnimInstance::NativeInitializeAnimation()
{
    // Get references to the owning pawn and enemy
    OwningPawn = TryGetPawnOwner();
    if (OwningPawn)
    {
        OwningEnemy = Cast<AProject1Enemy>(OwningPawn);
    }
}


void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    if (!OwningEnemy) return;

    // Update animation parameters based on the owning enemy's properties
    // For example, you can access EnemyMoveSpeed from OwningEnemy and update animations accordingly
    if (OwningEnemy->EnemyMoveSpeed == 50.0f)
    {
        // Play the zombiescream animation
        // Assuming you have a function to play animations in your enemy class
        OwningEnemy->PlayScreamAnimation();
    }
}