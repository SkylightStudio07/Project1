// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GunAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT1_API UGunAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
    UGunAnimInstance();
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Battle, meta = (AllowPrivateAccess = "true"))
		bool GunIsFiring;

    UFUNCTION(BlueprintCallable, Category = "Animation")
        bool IsGunFiring() const { return GunIsFiring; }

    UFUNCTION(BlueprintCallable, Category = "Animation")
        void SetGunIsFiring(bool bNewGunIsFiring) { GunIsFiring = bNewGunIsFiring; }

private:

};
