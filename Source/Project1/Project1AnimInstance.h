
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Project1AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT1_API UProject1AnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UProject1AnimInstance();

	/* 이제 상시 조준 상태이므로 필요하지 않다.
	// IsAiming Setter 함수
	void SetIsAiming(bool bNewIsAiming);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Battle, meta = (AllowPrivateAccess = "true"))
		bool IsAiming;
		*/

	// IsAiming Setter 함수
	void SetIsReloading(bool bNewIsReloading);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Battle, meta = (AllowPrivateAccess = "true"))
		bool IsReloading;

	// IsAiming Setter 함수
	void SetIsFiring(bool bNewIsFiring);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Battle, meta = (AllowPrivateAccess = "true"))
		bool IsFiring;

protected :
};
