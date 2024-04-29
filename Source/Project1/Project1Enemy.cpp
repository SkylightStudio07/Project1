    // Fill out your copyright notice in the Description page of Project Settings.

// Project1Enemy.cpp 파일

#include "Project1Enemy.h"
#include "Project1Character.h"
#include "EnemyAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/WidgetComponent.h"
#include "EnemyHPWidget.h"
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

    static ConstructorHelpers::FObjectFinder<UAnimMontage> ZombieScreamMontageAsset(TEXT("/Game/Enemies/ZombieScreamMontage.ZombieScreamMontage"));
    if (ZombieScreamMontageAsset.Succeeded())
    {
        ZombieScreamMontage = ZombieScreamMontageAsset.Object;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load ZombieScreamMontage!"));
    }
}

// Called when the game starts or when spawned
void AProject1Enemy::BeginPlay()
{
    Super::BeginPlay();

    TargetMovementLocation = GetActorLocation();
    AnimInstance = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance());

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
    Super::Tick(DeltaTime);
    AProject1Character* PlayerCharacter = Cast<AProject1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    APlayerController* PlayerController = Cast<APlayerController>(PlayerCharacter->GetController());
    AProject1GameMode* Project1GameMode = GetWorld()->GetAuthGameMode<AProject1GameMode>();

    WorldStatus currentWorldStatus = Project1GameMode->CurrentWorldStatus;

    // 안전 상황일 때는 눈에 보여야 추적
    if (currentWorldStatus == WorldStatus::Safe) {
        PlayerChase_PlayerCrouch();
    }

    // 경고 상황일 때는 눈에 보이거나, 가까이 접근하면 추적
    else if (currentWorldStatus == WorldStatus::Caution) {
        if (PlayerCharacter->bIsCrouching) { 
            PlayerChase_PlayerCrouch(); 
        }
        else if (!PlayerCharacter->bIsCrouching) { PlayerChase_PlayerNOTCrouch(400.0f); }
    }

    // 위험 상황 : 좀비들이 거리를 무시하고 플레이어를 추격.
    else if(currentWorldStatus == WorldStatus::Warning) { 
        if (PlayerCharacter) {
            float DistanceToPlayer = FVector::Distance(PlayerCharacter->GetActorLocation(), GetActorLocation());
            // Draw the vision cone
            // DrawVisionCone();

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

            MoveToTarget(PlayerLocation);
        }
                
        
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

                    MoveToTarget(PlayerLocation);
                }
            }
        
    
    }
}

// Called to bind functionality to input
void AProject1Enemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AProject1Enemy::MoveToTarget(const FVector& InTargetLocation)
{
    
    FVector CurrentLocation = GetActorLocation();
    FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, InTargetLocation, GetWorld()->GetDeltaSeconds(), EnemyMoveSpeed); // Adjust the movement speed as needed
    SetActorLocation(NewLocation);
    

    TargetMovementLocation = InTargetLocation;
}

// Function to handle enemy taking damage
float AProject1Enemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    EnemyHP -= ActualDamage;

    UE_LOG(LogTemp, Warning, TEXT("Enemy took %f damage / Current HP: %f"), ActualDamage, EnemyHP);

    float CurrentHP = FMath::Clamp(EnemyHP, 0.0f, MaxHP);
    float Percentage = CurrentHP / MaxHP;
    HPProgressBar->SetPercent(Percentage);

    // 0일때 Enemy HUD 제거
    if (EnemyHP <= 0)
    {
        if (EnemyHPWidget)
        {
            EnemyHPWidget->SetVisibility(ESlateVisibility::Hidden);
        }
        Destroy();
    }

    return ActualDamage;
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

// DrawVisionCone() 레거시 코드
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

                CollisionParams.AddIgnoredActor(PlayerCharacter); // Ignore the player character
                CollisionParams.bTraceComplex = true; // Trace against complex collision

                // Perform the line trace
                if (!GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(), PlayerCharacter->GetActorLocation(), ECC_Visibility, CollisionParams))
                {
                    // Line trace didn't hit any obstacles, player is within sight
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