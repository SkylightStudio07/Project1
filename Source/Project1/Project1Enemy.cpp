#include "Project1Enemy.h"
#include "Project1Character.h"
#include "EnemyAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Project1EnemyAIController.h"
#include "TimerManager.h"
#include "Components/WidgetComponent.h"
#include "EnemyHPWidget.h"
#include "Components/MeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Project1GameMode.h"
#include "Components/ProgressBar.h"

// Sets default values
AProject1Enemy::AProject1Enemy()
{
    // Set this character to call Tick() every frame
    PrimaryActorTick.bCanEverTick = true;

    // RecogDistance = 300.0f; // 인지범위. 초기 상태는 300
    EnemyMoveSpeed = 200.0f;
    EnemyHP = 100.0f;
    MaxHP = EnemyHP;
    FieldOfView = 60.0f;
    IsChasing = false;
    isRecognizingPlayer = false;

    // RootComponent 설정
    RootComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RootComponent"));

    // 스켈레탈 메쉬
    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
    SkeletalMesh->SetupAttachment(RootComponent);
    SkeletalMesh->SetCollisionProfileName(TEXT("CharacterMesh"));

    // 애니메이션 블루프린트
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshAsset(TEXT("/Game/Enemies/Ch10_nonPBR.Ch10_nonPBR"));
    if (SkeletalMeshAsset.Succeeded())
    {
        SkeletalMesh->SetSkeletalMesh(SkeletalMeshAsset.Object);

        static ConstructorHelpers::FClassFinder<UEnemyAnimInstance> AnimBPClass(TEXT("/Game/Enemies/DefaultAnimBlueprint.DefaultAnimBlueprint_C"));
        if (AnimBPClass.Succeeded())
        {
            SkeletalMesh->SetAnimInstanceClass(AnimBPClass.Class);
        }
        else {
            UE_LOG(LogTemp, Error, TEXT("Failed to Load AnimInstance in CS"));
        }
    }

    static ConstructorHelpers::FObjectFinder<UAnimMontage> ZombieScreamMontageAsset(TEXT("/Game/Enemies/ZombieScreamMontage.ZombieScreamMontage"));
    if (ZombieScreamMontageAsset.Succeeded()) { ZombieScreamMontage = ZombieScreamMontageAsset.Object; }
    else { UE_LOG(LogTemp, Error, TEXT("Failed to load ZombieScreamMontage!")); }

    /*
    RecognitionVolume = CreateDefaultSubobject<USphereComponent>(TEXT("RecognitionVolume"));
    if (RecognitionVolume)
    {
        UE_LOG(LogTemp, Warning, TEXT("RecognitionVolume successfully created."));
        RecognitionVolume->InitSphereRadius(RecognitionRadius);
        RecognitionVolume->SetCollisionProfileName(TEXT("Trigger")); // 콜리전 프로파일 설정

        RecognitionVolume->SetupAttachment(RootComponent);
        RecognitionVolume->OnComponentBeginOverlap.AddDynamic(this, &AProject1Enemy::OnPlayerEnterRecognitionVolume);
        RecognitionVolume->OnComponentEndOverlap.AddDynamic(this, &AProject1Enemy::OnPlayerExitRecognitionVolume);
        RecognitionRadius = 300.0f;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("RecognitionVolume is NULL."));
    }
    */
    

    AttackRange = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRange"));
    if (AttackRange)
    {
        AttackRange->InitSphereRadius(AttackRadius);
        AttackRange->SetCollisionProfileName(TEXT("Trigger"));
        AttackRange->SetupAttachment(RootComponent);
        AttackRange->OnComponentBeginOverlap.AddDynamic(this, &AProject1Enemy::OnPlayerEnterAttackRange);
        AttackRange->OnComponentEndOverlap.AddDynamic(this, &AProject1Enemy::OnPlayerExitAttackRange);
    }

    bIsGaugeIncreaseTimerActive = false;

    IsDead = false;
    TimeBeforeRemoval = 7.0f;

    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    AIControllerClass = AProject1EnemyAIController::StaticClass();

}

