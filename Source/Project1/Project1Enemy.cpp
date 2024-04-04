// Fill out your copyright notice in the Description page of Project Settings.

// Project1Enemy.cpp ����

#include "Project1Enemy.h"
#include "Project1Character.h"
#include "EnemyAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/WidgetComponent.h"
#include "EnemyHPWidget.h"
#include "Components/ProgressBar.h"

// Sets default values
AProject1Enemy::AProject1Enemy()
{
    // Set this character to call Tick() every frame
    PrimaryActorTick.bCanEverTick = true;

    RecogDistance = 500.0f; // ��������
    EnemyMoveSpeed = 200.0f; 
    EnemyHP = 100.0f; 
    MaxHP = EnemyHP;

    static ConstructorHelpers::FObjectFinder<UAnimMontage> ZombieScreamMontageAsset(TEXT("/Game/Enemies/ZombieScreamMontage.ZombieScreamMontage"));
    if (ZombieScreamMontageAsset.Succeeded())
    {
        ZombieScreamMontage = ZombieScreamMontageAsset.Object;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load ZombieScreamMontage!"));
    }

    /* ���� HPBar�� EnemyHPWidget.h���� ������
    HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET"));
    HPBarWidget->SetupAttachment(GetMesh());
    HPBarWidget->SetRelativeLocation(FVector(0.0F, 0.0F, 180.0F));
    HPBarWidget->SetRelativeRotation(FQuat(FRotator(0.0f, 0.0f, 0.0f)));
    HPBarWidget->SetWidgetSpace(EWidgetSpace::World);
    static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(TEXT("/Game/Blueprints/EnemyHPBar.EnemyHPBar_C"));
    if (UI_HUD.Succeeded()) {
        HPBarWidget->SetWidgetClass(UI_HUD.Class);
        HPBarWidget->SetDrawSize(FVector2D(150.0F, 50.0F));
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("Failed to load HPBARWIDGET!"));
    }
    */


}

// Called when the game starts or when spawned
void AProject1Enemy::BeginPlay()
{
    Super::BeginPlay();

    // Set initial target location for enemy movement
    TargetMovementLocation = GetActorLocation();
    AnimInstance = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance());

    if (EnemyHPBarWidgetClass)
    {
        EnemyHPWidget = CreateWidget<UEnemyHPWidget>(GetWorld(), EnemyHPBarWidgetClass);
        if (EnemyHPWidget)
        {
            // ����Ʈ�� ������ �߰��մϴ�.
            EnemyHPWidget->AddToViewport();
            // ProgressBar�� ã�Ƽ� �����մϴ�.
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
        // ���� ��ġ�� ��ũ�� ��ǥ�� ��ȯ
        FVector2D ScreenPosition;
        UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), GetActorLocation(), ScreenPosition);

        // UI�� ��ġ�� ���� ��ũ�� ��ǥ�� ����
        int32 ViewportSizeX, ViewportSizeY;

        GetWorld()->GetFirstPlayerController()->GetViewportSize(ViewportSizeX, ViewportSizeY);
        FVector2D ViewportSize(ViewportSizeX, ViewportSizeY);

        FVector2D WidgetOffset = FVector2D(0.0f, -50.0f);
        FVector2D FinalPosition = ScreenPosition + WidgetOffset;

        // UI ��ġ�� �����մϴ�.
        FVector2D AnchorPoint = FVector2D(0.5f, 0.5f); // UI �߾� ��Ŀ
        EnemyHPWidget->SetPositionInViewport(FinalPosition, true);
        EnemyHPWidget->SetAlignmentInViewport(AnchorPoint);
    }
}

