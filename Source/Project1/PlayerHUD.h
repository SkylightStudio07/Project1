// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.generated.h"

UCLASS()
class PROJECT1_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()

public:

	UPlayerHUD(const FObjectInitializer& ObjectInitializer);

	// 총알 잔량 텍스트
	UFUNCTION(BlueprintCallable)
		void SetAmmoText(int32 RemainingAmmo);

	// 이 섹션은 Alert ProgressBar을 관장
	UFUNCTION(BlueprintCallable)
		void SetAlertProgressBar(float ParaGuage);
	UFUNCTION(BlueprintCallable)
		void SetAlertProgressBarColorWithAlertLevel(int32 Status);

	// 이 섹션은 Recognition ProgressBar을 관장
	UFUNCTION(BlueprintCallable)
		void SetRecogProgressBar(float ParaGuage);

	// 대사를 출력하는 함수
	UFUNCTION(BlueprintCallable, Category = "Dialog")
		void DisplayDialog(const FText& Message, class AProject1LevelDevice* DeviceToDeactivate);

	UFUNCTION(BlueprintCallable, Category = "Dialog")
		void HideDialog();

	FTimerHandle DialogTimerHandle;

	AProject1LevelDevice* TargetDevice;

	void SetTimer_World1();

protected:

	virtual void NativeConstruct() override;

	/*
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* AmmoText;

	UPROPERTY(meta = (BindWidget))
		class UProgressBar* AlertProgressBar;

	UPROPERTY(meta = (BindWidget))
		class UProgressBar* RecogProgressBar;
		*/

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* DialogTextBlock;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TimerText;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Timer;
	
	UPROPERTY()
		class UFont* KoreanFont;

	FTimerHandle TimerHandle_World1;
	int32 CountdownTime;

	void UpdateTimer();
};
