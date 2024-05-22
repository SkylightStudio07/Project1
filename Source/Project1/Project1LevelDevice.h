#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelTransferVolume.h"
#include "Project1LevelDevice.generated.h"

UENUM(BlueprintType)
enum class LevelDV : uint8
{
    World2Opening UMETA(DisplayName = "World2Opening"),
    World2Progress1 UMETA(DisplayName = "World2Progress1"),
    World2Progress2 UMETA(DisplayName = "World2Progress2"),
    World2Progress3 UMETA(DisplayName = "World2Progress3"),
    World2Progress4 UMETA(DisplayName = "World2Progress4"),
    World2Progress5 UMETA(DisplayName = "World2Progress5"),
    DoorLockPick UMETA(DisplayName = "DoorLockPick"),
    DoorOpener UMETA(DisplayName = "DoorOpener")
};

UCLASS()
class PROJECT1_API AProject1LevelDevice : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AProject1LevelDevice();

    void DeactivateDevice();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
        LevelDV LevelDVStat;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
        class AProject1Character* Player;



private:
    // 트리거 박스 컴포넌트
    UPROPERTY(VisibleAnywhere, Category = "Components")
        class UBoxComponent* TriggerBox;

    // 트리거 진입 이벤트
    UFUNCTION()
        void OnTriggerBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
            bool bFromSweep, const FHitResult& SweepResult);

    // 타이머 핸들
    FTimerHandle TimerHandle;

    // 이벤트 플래그
    bool bEventTriggered;

    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    void SpawnLevelTransferVolume();

    UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
        TSubclassOf<ALevelTransferVolume> LevelTransferVolumeClass;

    bool bIsActive;


};