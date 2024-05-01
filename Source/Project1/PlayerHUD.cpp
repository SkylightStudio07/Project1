// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UPlayerHUD::SetAmmoText(int32 RemainingAmmo)
{
    UTextBlock* AmmoTextWidget = Cast<UTextBlock>(GetWidgetFromName(TEXT("AMMO")));

    if (AmmoTextWidget)
    {
        // AMMO �ؽ�Ʈ ������ ã������ ���ο� �ؽ�Ʈ�� ������Ʈ�մϴ�.
        FText AmmoText = FText::FromString(FString::Printf(TEXT("%d"), RemainingAmmo));
        AmmoTextWidget->SetText(AmmoText);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to find AMMO text widget!"));
    }
}

void UPlayerHUD::SetAlertProgressBar(float ParaGuage)
{
    UProgressBar* AlertProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("ALERTPROGRESSBAR")));
    
    if (AlertProgressBar)
    {
        AlertProgressBar->SetPercent(ParaGuage);
    }   
}

void UPlayerHUD::SetRecogProgressBar(float ParaGuage) {
    UProgressBar* RecogProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("RECOGPROGRESSBAR")));

    if (RecogProgressBar) {
        RecogProgressBar->SetPercent(ParaGuage);
    }
}

void UPlayerHUD::SetAlertProgressBarColorWithAlertLevel(int32 Status) {
    
    // ���� ���������� ����
    FLinearColor ProgressBarColor;
    switch (Status)
    {
    case 0:
        ProgressBarColor = FLinearColor::Blue; // û��
        break;
    case 1:
        ProgressBarColor = FLinearColor(1.0f, 1.0f, 0.0f); // ��� ��Ȳ : Ȳ��
        break;
    case 2:
        ProgressBarColor = FLinearColor::Red; // ���� ��Ȳ : RED
        break;
    default:
        ProgressBarColor = FLinearColor::Blue; // Default
        break;
    }

    UProgressBar* AlertProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("ALERTPROGRESSBAR")));

    if (AlertProgressBar)
    {
        AlertProgressBar->SetFillColorAndOpacity(ProgressBarColor);
    }
}