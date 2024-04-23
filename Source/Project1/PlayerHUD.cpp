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
        // AMMO 텍스트 위젯을 찾았으면 새로운 텍스트로 업데이트합니다.
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