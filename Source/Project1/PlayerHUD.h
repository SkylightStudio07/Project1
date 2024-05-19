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

	// �Ѿ� �ܷ� �ؽ�Ʈ
	UFUNCTION(BlueprintCallable)
		void SetAmmoText(int32 RemainingAmmo);

	// �� ������ Alert ProgressBar�� ����
	UFUNCTION(BlueprintCallable)
		void SetAlertProgressBar(float ParaGuage);
	UFUNCTION(BlueprintCallable)
		void SetAlertProgressBarColorWithAlertLevel(int32 Status);

	// �� ������ Recognition ProgressBar�� ����
	UFUNCTION(BlueprintCallable)
		void SetRecogProgressBar(float ParaGuage);

	// ��縦 ����ϴ� �Լ�
	UFUNCTION(BlueprintCallable, Category = "Dialog")
		void DisplayDialog(FText DialogText);

protected:
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* AmmoText;

	UPROPERTY(meta = (BindWidget))
		class UProgressBar* AlertProgressBar;

	UPROPERTY(meta = (BindWidget))
		class UProgressBar* RecogProgressBar;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* DialogTextBlock;
	
};
