#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Project1AnimInstance.h"
#include "GunAnimInstance.h"
#include "PlayerHUD.h"
#include "Project1Character.generated.h"

UCLASS(Blueprintable)
class PROJECT1_API AProject1Character : public ACharacter
{
    GENERATED_BODY()

public:
    AProject1Character();

    UFUNCTION(BlueprintCallable)
    void SetControlMode(int32 ControlMode);

    UPROPERTY(VisibleAnywhere, Category = Weapon)
        USkeletalMeshComponent* Weapon;

    UPROPERTY(EditDefaultsOnly, Category = Projectile)
        TSubclassOf<class AProject1Projectile> ProjectileClass;
    
    UFUNCTION(BlueprintCallable)
    void Fire();

    UFUNCTION(BlueprintCallable)
        void PlayRifleFireMontage();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire")
        UAnimBlueprint* RifleAnimBlueprint;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire")
        class UAnimMontage* RifleFireMontage;

    void StopFiring();
    void UpdateAmmoText(int32 RemainingAmmo);
    void ReloadManager();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire")
        int32 Bullets;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire")
        bool CanFire;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire")
        bool isReloading;

    void ItemPickup();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn")
        bool bIsCrouching;

    float MaxAlertGuage;

    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Player")
        UPlayerHUD* GetPlayerHUD() const;

    UPROPERTY()
        UPlayerHUD* PlayerHUD;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
        float PlayerHP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
        float MaxHP;

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator, AActor* DamageCauser) override;


protected:
    virtual void BeginPlay() override;

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
        float MaxWalkSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
        float MaxWalkSpeedCrouched;

    UFUNCTION(BlueprintCallable, Category = "Player")
    void ChangeMovementSpeed(float NewSpeed);

    UPROPERTY(EditAnywhere, Category = "Player")
        TSubclassOf<UPlayerHUD> PlayerHUDClass;

    FTimerHandle FireTimerHandle;

    void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);
    void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);
    void TurnAtRate(float Rate);
    void LookUpAtRate(float Rate);
    void MoveForward(float Value);
    void MoveRight(float Value);
    void Crouching();
    void CrouchingEnd();

    UFUNCTION(BlueprintCallable)
    void SetControlModeTPS();

    UFUNCTION(BlueprintCallable)
    void SetControlModeFPS();

    UFUNCTION(BlueprintCallable)
    void SetControlModeTopView();

    UFUNCTION(BlueprintCallable)
        void SetIsCrouching(bool isCrouchingSetter);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
        class USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
        class UCameraComponent* FollowCamera;

    float BaseTurnRate;
    float BaseLookUpRate;

    void OnRightMouseButtonPressed();
    void OnLeftMouseButtonPressed();
    void OnLeftMouseButtonReleased();

    class UProject1AnimInstance* PlayerAnimInstance;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fire")
        bool bIsFiring;

    // 총 애니메이션 인스턴스
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire")
        UGunAnimInstance* GunAnimInstance;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire")
        class UAnimInstance* RifleAnimInstance;

};