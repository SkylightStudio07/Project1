// Fill out your copyright notice in the Description page of Project Settings.


#include "Project1LevelDevice.h"
#include "Components/BoxComponent.h"
#include "Project1Character.h"
#include "Project1GameMode.h"
#include "Engine/World.h"

// Sets default values
AProject1LevelDevice::AProject1LevelDevice()
{
    // 트리거 박스 컴포넌트를 생성하고 설정합니다.
    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
    TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    // 트리거 박스 크기 설정
    TriggerBox->InitBoxExtent(FVector(200.0f, 200.0f, 100.0f));
    RootComponent = TriggerBox;

    // 트리거 진입 이벤트를 바인딩합니다.
    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AProject1LevelDevice::OnTriggerBoxOverlapBegin);
}

// Called when the game starts or when spawned
void AProject1LevelDevice::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void AProject1LevelDevice::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AProject1LevelDevice::OnTriggerBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    // 플레이어 캐릭터가 트리거 박스에 진입했는지 확인합니다.
    AProject1Character* PlayerCharacter = Cast<AProject1Character>(OtherActor);
    if (PlayerCharacter)
    {
        if (LevelDVStat == LevelDV::DoorLockPick) {

            // GameMode를 가져와서 IsDoorReadyToOpen 변수를 true로 설정합니다.
            AProject1GameMode* GameMode = Cast<AProject1GameMode>(GetWorld()->GetAuthGameMode());
            if (GameMode)
            {
                GameMode->IsDoorReadyToOpen = true;
            }
        }
        if (LevelDVStat == LevelDV::DoorOpener) {

            // GameMode를 가져와서 IsDoorReadyToOpen 변수를 true로 설정합니다.
            AProject1GameMode* GameMode = Cast<AProject1GameMode>(GetWorld()->GetAuthGameMode());
            if (GameMode)
            {
                if (GameMode->IsDoorReadyToOpen == true) {

                }
            }
        }
    }
}