// Called when the game starts or when spawned
void AProject1Enemy::BeginPlay()
{
    UE_LOG(LogTemp, Warning, TEXT("Before Super::BeginPlay"));
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("After Super::BeginPlay"));

    TargetMovementLocation = GetActorLocation();

    AnimInstance = Cast<UEnemyAnimInstance>(SkeletalMesh->GetAnimInstance());
    if (!AnimInstance) {
        UE_LOG(LogTemp, Error, TEXT("Failed to Load AnimInstance"));
    }
    else {
        UE_LOG(LogTemp, Log, TEXT("AnimInstance Loaded Successfully"));
    }

    if (EnemyHPBarWidgetClass)
    {
        EnemyHPWidget = CreateWidget<UEnemyHPWidget>(GetWorld(), EnemyHPBarWidgetClass);
        if (EnemyHPWidget)
        {
            // 뷰포트에 위젯 추가
            EnemyHPWidget->AddToViewport();
            HPProgressBar = Cast<UProgressBar>(EnemyHPWidget->GetWidgetFromName(TEXT("HPProgressBar")));
            HPProgressBar->SetPercent(1.0f);
            EnemyHPWidget->SetVisibility(ESlateVisibility::Hidden);
            if (!HPProgressBar)
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to find HPProgressBar in EnemyHPWidget"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create EnemyHPWidget"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("EnemyHPBarWidgetClass is nullptr"));
    }


}

void AProject1Enemy::UpdateUIPosition()
{
    if (EnemyHPWidget)
    {
        // 적의 위치를 스크린 좌표로 변환
        FVector2D ScreenPosition;
        UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), GetActorLocation(), ScreenPosition);

        // UI의 위치를 적의 스크린 좌표로 설정
        int32 ViewportSizeX, ViewportSizeY;

        GetWorld()->GetFirstPlayerController()->GetViewportSize(ViewportSizeX, ViewportSizeY);
        FVector2D ViewportSize(ViewportSizeX, ViewportSizeY);

        FVector2D WidgetOffset = FVector2D(0.0f, -50.0f);
        FVector2D FinalPosition = ScreenPosition + WidgetOffset;

        // UI 위치를 설정
        FVector2D AnchorPoint = FVector2D(0.5f, 0.5f); // UI 중앙 앵커
        EnemyHPWidget->SetPositionInViewport(FinalPosition, true);
        EnemyHPWidget->SetAlignmentInViewport(AnchorPoint);
    }
}

