#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"
#include "EnemyAnimInstance.h"
#include "Project1Enemy.generated.h"

UCLASS()
class PROJECT1_API AProject1Enemy : public ACharacter
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AProject1Enemy();

    /*

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
        float RecogDistance;

    */

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
        float EnemyMoveSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
        float EnemyHP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
        bool IsZako;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation", Meta = (AllowPrivateAccess = true))
        UAnimMontage* ZombieScreamMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
        float FieldOfView;

    // ScreamMontage, 게이지바 체크에 사용
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
        bool isRecognizingPlayer;

    // Function to play scream animation
    void PlayScreamAnimation();

    bool IsScreaming;
    bool IsChasing;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", Meta = (AllowPrivateAccess = true))
    UEnemyAnimInstance* AnimInstance;

    UPROPERTY()
        class UEnemyHPWidget* EnemyHPWidget;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
        class UWidgetComponent* EnemyRecognitionWidget;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
        TSubclassOf<UEnemyHPWidget> EnemyHPBarWidgetClass;

    UPROPERTY()
        class UProgressBar* HPProgressBar;

    float MaxHP;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Recognition")
        class USphereComponent* RecognitionVolume;

    // Radius for the recognition volume
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recognition")
        float RecognitionRadius;

    void IncreaseRecognitionGauge();

    void OnPlayerEnterRecognitionVolume(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
        const FHitResult& SweepResult);

    // Function to change world status to Caution when recognition gauge is full
    // void CheckWorldStatus();



protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // Function to handle enemy movement
    void MoveToTarget(const FVector& TargetLocation);

    // Function to handle enemy taking damage
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    /*
    void DrawVisionCone();
    */

    void UpdateUIPosition();

    bool CanSeePlayer();

    bool IsPlayerInFront(const FVector& PlayerDirection) const;

    void PlayerChase_PlayerCrouch();

    void PlayerChase_PlayerNOTCrouch(float RecogDistance);
    

public:
    // Target location for enemy movement
    FVector TargetLocation;
    FVector TargetMovementLocation;

};