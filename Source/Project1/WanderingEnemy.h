// WanderingEnemy.h

#pragma once

#include "CoreMinimal.h"
#include "Project1Enemy.h"
#include "WanderingEnemy.generated.h"

UCLASS()
class PROJECT1_API AWanderingEnemy : public AProject1Enemy
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AWanderingEnemy();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

private:
    // Interval between updating target location
    UPROPERTY(EditAnywhere, Category = "Wandering")
        float UpdateInterval = 5.0f;

    // Radius of the wandering area
    UPROPERTY(EditAnywhere, Category = "Wandering")
        float WanderRadius = 500.0f;

    // Timer handle for updating target location
    FTimerHandle UpdateTimerHandle;

    // Update target location for wandering
    void UpdateTargetLocation();
};
