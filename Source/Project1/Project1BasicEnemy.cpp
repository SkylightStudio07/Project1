#include "Project1BasicEnemy.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "Project1Character.h"
#include "DrawDebugHelpers.h"
#include "EnemyAnimInstance.h"
#include "TimerManager.h"
#include "Components/WidgetComponent.h"
#include "EnemyHPWidget.h"
#include "Components/MeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Project1GameMode.h"
#include "Components/ProgressBar.h"

AProject1BasicEnemy::AProject1BasicEnemy()
{

    IsDead = false;
    EnemyHP = 200;
    TimeBeforeRemoval = 5.0f;

    PrimaryActorTick.bCanEverTick = true;
    EnemyMoveSpeed = 50.0f;

    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
    SkeletalMesh->SetupAttachment(RootComponent);
    SkeletalMesh->SetCollisionProfileName(TEXT("CharacterMesh"));

    // 애니메이션 블루프린트
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshAsset(TEXT("/Game/Enemies/Ch10_nonPBR.Ch10_nonPBR"));
    SkeletalMesh->SetSkeletalMesh(SkeletalMeshAsset.Object);

    static ConstructorHelpers::FClassFinder<UEnemyAnimInstance> AnimBPClass(TEXT("/Game/Enemies/DefaultAnimBlueprint.DefaultAnimBlueprint_C"));
    if (AnimBPClass.Succeeded())
    {
        SkeletalMesh->SetAnimInstanceClass(AnimBPClass.Class);
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("Failed to Load AnimInstance in CS"));
    }

    BulletCollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BulletCollisionCapsule"));
    BulletCollisionCapsule->SetupAttachment(RootComponent);
    BulletCollisionCapsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    BulletCollisionCapsule->SetCollisionResponseToAllChannels(ECR_Ignore);
    BulletCollisionCapsule->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block); // 총알과 충돌
    BulletCollisionCapsule->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block); // 사용자 지정 총알 트레이스 채널
    BulletCollisionCapsule->SetCapsuleHalfHeight(96.0f);
    BulletCollisionCapsule->SetCapsuleRadius(42.0f);
}

void AProject1BasicEnemy::BeginPlay()
{
    Super::BeginPlay();

    // 플레이어 캐릭터 참조
    PlayerActor = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

    if (!PlayerActor)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerActor not found!"));
    }

    if (PlayerActor) {
        UCharacterMovementComponent* MovementComp = FindComponentByClass<UCharacterMovementComponent>();
        if (MovementComp)
        {
            MovementComp->MaxWalkSpeed = EnemyMoveSpeed;
            UE_LOG(LogTemp, Warning, TEXT("AI Max Walk Speed set to: %f"), MovementComp->MaxWalkSpeed);
        }
    }
    MaxHP = EnemyHP;
}

void AProject1BasicEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (PlayerActor)
    {
        MoveToPlayer();
    }
}

void AProject1BasicEnemy::MoveToPlayer()
{
    AAIController* AIController = Cast<AAIController>(GetController());
    if (!AIController)
    {
        UE_LOG(LogTemp, Error, TEXT("cannot found aicontroller."));
        return;
    }

    // 플레이어 위치로 이동
    FVector PlayerLocation = PlayerActor->GetActorLocation();
    EPathFollowingRequestResult::Type Result = AIController->MoveToLocation(PlayerLocation, 1.0f); // AcceptanceRadius = 5.0f

    if (Result == EPathFollowingRequestResult::Failed)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to move to player."));
    }
    else if (Result == EPathFollowingRequestResult::AlreadyAtGoal)
    {
        UE_LOG(LogTemp, Warning, TEXT("Already at player location: %s"), *PlayerLocation.ToString());
    }
}

float AProject1BasicEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    EnemyHP -= ActualDamage;

    UE_LOG(LogTemp, Warning, TEXT("Enemy took %f damage / Current HP: %f"), ActualDamage, EnemyHP);

    float CurrentHP = FMath::Clamp(EnemyHP, 0.0f, MaxHP);
    float Percentage = CurrentHP / MaxHP;

    if (EnemyHP <= 0 && !IsDead)
    {
        Die();
    }

    return ActualDamage;
}

void AProject1BasicEnemy::Die()
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

    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AProject1BasicEnemy::OnDeathFinished, TimeBeforeRemoval, false);
}

void AProject1BasicEnemy::OnDeathFinished()
{
    Destroy();
}
