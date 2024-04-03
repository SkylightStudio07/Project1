// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHPWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT1_API UEnemyHPWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget)) // 이 매크로는 ProgressBar를 바인딩합니다.
        class UProgressBar* HPProgressBar; // ProgressBar 포인터 선언	
};
