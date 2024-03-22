#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Project1AnimInstance.h"
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

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);
    void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);
    void TurnAtRate(float Rate);
    void LookUpAtRate(float Rate);
    void MoveForward(float Value);
    void MoveRight(float Value);

    UFUNCTION(BlueprintCallable)
    void SetControlModeTPS();

    UFUNCTION(BlueprintCallable)
    void SetControlModeFPS();

    UFUNCTION(BlueprintCallable)
    void SetControlModeTopView();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
        class USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
        class UCameraComponent* FollowCamera;

    float BaseTurnRate;
    float BaseLookUpRate;

    void OnRightMouseButtonPressed();

    void PlayAimAnimation();

    class UProject1AnimInstance* PlayerAnimInstance;
};
