// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PlayerHUD.h"
#include "Project1GameMode.generated.h"

UENUM(BlueprintType)
enum class WorldStatus : uint8
{
    Safe UMETA(DisplayName = "Safe"),
    Caution UMETA(DisplayName = "Caution"),
    Warning UMETA(DisplayName = "Warning")
};

UCLASS(minimalapi)
class AProject1GameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

public:

	AProject1GameMode();

    // 게임모드 Alert Sitiuation
    // 0 : Caution
    // 1 : Alert
    // 2 : WARNING

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
        WorldStatus CurrentWorldStatus;

    UFUNCTION(BlueprintCallable)
        void SetCurrentWorldStatus(WorldStatus AlertLevel);

    UFUNCTION(BlueprintCallable, Category = "WorldStatus")
        WorldStatus GetCurrentWorldStatus() const;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
        float CurrentAlertGuage;

    void SetAlertGuage(float GuageAmount);

    float MaxAlertGuage;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    UPlayerHUD* PlayerHUDInstance;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
        TSubclassOf<UPlayerHUD> PlayerHUDClass;

};