// Called every frame
void AProject1Enemy::Tick(float DeltaTime)
{
    if (!IsDead) {
        Super::Tick(DeltaTime);

        /*
        if (PlayerCharacter) {

            // 안전 상황일 때는 눈에 보여야 추적
            if (currentWorldStatus == WorldStatus::Safe) {
                PlayerChase_PlayerCrouch();

                float DistanceToPlayer = FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation());

                // 인식 범위 안에 있는지 확인
                if (DistanceToPlayer <= RecognitionRadius)
                {
                    // UE_LOG(LogTemp, Error, TEXT("Caution 변화 준비중"));
                    // IncreaseRecognitionGauge();
                }
                else
                {
                    // 인식 범위 밖에 있을 때의 처리
                }
            }

            else if (currentWorldStatus == WorldStatus::Caution) {
                if (PlayerCharacter->bIsCrouching) {
                    PlayerChase_PlayerCrouch();
                }
                else if (!PlayerCharacter->bIsCrouching) { PlayerChase_PlayerNOTCrouch(500.0f); }
            }

            // 위험 상황 : 좀비들이 거리를 무시하고 플레이어를 추격.
            else if (currentWorldStatus == WorldStatus::Warning) {
                if (PlayerCharacter) {
                    float DistanceToPlayer = FVector::Distance(PlayerCharacter->GetActorLocation(), GetActorLocation());

                    FVector PlayerLocation = PlayerCharacter->GetActorLocation();
                    FVector EnemyLocation = GetActorLocation();

                    FVector DirectionToPlayer = PlayerLocation - EnemyLocation;

                    FRotator LookAtRotation = FRotationMatrix::MakeFromX(DirectionToPlayer).Rotator();
                    SetActorRotation(FRotator(0.0f, LookAtRotation.Yaw, 0.0f)); // 좌우 회전만 고려하여 설정


                    if (AnimInstance)
                    {
                        AnimInstance->IsMoving = true; // 이동 중임을 설정
                    }

                    if (!IsScreaming)
                    {
                        // Play the animation montage
                        PlayScreamAnimation();
                        IsScreaming = true;
                    }

                    if (!isRecognizingPlayer)
                    {
                        Project1GameMode->SetAlertGuage(2.0f);
                        isRecognizingPlayer = true;
                        UE_LOG(LogTemp, Error, TEXT("플레이어 추적 개시"));
                    }

                    // MoveToTarget(PlayerLocation);
                }


            }

            float DistanceToPlayer = FVector::Distance(PlayerCharacter->GetActorLocation(), GetActorLocation());
        }

        // 구 Tick() 플레이어 추적 레거시 코드
        /*

        if (PlayerCharacter) {
            // 플레이어가 Crouching 중인지 체크
            if (PlayerCharacter->bIsCrouching && Project1GameMode->CurrentWorldStatus != WorldStatus::Warning) {

                // 플레이어가 시야에 있는지 판정
                if (CanSeePlayer())
                {
                    // 추격 시작
                    IsChasing = true;
                    FVector PlayerLocation = PlayerCharacter->GetActorLocation();
                    MoveToTarget(PlayerLocation);

                    // 플레이어 방향으로 회전
                    FRotator LookAtRotation = (PlayerLocation - GetActorLocation()).Rotation();
                    SetActorRotation(FRotator(0.0f, LookAtRotation.Yaw, 0.0f)); // Only rotate on Yaw axis

                    // 애니메이션 체크
                    if (AnimInstance)
                    {
                        AnimInstance->IsMoving = true; // Set moving state
                    }
                    if (!IsScreaming)
                    {
                        PlayScreamAnimation();
                        IsScreaming = true;
                    }
                }
                else
                {
                    IsChasing = false;
                    if (AnimInstance) { AnimInstance->IsMoving = false; }
                    IsScreaming = false;
                }
            }

            // 숙이지 않고 있는 경우

            else if (PlayerCharacter->bIsCrouching == false) {

                // 플레이어 거리 판정
                float DistanceToPlayer = FVector::Distance(PlayerCharacter->GetActorLocation(), GetActorLocation());

                if (DistanceToPlayer < RecogDistance)
                {
                    // Draw the vision cone
                    // DrawVisionCone();

                    FVector PlayerLocation = PlayerCharacter->GetActorLocation();
                    FVector EnemyLocation = GetActorLocation();

                    FVector DirectionToPlayer = PlayerLocation - EnemyLocation;

                    if (DirectionToPlayer.Size() <= RecogDistance)
                    {
                        FRotator LookAtRotation = FRotationMatrix::MakeFromX(DirectionToPlayer).Rotator();
                        SetActorRotation(FRotator(0.0f, LookAtRotation.Yaw, 0.0f)); // 좌우 회전만 고려하여 설정
                        if (AnimInstance)
                        {
                            AnimInstance->IsMoving = true; // 이동 중임을 설정
                        }

                        if (!IsScreaming)
                        {
                            // Play the animation montage
                            PlayScreamAnimation();
                            IsScreaming = true;
                        }

                        if (PlayerController && !isRecognizingPlayer)
                        {
                            isRecognizingPlayer = true;
                            UE_LOG(LogTemp, Error, TEXT("플레이어 추적 시작"));


                            Project1GameMode->SetAlertGuage(2.0f);
                        }

                        MoveToTarget(PlayerLocation);
                    }
                }
            }
        }
        */

        // Update UI position
        UpdateUIPosition();
    }
}

void AProject1Enemy::AttackPlayer()
{
    if (!IsDead) {
        AProject1Character* PlayerCharacter = Cast<AProject1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
        if (PlayerCharacter)
        {
            // 플레이어에게 데미지를 입히는 로직
            float Damage = 10.0f; // 예시 데미지 값
            PlayerCharacter->TakeDamage(Damage, FDamageEvent(), GetController(), this);
            // 필요 시 사운드 재생 등 추가 로직
        }
    }
}

