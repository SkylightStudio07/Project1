#include "PlayerHUD.h"
#include "Components/TextBlock.h"
#include "Engine/Font.h"
#include "Components/ProgressBar.h"
#include "TimerManager.h"
#include "Project1LevelDevice.h"

UPlayerHUD::UPlayerHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// 폰트를 로드합니다.
	static ConstructorHelpers::FObjectFinder<UFont> KoreanFontObj(TEXT("/Game/Font/NanumBarunGothic_Font.NanumBarunGothic_Font"));
	if (KoreanFontObj.Succeeded())
	{
		KoreanFont = KoreanFontObj.Object;
	}


}

void UPlayerHUD::SetAmmoText(int32 RemainingAmmo)
{
	UTextBlock* AmmoTextWidget = Cast<UTextBlock>(GetWidgetFromName(TEXT("AmmoText")));

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

void UPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();

	if (TimerText)
	{
		TimerText->SetVisibility(ESlateVisibility::Hidden);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Failed to find TimerText widget!"));
	}
	if (Timer)
	{
		Timer->SetVisibility(ESlateVisibility::Hidden);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Failed to find Timer widget!"));
	}
}

void UPlayerHUD::SetAlertProgressBar(float ParaGuage)
{
	if (AlertProgressBar)
	{
		AlertProgressBar->SetPercent(ParaGuage);
		UE_LOG(LogTemp, Warning, TEXT("Alert Bar Increased."));
	}
}

void UPlayerHUD::SetRecogProgressBar(float ParaGuage)
{

	if (RecogProgressBar) {
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

void UPlayerHUD::DisplayDialog(const FText& Message, AProject1LevelDevice* DeviceToDeactivate)
{
	if (DialogTextBlock)
	{
		// 다이얼로그 텍스트 블록에 메시지를 설정하고 표시합니다.
		DialogTextBlock->SetText(Message);
		DialogTextBlock->SetVisibility(ESlateVisibility::Visible);

		// 폰트를 설정합니다.
		if (KoreanFont)
		{
			FSlateFontInfo FontInfo = FSlateFontInfo(KoreanFont, 24);
			DialogTextBlock->SetFont(FontInfo);
		}

		// 타겟 디바이스를 설정합니다.
		TargetDevice = DeviceToDeactivate;

		// 기존 타이머가 있으면 초기화합니다.
		GetWorld()->GetTimerManager().ClearTimer(DialogTimerHandle);

		// 타이머를 설정하여 5초 후에 HideDialog 함수를 호출하도록 합니다.
		GetWorld()->GetTimerManager().SetTimer(DialogTimerHandle, this, &UPlayerHUD::HideDialog, 5.0f, false);


	}
}

void UPlayerHUD::HideDialog()
{
	if (DialogTextBlock)
	{
		DialogTextBlock->SetVisibility(ESlateVisibility::Hidden);
	}

	if (TargetDevice)
	{
		// 타겟 디바이스를 비활성화합니다.
		TargetDevice->DeactivateDevice();
	}
}

void UPlayerHUD::SetTimer_World1()
{
	if (TimerText)
	{
		TimerText->SetVisibility(ESlateVisibility::Visible);
	}
	if (Timer)
	{
		Timer->SetVisibility(ESlateVisibility::Visible);
	}

	// 타이머 초기화
	CountdownTime = 30;

	// 타이머 텍스트 초기화
	Timer->SetText(FText::AsNumber(CountdownTime));

	// 1초마다 UpdateTimer 함수를 호출하도록 설정
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_World1, this, &UPlayerHUD::UpdateTimer, 1.0f, true);
}

void UPlayerHUD::UpdateTimer()
{
	// 타이머가 0보다 클 경우 1씩 감소
	if (CountdownTime > 0)
	{
		CountdownTime--;

		// 타이머 텍스트 업데이트
		TimerText->SetText(FText::AsNumber(CountdownTime));
	}
	else
	{
		// 타이머가 0에 도달하면 타이머를 멈춤
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_World1);

		// 필요한 경우 타이머 텍스트 숨기기
		TimerText->SetVisibility(ESlateVisibility::Hidden);
		Timer->SetVisibility(ESlateVisibility::Hidden);
	}
}