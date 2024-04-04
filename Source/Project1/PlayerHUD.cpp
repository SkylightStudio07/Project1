// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"
#include "Components/TextBlock.h"

void UPlayerHUD::SetAmmoText(int32 RemainingAmmo)
{
    // AMMO �ؽ�Ʈ ������ ã���ϴ�. ���� ���, AMMO �ؽ�Ʈ ������ �̸��� AmmoText��� �����մϴ�.
    UTextBlock* AmmoTextWidget = Cast<UTextBlock>(GetWidgetFromName(TEXT("AMMO")));

    if (AmmoTextWidget)
    {
        // AMMO �ؽ�Ʈ ������ ã������ ���ο� �ؽ�Ʈ�� ������Ʈ�մϴ�.
        FText AmmoText = FText::FromString(FString::Printf(TEXT("%d"), RemainingAmmo));
        AmmoTextWidget->SetText(AmmoText);
    }
    else
    {
        // AMMO �ؽ�Ʈ ������ ã�� ���� ��쿡�� �α׸� ����մϴ�.
        UE_LOG(LogTemp, Warning, TEXT("Failed to find AMMO text widget!"));
    }
}
