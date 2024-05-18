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
    CurrentRecogGuage = 0;
    MaxAlertGuage = 100.0f;
    MaxRecogGuage = 100.0f;
    /*
    if (PlayerHUDClass)
    {
        PlayerHUDInstance = CreateWidget<UPlayerHUD>(GetWorld(), PlayerHUDClass);

        if (PlayerHUDInstance)
        {
            PlayerHUDInstance->AddToViewport(); // ȭ�鿡 �߰�
            PlayerHUDInstance->SetAlertProgressBar(0);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerHUD class is not set!"));
    }
    */

    // IsDoorReadyToOpen 초기화
    IsDoorReadyToOpen = false;
}

void AProject1GameMode::BeginPlay()
{
    Super::BeginPlay();

    // BeginPlay���� HUD ���� �� ����
    if (PlayerHUDClass)
    {
        PlayerHUDInstance = CreateWidget<UPlayerHUD>(GetWorld(), PlayerHUDClass);

        if (PlayerHUDInstance)
        {
            PlayerHUDInstance->AddToViewport(); // ȭ�鿡 �߰�
            PlayerHUDInstance->SetAlertProgressBar(0);
            PlayerHUDInstance->SetRecogProgressBar(0);
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

void AProject1GameMode::SetRecogGuage(float GuageAmount) {
    CurrentRecogGuage += GuageAmount;

    UE_LOG(LogTemp, Error, TEXT("CurrentRecogGuage : %f"), CurrentRecogGuage);

    float currentRecogGuage = FMath::Clamp(CurrentRecogGuage, 0.0f, MaxRecogGuage);
    float Percentage = currentRecogGuage / MaxRecogGuage;
    PlayerHUDInstance->SetRecogProgressBar(Percentage);

    if (Percentage >= 1 && CurrentWorldStatus == WorldStatus::Safe) {
        CurrentWorldStatus = WorldStatus::Caution;
        SetAlertGuage(30.0f);
        UE_LOG(LogTemp, Error, TEXT("Recognition ProgrerssBar is Full. Transited to Caution Status."));
    }
}

WorldStatus AProject1GameMode::GetCurrentWorldStatus() const
{
    return CurrentWorldStatus;
}