void AProject1Enemy::OnPlayerEnterAttackRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!IsDead) {
        AProject1Character* PlayerCharacter = Cast<AProject1Character>(OtherActor);
        if (PlayerCharacter && AnimInstance)
        {
            UE_LOG(LogTemp, Error, TEXT("플레이어 피격"));
            AttackPlayer();
            AnimInstance->IsAttacking = true;
        }
    }
}


void AProject1Enemy::OnPlayerExitAttackRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    AProject1Character* PlayerCharacter = Cast<AProject1Character>(OtherActor);
    if (PlayerCharacter && AnimInstance)
    {
        AnimInstance->IsAttacking = false;
    }
}

// 플레이어가 시야에 보일 때

void AProject1Enemy::PlayerChase_PlayerCrouch() {

    AProject1Character* PlayerCharacter = Cast<AProject1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    APlayerController* PlayerController = Cast<APlayerController>(PlayerCharacter->GetController());
    AProject1GameMode* Project1GameMode = GetWorld()->GetAuthGameMode<AProject1GameMode>();

    if (PlayerCharacter) {
        // 플레이어가 Crouching 중인지 체크
        // if (PlayerCharacter->bIsCrouching && Project1GameMode->CurrentWorldStatus != WorldStatus::Warning) {

            // 플레이어가 시야에 있는지 판정
        if (CanSeePlayer())
        {
            // 추격 시작
            IsChasing = true;
            FVector PlayerLocation = PlayerCharacter->GetActorLocation();
            // MoveToTarget(PlayerLocation);

            // 플레이어 방향으로 회전
            FRotator LookAtRotation = (PlayerLocation - GetActorLocation()).Rotation();
            SetActorRotation(FRotator(0.0f, LookAtRotation.Yaw, 0.0f)); // Only rotate on Yaw axis

            // 애니메이션 체크
            if (AnimInstance)
            {
                AnimInstance->IsMoving = true; // Set moving state
            }
            if (!IsScreaming)
            {
                PlayScreamAnimation();
                IsScreaming = true;
            }
            if (!isRecognizingPlayer)
            {
                Project1GameMode->SetAlertGuage(2.0f);
                isRecognizingPlayer = true;
                UE_LOG(LogTemp, Error, TEXT("플레이어 추적 개시 : PlayerChase_PlayerCrouch()"));
            }
        }
        else
        {
            IsChasing = false;
            if (AnimInstance) { AnimInstance->IsMoving = false; }
            IsScreaming = false;
        }
    }

}

void AProject1Enemy::PlayerChase_PlayerNOTCrouch(float RecogDistance)
{
    AProject1Character* PlayerCharacter = Cast<AProject1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    APlayerController* PlayerController = Cast<APlayerController>(PlayerCharacter->GetController());
    AProject1GameMode* Project1GameMode = GetWorld()->GetAuthGameMode<AProject1GameMode>();

    if (PlayerCharacter) {

        // 플레이어 거리 판정
        float DistanceToPlayer = FVector::Distance(PlayerCharacter->GetActorLocation(), GetActorLocation());

        if (DistanceToPlayer < RecogDistance)
        {
            // Draw the vision cone
            // DrawVisionCone();

            FVector PlayerLocation = PlayerCharacter->GetActorLocation();
            FVector EnemyLocation = GetActorLocation();

            FVector DirectionToPlayer = PlayerLocation - EnemyLocation;

            if (DirectionToPlayer.Size() <= RecogDistance)
            {
                FRotator LookAtRotation = FRotationMatrix::MakeFromX(DirectionToPlayer).Rotator();
                SetActorRotation(FRotator(0.0f, LookAtRotation.Yaw, 0.0f)); // 좌우 회전만 고려하여 설정
                if (AnimInstance)
                {
                    AnimInstance->IsMoving = true; // 이동 중임을 설정
                }

                if (!IsScreaming)
                {
                    // Play the animation montage
                    PlayScreamAnimation();
                    IsScreaming = true;
                }

                if (!isRecognizingPlayer)
                {
                    Project1GameMode->SetAlertGuage(2.0f);
                    isRecognizingPlayer = true;
                    UE_LOG(LogTemp, Error, TEXT("플레이어 추적 개시"));
                }

                // MoveToTarget(PlayerLocation);
            }
        }


    }
}

