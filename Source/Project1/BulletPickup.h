// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletPickup.generated.h"

UCLASS()
class PROJECT1_API ABulletPickup : public AActor
{
	GENERATED_BODY()
	
public:
    ABulletPickup();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
        class USphereComponent* CollisionComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
        int32 BulletAmount;

    UFUNCTION()
        void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
            const FHitResult& SweepResult);
};