// Called every frame
void AProject1Enemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Check the distance to the player character
    AProject1Character* PlayerCharacter = Cast<AProject1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (PlayerCharacter)
    {
        float DistanceToPlayer = FVector::Distance(PlayerCharacter->GetActorLocation(), GetActorLocation());
        if (IsZako && DistanceToPlayer < RecogDistance)
        {
            // Draw the vision cone
            DrawVisionCone();

            FVector PlayerLocation = PlayerCharacter->GetActorLocation();
            FVector EnemyLocation = GetActorLocation();

            // Calculate vector from enemy to player
            FVector DirectionToPlayer = PlayerLocation - EnemyLocation;

            // Calculate angle between enemy forward vector and vector to player
            float AngleToPlayer = FMath::Acos(FVector::DotProduct(FVector::RightVector, DirectionToPlayer.GetSafeNormal())); // RightVector�� ����Ͽ� ��ä���� ���ʿ��� ����
            float AngleInDegrees = FMath::RadiansToDegrees(AngleToPlayer);

            // Check if the player is within the cone of vision (e.g., 90 degrees)
            if (AngleInDegrees <= 45.0f && DirectionToPlayer.Size() <= RecogDistance)
            {
                FRotator LookAtRotation = FRotationMatrix::MakeFromX(DirectionToPlayer).Rotator();
                SetActorRotation(FRotator(0.0f, LookAtRotation.Yaw - 90.0f, 0.0f)); // �¿� ȸ���� ����Ͽ� ����
                if (AnimInstance)
                {
                    AnimInstance->IsMoving = true; // �̵� ������ ����
                }
                if (!IsScreaming)
                {
                    // Play the animation montage
                    PlayScreamAnimation();
                    IsScreaming = true;
                }

                MoveToTarget(PlayerLocation);
            }
        }
        else if (!IsZako && DistanceToPlayer < RecogDistance)
        {
            FRotator LookAtRotation = FRotationMatrix::MakeFromX(PlayerCharacter->GetActorLocation() - GetActorLocation()).Rotator();
            SetActorRotation(FRotator(0.0f, LookAtRotation.Yaw - 90.0f, 0.0f)); // �¿� ȸ���� ����Ͽ� ����
            if (AnimInstance)
            {
                AnimInstance->IsMoving = true; // �̵� ������ ����
            }
            if (!IsScreaming)
            {
                // Play the animation montage
                PlayScreamAnimation();
                IsScreaming = true;
            }

            MoveToTarget(PlayerCharacter->GetActorLocation());
        }
        else
        {
            if (AnimInstance)
            {
                AnimInstance->IsMoving = false; // �̵� ������ ����
            }
            IsScreaming = false; // Reset the flag if the player is out of range
        }
    }
    UpdateUIPosition();
}

// Called to bind functionality to input
void AProject1Enemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// Function to handle enemy movement
void AProject1Enemy::MoveToTarget(const FVector& InTargetLocation)
{
    // Move the enemy towards the target location
    FVector CurrentLocation = GetActorLocation();
    FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, InTargetLocation, GetWorld()->GetDeltaSeconds(), EnemyMoveSpeed); // Adjust the movement speed as needed
    SetActorLocation(NewLocation);
    // Update target movement location
    TargetMovementLocation = InTargetLocation;
}

// Function to handle enemy taking damage
float AProject1Enemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
    // Call the base TakeDamage function to handle default behavior
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    // Reduce enemy health by the amount of damage taken
    EnemyHP -= ActualDamage;

    // Log the amount of damage taken
    UE_LOG(LogTemp, Warning, TEXT("Enemy took %f damage. Current HP: %f"), ActualDamage, EnemyHP);

    float CurrentHP = FMath::Clamp(EnemyHP, 0.0f, MaxHP);
    float Percentage = CurrentHP / MaxHP;
    HPProgressBar->SetPercent(Percentage);

    // Check if the enemy's health is depleted
    if (EnemyHP <= 0)
    {
        if (EnemyHPWidget)
        {
            EnemyHPWidget->SetVisibility(ESlateVisibility::Hidden);
        }
        // Handle enemy death
        Destroy();
    }

    return ActualDamage;
}

// Function to play scream animation
void AProject1Enemy::PlayScreamAnimation()
{
    if (ZombieScreamMontage)
    {
        EnemyHPWidget->SetVisibility(ESlateVisibility::Visible);
        // Play the animation montage
        PlayAnimMontage(ZombieScreamMontage, 1.f, NAME_None);

        // �α� ���
        UE_LOG(LogTemp, Warning, TEXT("Zombiescream animation is playing!"));
    }
}

void AProject1Enemy::DrawVisionCone()
{
    if (GetWorld())
    {
        // Get the player character
        AProject1Character* PlayerCharacter = Cast<AProject1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
        if (PlayerCharacter)
        {
            // Calculate the vertices of the fan shape
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