void AProject1Enemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}
/*
void AProject1Enemy::MoveToTarget(const FVector& InTargetLocation)
{

    FVector CurrentLocation = GetActorLocation();
    FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, InTargetLocation, GetWorld()->GetDeltaSeconds(), EnemyMoveSpeed); // Adjust the movement speed as needed
    SetActorLocation(NewLocation);


    TargetMovementLocation = InTargetLocation;
}
*/

float AProject1Enemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    EnemyHP -= ActualDamage;

    UE_LOG(LogTemp, Warning, TEXT("Enemy took %f damage / Current HP: %f"), ActualDamage, EnemyHP);

    float CurrentHP = FMath::Clamp(EnemyHP, 0.0f, MaxHP);
    float Percentage = CurrentHP / MaxHP;
    HPProgressBar->SetPercent(Percentage);

    // 0일때 Enemy HUD 제거
    if (EnemyHP <= 0 && !IsDead)
    {
        if (EnemyHPWidget)
        {
            EnemyHPWidget->SetVisibility(ESlateVisibility::Hidden);
        }
        Die();
    }

    return ActualDamage;
}

void AProject1Enemy::Die()
{
    IsDead = true;

    // Enable physics simulation for the skeletal mesh
    GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
    GetMesh()->SetAllBodiesSimulatePhysics(true);
    GetMesh()->WakeAllRigidBodies();
    GetMesh()->bBlendPhysics = true;

    if (SkeletalMesh)
    {
        SkeletalMesh->SetCollisionProfileName(TEXT("Ragdoll"));
        SkeletalMesh->SetSimulatePhysics(true);
        SkeletalMesh->WakeAllRigidBodies();
        UE_LOG(LogTemp, Warning, TEXT("Simulate Physics activated on MeshComponent"));
    }
    
    /*
    if (RecognitionVolume)
    {
        RecognitionVolume->DestroyComponent();
        RecognitionVolume = nullptr;
    }
    */
    // CollisionCylinder->DestroyComponent();

    GetCharacterMovement()->MaxWalkSpeed = 300.0f;  // AI 이동 속도 설정
    GetCharacterMovement()->bOrientRotationToMovement = true;  // 이동 방향으로 회전
    GetCharacterMovement()->bUseControllerDesiredRotation = true;  // AIController가 회전을 제어

    // Set a timer to remove the enemy from the game after a delay
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AProject1Enemy::OnDeathFinished, TimeBeforeRemoval, false);
}

void AProject1Enemy::OnDeathFinished()
{
    Destroy();
}

void AProject1Enemy::PlayScreamAnimation()
{
    if (ZombieScreamMontage)
    {
        EnemyHPWidget->SetVisibility(ESlateVisibility::Visible);
        PlayAnimMontage(ZombieScreamMontage, 1.f, NAME_None);

        // 로그 출력
        UE_LOG(LogTemp, Warning, TEXT("Zombiescream animation is playing!"));
    }
}

/*

void AProject1Enemy::DrawVisionCone()
{
    if (GetWorld())
    {
        AProject1Character* PlayerCharacter = Cast<AProject1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
        if (PlayerCharacter)
        {

            const int32 NumSegments = 36; // Number of segments to approximate the fan shape
            const float AngleIncrement = 360.0f / NumSegments;
            const FVector StartDirection = GetActorForwardVector();
            const FVector EnemyLocation = GetActorLocation();
            const float MaxDistance = RecogDistance;

            for (int32 i = 0; i < NumSegments; ++i)
            {
                // Calculate the angle for this segment
                float Angle = i * AngleIncrement;

                // Calculate the direction vector for this segment
                FVector SegmentDirection = StartDirection.RotateAngleAxis(Angle, FVector::UpVector);

                // Calculate the endpoint of this segment
                FVector Endpoint = EnemyLocation + SegmentDirection * MaxDistance;

                // Draw a red line from the enemy location to the endpoint
                DrawDebugLine(GetWorld(), EnemyLocation, Endpoint, FColor::Red, false, 0.0f, 0, 1.0f);
            }
        }
    }
}



void AProject1Enemy::OnPlayerEnterRecognitionVolume(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
    const FHitResult& SweepResult)
{
    UE_LOG(LogTemp, Warning, TEXT("Recognition volume overlap event triggered."));

        AProject1Character* Character = Cast<AProject1Character>(OtherActor);
        if (Character) {
            UE_LOG(LogTemp, Error, TEXT("Caution 변화 준비중"));
            IncreaseRecognitionGauge();
        }

}
*/

