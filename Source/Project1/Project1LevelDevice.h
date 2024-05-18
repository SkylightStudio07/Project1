// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Project1LevelDevice.generated.h"

UENUM(BlueprintType)
enum class LevelDV : uint8
{
    DoorLockPick UMETA(DisplayName = "DoorLockPick"),
    DoorOpener UMETA(DisplayName = "DoorOpener"),
};

UCLASS()
class PROJECT1_API AProject1LevelDevice : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AProject1LevelDevice();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
        LevelDV LevelDVStat;

private:
    // 트리거 박스 컴포넌트
    UPROPERTY(VisibleAnywhere, Category = "Components")
        class UBoxComponent* TriggerBox;

    // 트리거 진입 이벤트
    UFUNCTION()
        void OnTriggerBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
            bool bFromSweep, const FHitResult& SweepResult);


};