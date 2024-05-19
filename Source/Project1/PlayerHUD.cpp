#include "PlayerHUD.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UPlayerHUD::SetAmmoText(int32 RemainingAmmo)
{
	if (AmmoText)
	{
		// AMMO 텍스트 위젯을 찾았으면 새로운 텍스트로 업데이트합니다.
		FText AmmoTextContent = FText::FromString(FString::Printf(TEXT("%d"), RemainingAmmo));
		AmmoText->SetText(AmmoTextContent);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find AMMO text widget!"));
	}
}

void UPlayerHUD::SetAlertProgressBar(float ParaGuage)
{
	if (AlertProgressBar)
	{
		AlertProgressBar->SetPercent(ParaGuage);
	}
}

void UPlayerHUD::SetRecogProgressBar(float ParaGuage)
{
	if (RecogProgressBar)
	{
		RecogProgressBar->SetPercent(ParaGuage);
	}
}

void UPlayerHUD::SetAlertProgressBarColorWithAlertLevel(int32 Status)
{
	// 색을 통합적으로 관리
	FLinearColor ProgressBarColor;
	switch (Status)
	{
	case 0:
		ProgressBarColor = FLinearColor::Blue; // 청색
		break;
	case 1:
		ProgressBarColor = FLinearColor(1.0f, 1.0f, 0.0f); // 경고 상황 : 황색
		break;
	case 2:
		ProgressBarColor = FLinearColor::Red; // 위험 상황 : RED
		break;
	default:
		ProgressBarColor = FLinearColor::Blue; // Default
		break;
	}

	if (AlertProgressBar)
	{
		AlertProgressBar->SetFillColorAndOpacity(ProgressBarColor);
	}
}

void UPlayerHUD::DisplayDialog(FText DialogText)
{
	if (DialogTextBlock)
	{
		DialogTextBlock->SetText(DialogText);
		UE_LOG(LogTemp, Warning, TEXT("Dialogue Text"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find Dialog text widget!"));
	}
}