/*
void AProject1Enemy::OnPlayerEnterRecognitionVolume(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AProject1Character* Character = Cast<AProject1Character>(OtherActor);
    if (Character)
    {
        UE_LOG(LogTemp, Warning, TEXT("Player Entered Recognition Volume"));
        StartRecognitionGaugeIncrease();
    }
}

void AProject1Enemy::OnPlayerExitRecognitionVolume(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    AProject1Character* Character = Cast<AProject1Character>(OtherActor);
    if (Character)
    {
        UE_LOG(LogTemp, Warning, TEXT("Stopped Timer"));
        GetWorldTimerManager().ClearTimer(GaugeIncreaseTimerHandle);
        StopRecognitionGaugeIncrease();
    }
}

void AProject1Enemy::StartRecognitionGaugeIncrease()
{
    if (!bIsGaugeIncreaseTimerActive)
    {
        UE_LOG(LogTemp, Warning, TEXT("Timer Begin"));
        GetWorldTimerManager().SetTimer(GaugeIncreaseTimerHandle, this, &AProject1Enemy::IncreaseRecognitionGauge, 1.0f, true);
        bIsGaugeIncreaseTimerActive = true;
    }
}

void AProject1Enemy::StopRecognitionGaugeIncrease()
{
    if (bIsGaugeIncreaseTimerActive)
    {
        GetWorldTimerManager().ClearTimer(GaugeIncreaseTimerHandle);
        bIsGaugeIncreaseTimerActive = false;
    }
}

void AProject1Enemy::IncreaseRecognitionGauge()
{
    AProject1GameMode* GameMode = Cast<AProject1GameMode>(GetWorld()->GetAuthGameMode());
    if (GameMode) {
        UE_LOG(LogTemp, Error, TEXT("함수 표지"));
        GameMode->SetRecogGuage(2.0f);
    }
}

/*

void AProject1Enemy::CheckWorldStatus()
{
    // Check if recognition gauge is full
    if (RecognitionGaugeFull())
    {
        // Change world status to Caution
        AProject1GameMode* GameMode = Cast<AProject1GameMode>(GetWorld()->GetAuthGameMode());
        if (GameMode)
        {
            GameMode->SetCurrentWorldStatus(WorldStatus::Caution);
        }
    }
}

*/

bool AProject1Enemy::CanSeePlayer()
{
    if (GetWorld())
    {
        AProject1Character* PlayerCharacter = Cast<AProject1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
        if (PlayerCharacter)
        {
            FVector DirectionToPlayer = PlayerCharacter->GetActorLocation() - GetActorLocation();

            float AngleToPlayer = FMath::Acos(FVector::DotProduct(GetActorForwardVector(), DirectionToPlayer.GetSafeNormal()));
            float AngleInDegrees = FMath::RadiansToDegrees(AngleToPlayer);

            if (AngleInDegrees <= FieldOfView && DirectionToPlayer.Size() <= 4000.0f)
            {
                FHitResult HitResult;
                FCollisionQueryParams CollisionParams;
                CollisionParams.AddIgnoredActor(this); // 본체 액터 무시

                CollisionParams.AddIgnoredActor(PlayerCharacter);
                CollisionParams.bTraceComplex = true;

                // 라인 트레이싱
                if (!GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(), PlayerCharacter->GetActorLocation(), ECC_Visibility, CollisionParams))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool AProject1Enemy::IsPlayerInFront(const FVector& PlayerDirection) const
{
    // Calculate the dot product between enemy's forward vector and player's direction
    float DotProduct = FVector::DotProduct(GetActorForwardVector(), PlayerDirection.GetSafeNormal());

    // If the dot product is positive, player is in front of the enemy
    return DotProduct > 0;
}