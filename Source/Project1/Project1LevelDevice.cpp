// Fill out your copyright notice in the Description page of Project Settings.


#include "Project1LevelDevice.h"
#include "Components/BoxComponent.h"
#include "Project1Character.h"
#include "Project1GameMode.h"
#include "Engine/World.h"

// Sets default values
AProject1LevelDevice::AProject1LevelDevice()
{
    // Ʈ���� �ڽ� ������Ʈ�� �����ϰ� �����մϴ�.
    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
    TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    // Ʈ���� �ڽ� ũ�� ����
    TriggerBox->InitBoxExtent(FVector(200.0f, 200.0f, 100.0f));
    RootComponent = TriggerBox;

    // Ʈ���� ���� �̺�Ʈ�� ���ε��մϴ�.
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
    // �÷��̾� ĳ���Ͱ� Ʈ���� �ڽ��� �����ߴ��� Ȯ���մϴ�.
    AProject1Character* PlayerCharacter = Cast<AProject1Character>(OtherActor);
    if (PlayerCharacter)
    {
        if (LevelDVStat == LevelDV::DoorLockPick) {

            // GameMode�� �����ͼ� IsDoorReadyToOpen ������ true�� �����մϴ�.
            AProject1GameMode* GameMode = Cast<AProject1GameMode>(GetWorld()->GetAuthGameMode());
            if (GameMode)
            {
                GameMode->IsDoorReadyToOpen = true;
            }
        }
        if (LevelDVStat == LevelDV::DoorOpener) {

            // GameMode�� �����ͼ� IsDoorReadyToOpen ������ true�� �����մϴ�.
            AProject1GameMode* GameMode = Cast<AProject1GameMode>(GetWorld()->GetAuthGameMode());
            if (GameMode)
            {
                if (GameMode->IsDoorReadyToOpen == true) {

                }
            }
        }
    }
}