#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyAnimInstance.h"
#include "Project1Enemy.generated.h"

UCLASS()
class PROJECT1_API AProject1Enemy : public ACharacter
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AProject1Enemy();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
        float RecogDistance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
        float EnemyMoveSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
        float EnemyHP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
        bool IsZako;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation", Meta = (AllowPrivateAccess = true))
        UAnimMontage* ZombieScreamMontage;

    // Function to play scream animation
    void PlayScreamAnimation();

    bool IsScreaming;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", Meta = (AllowPrivateAccess = true))
    UEnemyAnimInstance* AnimInstance;

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

    void DrawVisionCone();

private:
    // Target location for enemy movement
    FVector TargetLocation;
    FVector TargetMovementLocation;

};