// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"
#include "Components/TextBlock.h"

void UPlayerHUD::SetAmmoText(int32 RemainingAmmo)
{
    // AMMO 텍스트 위젯을 찾습니다. 예를 들어, AMMO 텍스트 위젯의 이름이 AmmoText라고 가정합니다.
    UTextBlock* AmmoTextWidget = Cast<UTextBlock>(GetWidgetFromName(TEXT("AMMO")));

    if (AmmoTextWidget)
    {
        // AMMO 텍스트 위젯을 찾았으면 새로운 텍스트로 업데이트합니다.
        FText AmmoText = FText::FromString(FString::Printf(TEXT("%d"), RemainingAmmo));
        AmmoTextWidget->SetText(AmmoText);
    }
    else
    {
        // AMMO 텍스트 위젯을 찾지 못한 경우에는 로그를 출력합니다.
        UE_LOG(LogTemp, Warning, TEXT("Failed to find AMMO text widget!"));
    }
}
