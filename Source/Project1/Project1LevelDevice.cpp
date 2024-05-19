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

    // 이벤트 플래그 초기화
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
    // 이벤트가 이미 실행되었는지 확인합니다.
    if (bEventTriggered)
    {
        return;
    }

    // 플레이어 캐릭터가 트리거 박스에 진입했는지 확인합니다.
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

            // 대사 출력
            PlayerHUD->DisplayDialog(FText::FromString("The door lock pick is ready!"));
        }
        else if (LevelDVStat == LevelDV::DoorOpener && GameMode->IsDoorReadyToOpen)
        {
            // 이벤트가 실행되었음을 표시
            bEventTriggered = true;

            // 대사 출력
            PlayerHUD->DisplayDialog(FText::FromString("The door is ready to open!"));

            // 30초 후에 MyLevelTransferVolumeBlueprint를 생성하는 타이머 설정
            GetWorldTimerManager().SetTimer(TimerHandle, this, &AProject1LevelDevice::SpawnLevelTransferVolume, 30.0f, false);
        }
    }
}

void AProject1LevelDevice::SpawnLevelTransferVolume()
{
    // MyLevelTransferVolumeBlueprint를 생성합니다.
    static ConstructorHelpers::FClassFinder<AActor> LevelTransferVolumeBP(TEXT("/Game/Blueprints/MyLevelTransferVolumeBlueprint"));
    if (LevelTransferVolumeBP.Succeeded())
    {
        FActorSpawnParameters SpawnParams;
        GetWorld()->SpawnActor<AActor>(LevelTransferVolumeBP.Class, GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
    }
}