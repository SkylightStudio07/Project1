
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

	// IsAiming Setter �Լ�
	void SetIsAiming(bool bNewIsAiming);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Battle, meta = (AllowPrivateAccess = "true"))
		bool IsAiming;

	// IsAiming Setter �Լ�
	void SetIsFiring(bool bNewIsFiring);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Battle, meta = (AllowPrivateAccess = "true"))
		bool IsFiring;

protected :
};
