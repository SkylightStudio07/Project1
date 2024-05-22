#include "Project1LevelDevice.h"
#include "Components/BoxComponent.h"
#include "Project1Character.h"
#include "Project1GameMode.h"
#include "Engine/World.h"
#include "LevelTransferVolume.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProject1LevelDevice::AProject1LevelDevice()
{
    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
    TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    // 트리거 크기
    TriggerBox->InitBoxExtent(FVector(200.0f, 200.0f, 100.0f));
    RootComponent = TriggerBox;

    // 트리거 이벤트 바인딩
    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AProject1LevelDevice::OnTriggerBoxOverlapBegin);

    // 이벤트 플래그 초기화
    bEventTriggered = false;

    bIsActive = true;

    static ConstructorHelpers::FClassFinder<ALevelTransferVolume> LevelTransferVolumeBP(TEXT("/Game/Blueprints/MyLevelTransferVolumeBlueprint.MyLevelTransferVolumeBlueprint_C"));
    if (LevelTransferVolumeBP.Succeeded())
    {
        LevelTransferVolumeClass = LevelTransferVolumeBP.Class;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find LevelTransferVolumeBP class"));
    }

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

        FString KoreanMessage;

        
        // 여기서부터는 다이얼로그와 해당 상황에 대응하는 기타 처리를 관리
        switch (LevelDVStat)
        {
        case LevelDV::World2Opening:
            KoreanMessage = FString(TEXT("오퍼레이터 : 더럽게 어둡군.... 빌어먹을.\n감염체가 얼마나 있을지 모른다. 발포는 어지간하면 자제하도록. "));
            break;
        case LevelDV::World2Progress1:
            KoreanMessage = FString(TEXT("오퍼레이터 : 좌측의 감염체 2구 색적. 그리고 우측에 두 마리 더.\n다른 감염체들이 깨어나지 않도록 신속하게 처리해."));
            break;
        case LevelDV::World2Progress2:
            KoreanMessage = FString(TEXT("치프 : 이래서야 생존자 수색은 어불성설이군요. 로비까지 감염체가 들어찰 정도면...\n작전 목표가 수정되었습니다, 터커. 샘플을 확보하고 그 망할 곳에서 신속하게 빠져나오세요."));
            break;
        case LevelDV::World2Progress3:
            KoreanMessage = FString(TEXT("치프 : 저기, 아직 살아있는 조명이 보입니다.\n오퍼레이터 : 아마 지하 연구소로 향하는 문일 거다. 터커, 가까이 가볼 수 있겠나?"));
            break;
        case LevelDV::World2Progress4:
            KoreanMessage = FString(TEXT("치프 : 이 문은 잠겨있는 것 같습니다. \n오퍼레이터 : 데이터상으로는 우측에 로비 경비실이 있다. 우측으로 우회해서 데이터키를 찾아봐."));
            break;
        case LevelDV::World2Progress5:
            KoreanMessage = FString(TEXT("치프 : 저 감염체는.... 무언가 이상합니다.\n오퍼레이터 : 터커. 저 자식에게는 어지간해서는 가까이 가지 마."));
            break;
        case LevelDV::DoorLockPick:
            GameMode->IsDoorReadyToOpen = true;
            KoreanMessage = FString(TEXT("치프 : 데이터키군요. \n오퍼레이터 : 자, 신속하게 다시 문으로 움직인다."));
            break;
        case LevelDV::DoorOpener:
            if (GameMode->IsDoorReadyToOpen) {

                bEventTriggered = true;

                GameMode->SpawnTriggerEnemies();
                GameMode->SetCurrentWorldStatus(WorldStatus::Warning);

                PlayerHUD->SetTimer_World1();

                KoreanMessage = FString(TEXT("치프 : ....좋지 않군요, 이건. 문이 열릴 때까지 30초는 소요될 겁니다.\n오퍼레이터 : 빌어먹을, 경보다! 터커, 어떻게 30초만 버텨내고 문이 열리면 지하로 이동해!"));
                GetWorldTimerManager().SetTimer(TimerHandle, this, &AProject1LevelDevice::SpawnLevelTransferVolume, 30.0f, false);
            }
            break;
        default:
            KoreanMessage = FString(TEXT("다이얼로그 오류 발생!"));
            break;
        }
        PlayerHUD->DisplayDialog(FText::FromString(KoreanMessage), this);


        /*
        // 여기서부터 다이얼로그를 관리합니다.

        if (LevelDVStat == LevelDV::World2Opening)
        {
            // 대사 출력
            FString KoreanMessage = FString(TEXT("오퍼레이터 : 더럽게 어둡군.... 빌어먹을.\n감염체가 얼마나 있을 지 모른다. 발포는 어지간하면 자제하도록."));
            PlayerHUD->DisplayDialog(FText::FromString(KoreanMessage), this);
            // PlayerHUD->DisplayDialog(FText::FromString("오퍼레이터 : 더럽게 어둡군.... 빌어먹을.\n감염체가 얼마나 있을 지 모른다. 발포는 어지간하면 자제하도록."), this);
        }

        else if (LevelDVStat == LevelDV::DoorLockPick)
        {
            GameMode->IsDoorReadyToOpen = true;

            // 대사 출력
            PlayerHUD->DisplayDialog(FText::FromString("The door lock pick is ready!"), this);
        }
        else if (LevelDVStat == LevelDV::DoorOpener && GameMode->IsDoorReadyToOpen)
        {
            // 이벤트가 트리거
            bEventTriggered = true;

            // 대사 출력
            PlayerHUD->DisplayDialog(FText::FromString("The door is ready to open!"), this);

            // 30초 후에 MyLevelTransferVolumeBlueprint를 생성하는 타이머 설정
            GetWorldTimerManager().SetTimer(TimerHandle, this, &AProject1LevelDevice::SpawnLevelTransferVolume, 30.0f, false);
        }
        */
    }
}

void AProject1LevelDevice::SpawnLevelTransferVolume()
{
    if (LevelTransferVolumeClass != nullptr)
    {
        FActorSpawnParameters SpawnParams;
        AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(LevelTransferVolumeClass, GetActorLocation(), FRotator::ZeroRotator, SpawnParams);

        if (SpawnedActor)
        {
            ALevelTransferVolume* LevelTransferVolume = Cast<ALevelTransferVolume>(SpawnedActor);
            if (LevelTransferVolume)
            {
                LevelTransferVolume->SetTransferLevelName(TEXT("World1"));
                UE_LOG(LogTemp, Warning, TEXT("LevelTransferVolume spawned and set to transfer to World1"));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Spawned actor is not of type ALevelTransferVolume"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn LevelTransferVolume actor"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("LevelTransferVolumeClass is not set"));
    }
}

void AProject1LevelDevice::DeactivateDevice()
{
    bIsActive = false;
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
    SetActorTickEnabled(false);
}