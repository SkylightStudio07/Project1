#include "Project1LevelDevice.h"
#include "Components/BoxComponent.h"
#include "Project1Character.h"
#include "Project1GameMode.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

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

    // �̺�Ʈ �÷��� �ʱ�ȭ
    bEventTriggered = false;
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
    // �̺�Ʈ�� �̹� ����Ǿ����� Ȯ���մϴ�.
    if (bEventTriggered)
    {
        return;
    }

    // �÷��̾� ĳ���Ͱ� Ʈ���� �ڽ��� �����ߴ��� Ȯ���մϴ�.
    AProject1Character* PlayerCharacter = Cast<AProject1Character>(OtherActor);
    if (PlayerCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("Player Entered Level Device Trigger"));


        AProject1GameMode* GameMode = Cast<AProject1GameMode>(GetWorld()->GetAuthGameMode());
        if (!GameMode)
        {
            UE_LOG(LogTemp, Warning, TEXT("Cannot Find GameMode -> LevelDevice"));
            return;
        }

        UPlayerHUD* PlayerHUD = PlayerCharacter->GetPlayerHUD();
        if (!PlayerHUD)
        {
            UE_LOG(LogTemp, Warning, TEXT("Cannot Find PlayerHUD -> LevelDevice"));
            return;
        }

        if (LevelDVStat == LevelDV::DoorLockPick)
        {
            GameMode->IsDoorReadyToOpen = true;

            // ��� ���
            PlayerHUD->DisplayDialog(FText::FromString("The door lock pick is ready!"));
        }
        else if (LevelDVStat == LevelDV::DoorOpener && GameMode->IsDoorReadyToOpen)
        {
            // �̺�Ʈ�� ����Ǿ����� ǥ��
            bEventTriggered = true;

            // ��� ���
            PlayerHUD->DisplayDialog(FText::FromString("The door is ready to open!"));

            // 30�� �Ŀ� MyLevelTransferVolumeBlueprint�� �����ϴ� Ÿ�̸� ����
            GetWorldTimerManager().SetTimer(TimerHandle, this, &AProject1LevelDevice::SpawnLevelTransferVolume, 30.0f, false);
        }
    }
}

void AProject1LevelDevice::SpawnLevelTransferVolume()
{
    // MyLevelTransferVolumeBlueprint�� �����մϴ�.
    static ConstructorHelpers::FClassFinder<AActor> LevelTransferVolumeBP(TEXT("/Game/Blueprints/MyLevelTransferVolumeBlueprint"));
    if (LevelTransferVolumeBP.Succeeded())
    {
        FActorSpawnParameters SpawnParams;
        GetWorld()->SpawnActor<AActor>(LevelTransferVolumeBP.Class, GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
    }
}