#include "Project1GameMode.h"
#include "Project1Character.h"
#include "UObject/ConstructorHelpers.h"
#include "PlayerHUD.h"

AProject1GameMode::AProject1GameMode()
{
    static ConstructorHelpers::FClassFinder<ACharacter> PlayerCharacterBPClass(TEXT("/Game/AnimStarterPack/Ue4ASP_Character.Ue4ASP_Character_C"));
    if (PlayerCharacterBPClass.Class != NULL)
    {
        DefaultPawnClass = PlayerCharacterBPClass.Class;
    }

    CurrentWorldStatus = WorldStatus::Safe;
    CurrentAlertGuage = 0;
    MaxAlertGuage = 100.0f;
    /*
    if (PlayerHUDClass)
    {
        PlayerHUDInstance = CreateWidget<UPlayerHUD>(GetWorld(), PlayerHUDClass);

        if (PlayerHUDInstance)
        {
            PlayerHUDInstance->AddToViewport(); // 화면에 추가
            PlayerHUDInstance->SetAlertProgressBar(0);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerHUD class is not set!"));
    }
    */
}

void AProject1GameMode::BeginPlay()
{
    Super::BeginPlay();

    // BeginPlay에서 HUD 생성 및 설정
    if (PlayerHUDClass)
    {
        PlayerHUDInstance = CreateWidget<UPlayerHUD>(GetWorld(), PlayerHUDClass);

        if (PlayerHUDInstance)
        {
            PlayerHUDInstance->AddToViewport(); // 화면에 추가
            PlayerHUDInstance->SetAlertProgressBar(0);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerHUD class is not set!"));
    }
}

void AProject1GameMode::SetCurrentWorldStatus(WorldStatus AlertLevel) {
    CurrentWorldStatus = AlertLevel;
}
void AProject1GameMode::SetAlertGuage(float GuageAmount) {
    CurrentAlertGuage += GuageAmount;

    UE_LOG(LogTemp, Error, TEXT("CurrentAlertGuage : %f"), CurrentAlertGuage);

    float currentAlertGuage = FMath::Clamp(CurrentAlertGuage, 0.0f, MaxAlertGuage);
    float Percentage = currentAlertGuage / MaxAlertGuage;
    PlayerHUDInstance->SetAlertProgressBar(Percentage);
    
    if (Percentage >= 0 && Percentage <= 0.33f) {
        CurrentWorldStatus = WorldStatus::Safe;
        PlayerHUDInstance->SetAlertProgressBarColorWithAlertLevel(0);
    }
    else if (Percentage <= 0.66f && Percentage > 0.33f) {
        CurrentWorldStatus = WorldStatus::Caution;
        PlayerHUDInstance->SetAlertProgressBarColorWithAlertLevel(1);
    }
    else if ((Percentage > 0.66f)) {
        CurrentWorldStatus = WorldStatus::Warning;
        PlayerHUDInstance->SetAlertProgressBarColorWithAlertLevel(2);
    }
    
}

WorldStatus AProject1GameMode::GetCurrentWorldStatus() const
{
    return CurrentWorldStatus;
}