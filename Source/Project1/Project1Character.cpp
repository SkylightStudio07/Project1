// Copyright Epic Games, Inc. All Rights Reserved.

#include "Project1Character.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Project1AnimInstance.h"
#include "GunAnimInstance.h"
#include "Project1Projectile.h"
#include "PlayerHUD.h"
#include "NavigationSystem.h"
#include "Project1GameMode.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// AProject1Character

AProject1Character::AProject1Character()
{
    // 충돌 캡슐 크기 설정
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // 입력을 위한 기본 회전 속도 설정
    BaseTurnRate = 45.f;
    BaseLookUpRate = 45.f;

    // 컨트롤러가 회전할 때 캐릭터가 회전하지 않도록 설정. 카메라만 회전
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // 캐릭터 움직임 설정
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
    GetCharacterMovement()->JumpZVelocity = 600.f;
    GetCharacterMovement()->AirControl = 0.2f;

    // 카메라 붐 생성 (충돌 시 플레이어 쪽으로 당겨짐)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 200.0f;
    CameraBoom->bUsePawnControlRotation = true;

    // 팔로우 카메라 생성
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    // 플레이어 애님 인스턴스 생성
   //PlayerAnimInstance = CreateDefaultSubobject<UProject1AnimInstance>(TEXT("PlayerAnimInstance"));

    Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_WEAPON(TEXT("/Game/MilitaryWeapSilver/Weapons/Assault_Rifle_A.Assault_Rifle_A"));
    if (SK_WEAPON.Succeeded()) {
        UE_LOG(LogTemp, Error, TEXT("Found Weapon!"));
        Weapon->SetSkeletalMesh(SK_WEAPON.Object);
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("Can't Found Weapon!"));
    }

    // 무기  부착
    FName WeaponSocket(TEXT("thumb_01_r"));
    Weapon->SetupAttachment(GetMesh(), WeaponSocket);

    static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBP_ClassFinder(TEXT("/Game/MilitaryWeapSilver/Weapons/RifleAnimBlueprint.RifleAnimBlueprint_C"));
    if (AnimBP_ClassFinder.Succeeded())
    {
        Weapon->SetAnimInstanceClass(AnimBP_ClassFinder.Class);
        GunAnimInstance = Cast<UGunAnimInstance>(Weapon->GetAnimInstance());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find RifleAnimBlueprint!"));
    }
    RifleAnimInstance = Cast<UAnimInstance>(AnimBP_ClassFinder.Class->GetDefaultObject());

    WeaponCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("WeaponCamera"));
    WeaponCamera->SetupAttachment(Weapon);
    WeaponCamera->SetRelativeLocation(FVector(-10.0f, 10.0f, 30.0f));
    WeaponCamera->bUsePawnControlRotation = true;
    WeaponCamera->Deactivate();  // 기본적으로 비활성화

    Bullets = 60;
    CanFire = true;
    bIsCrouching = false;
    ControlMode = 1;


    MaxWalkSpeed = 600.0f;
    MaxWalkSpeedCrouched = 300.0f;  // 웅크려서 걷기 속도 설정

}

void AProject1Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{

    // Set up gameplay key bindings
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

    PlayerInputComponent->BindAxis("MoveForward", this, &AProject1Character::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AProject1Character::MoveRight);

    PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis("TurnRate", this, &AProject1Character::TurnAtRate);
    PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
    PlayerInputComponent->BindAxis("LookUpRate", this, &AProject1Character::LookUpAtRate);

    PlayerInputComponent->BindTouch(IE_Pressed, this, &AProject1Character::TouchStarted);
    PlayerInputComponent->BindTouch(IE_Released, this, &AProject1Character::TouchStopped);

    // Bind control mode functions
    PlayerInputComponent->BindAction("SetTPS", IE_Pressed, this, &AProject1Character::SetControlModeTPS);
    PlayerInputComponent->BindAction("SetFPS", IE_Pressed, this, &AProject1Character::SetControlModeFPS);
    // PlayerInputComponent->BindAction("SetTopView", IE_Pressed, this, &AProject1Character::SetControlModeTopView);

    PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AProject1Character::Crouching);
    PlayerInputComponent->BindAction("CrouchEnd", IE_Released, this, &AProject1Character::CrouchingEnd);

    // PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AProject1Character::OnRightMouseButtonPressed);
    PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AProject1Character::OnRightMouseButtonPressed);
    PlayerInputComponent->BindAction("Aim", IE_Released, this, &AProject1Character::OnRightMouseButtonReleased);
    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AProject1Character::OnLeftMouseButtonPressed);
    PlayerInputComponent->BindAction("Fire_out", IE_Released, this, &AProject1Character::OnLeftMouseButtonReleased);

    PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AProject1Character::Reload);
}

void AProject1Character::BeginPlay()
{
    Super::BeginPlay();
    GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;

    // PlayerAnimInstance 초기화
    PlayerAnimInstance = Cast<UProject1AnimInstance>(GetMesh()->GetAnimInstance());

    if (!PlayerAnimInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to initialize PlayerAnimInstance!"));
    }

    FRotator WeaponRotation = FRotator(0.0f, 82.0f, 0.0f); // 회전하려는 각도를 설정합니다.
    Weapon->SetRelativeRotation(WeaponRotation); // 무기를 설정한 각도로 회전시킵니다.

    // 디버그 출력을 추가하여 무기의 회전 값을 확인합니다.
    UE_LOG(LogTemp, Warning, TEXT("Weapon Relative Rotation: %s"), *Weapon->GetRelativeRotation().ToString());

    GunAnimInstance = Cast<UGunAnimInstance>(Weapon->GetAnimInstance());

    if (!GunAnimInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to initialize GunAnimInstance!"));
    }

    if (PlayerHUDClass)
    {
        APlayerController* PlayerController = Cast<APlayerController>(GetController());
        if (PlayerController)
        {
            PlayerHUD = CreateWidget<UPlayerHUD>(PlayerController, PlayerHUDClass);
            if (PlayerHUD)
            {
                PlayerHUD->AddToViewport();
            }
        }
    }

    // SetControlMode(1);

    UpdateAmmoText(Bullets);

    if (GetCapsuleComponent())
    {
        FNavigationSystem::UpdateComponentData(*GetCapsuleComponent());
        UE_LOG(LogTemp, Warning, TEXT("Player collision data updated on NavMesh."));
    }
}

void AProject1Character::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    SetControlMode(ControlMode); // 카메라 설정 함수 호출
}

void AProject1Character::ChangeMovementSpeed(float NewSpeed)
{
    // 이동 속도 변경
    MaxWalkSpeed = NewSpeed;
    GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void AProject1Character::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    if (bIsFiring)
    {
        // 카메라의 회전을 가져오기
        FRotator CameraRotation = FollowCamera->GetComponentRotation();
        CameraRotation.Roll = 0.0f; // Roll은 무시할 수 있습니다.

        // 캐릭터 회전을 카메라의 회전과 일치시킴
        SetActorRotation(CameraRotation);
    }
}

float AProject1Character::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    if (ActualDamage > 0.0f)
    {
        PlayerHP -= ActualDamage;

        if (PlayerHP <= 0.0f)
        {
            Destroy();
        }
    }
    return ActualDamage;
}

void AProject1Character::Fire()
{
    ReloadManager();
    if (CanFire && Bullets > 0) {
        if (ProjectileClass != nullptr)
        {
            // 총구 위치와 방향을 가져옵니다.
            FVector MuzzleLocation = Weapon->GetSocketLocation(TEXT("MuzzleFlash"));
            FRotator MuzzleRotation = Weapon->GetSocketRotation(TEXT("MuzzleFlash"));

            // 총알이 발사될 방향을 총의 방향으로 설정합니다.
            FVector MuzzleDirection = MuzzleRotation.Vector();

            // 총알을 생성하고 발사합니다.
            Bullets--;
            UpdateAmmoText(Bullets);
            UE_LOG(LogTemp, Warning, TEXT("Fire"));

            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            SpawnParams.Instigator = GetInstigator();
            AProject1Projectile* Projectile = GetWorld()->SpawnActor<AProject1Projectile>(ProjectileClass, MuzzleLocation, MuzzleDirection.Rotation(), SpawnParams);
            

            if (Projectile)
            {
                // 총알 투사체 발사
                Projectile->FireInDirection(MuzzleDirection);

                // FPS에만 처리
                if (ControlMode == 1 || bIsAiming == true) {
                    // 총알 발사 시 애니메이션 상태 변경
                    bIsFiring = true;
                    if (PlayerAnimInstance != nullptr)
                    {
                        PlayerAnimInstance->SetIsFiring(bIsFiring);
                    }
                }
                else if (ControlMode == 0) {
                    // 총알 발사 시 애니메이션 상태 변경
                    bIsFiring = false;
                    if (PlayerAnimInstance != nullptr)
                    {
                        PlayerAnimInstance->SetIsFiring(bIsFiring);
                    }
                }
                else {
                    bIsFiring = false;
                }

                // AProject1GameMode* GameMode = Cast<AProject1GameMode>(GetWorld()->GetAuthGameMode());

                /*

                if (GameMode)
                {
                    GameMode->SetAlertGuage(1.5f); 
                }
                */
                

            }
            else
            {
                // 총알 생성 실패 시 처리
                UE_LOG(LogTemp, Error, TEXT("Failed to spawn projectile!"));
            }
        }
        else
        {
            bIsFiring = false;
        }
    }
}

void AProject1Character::StartFiring()
{
    Fire();  // 첫 발사
    GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &AProject1Character::FireRepeatedly, 0.23f, true);
}

/*
void AProject1Character::StopFiring()
{
    GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}
*/

void AProject1Character::FireRepeatedly()
{
    if (CanFire && Bullets > 0)
    {
        Fire();  
    }
    else // 여기서는 총알 소진 후 처리
    {
        if (GunAnimInstance)
        {
            GunAnimInstance->SetGunIsFiring(false); 
        }
        StopFiring();  
    }
}


void AProject1Character::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AProject1Character::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AProject1Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AProject1Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AProject1Character::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AProject1Character::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AProject1Character::SetControlMode(int32 controlMode)
{
    ControlMode = controlMode;
    if (controlMode == 0) // TPS
    {
        CameraBoom->TargetArmLength = 150.0f;
        CameraBoom->SetRelativeRotation(FRotator::ZeroRotator);
        CameraBoom->bUsePawnControlRotation = true;
        CameraBoom->bInheritPitch = true;
        CameraBoom->bInheritRoll = true;
        CameraBoom->bInheritYaw = true;
        CameraBoom->bDoCollisionTest = true;

        bUseControllerRotationYaw = true;
        bUseControllerRotationPitch = true;
        bUseControllerRotationRoll = true;
        GetCharacterMovement()->bOrientRotationToMovement = false;
    }
    else if (controlMode == 1) // FPS
    {
        CameraBoom->TargetArmLength = 0.0f;
        CameraBoom->SetRelativeRotation(FRotator::ZeroRotator);
        CameraBoom->bUsePawnControlRotation = true;
        CameraBoom->bInheritPitch = true;
        CameraBoom->bInheritRoll = true;
        CameraBoom->bInheritYaw = true;
        CameraBoom->bDoCollisionTest = true;

        bUseControllerRotationYaw = false;
        bUseControllerRotationPitch = true;
        bUseControllerRotationRoll = true;
        GetCharacterMovement()->bOrientRotationToMovement = false;
    }
    else if (controlMode == 2) // Top View
    {
        CameraBoom->TargetArmLength = 800.0f;
        CameraBoom->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));
        CameraBoom->bUsePawnControlRotation = false;
        bUseControllerRotationYaw = true;
    }
}

void AProject1Character::SetControlModeTPS()
{
    APlayerController* MyController = GetController<APlayerController>();
    if (MyController)
    {
        AProject1Character* MyCharacter = Cast<AProject1Character>(MyController->GetPawn());
        if (MyCharacter)
        {
            MyCharacter->SetControlMode(0); // 0 for TPS
        }
    }
}

void AProject1Character::SetControlModeFPS()
{
    APlayerController* MyController = GetController<APlayerController>();
    if (MyController)
    {
        AProject1Character* MyCharacter = Cast<AProject1Character>(MyController->GetPawn());
        if (MyCharacter)
        {
            MyCharacter->SetControlMode(1); // 1 for FPS
        }
    }
}

void AProject1Character::SetControlModeTopView()
{
    APlayerController* MyController = GetController<APlayerController>();
    if (MyController)
    {
        AProject1Character* MyCharacter = Cast<AProject1Character>(MyController->GetPawn());
        if (MyCharacter)
        {
            MyCharacter->SetControlMode(2); // 2 for Top View
        }
    }
}

void AProject1Character::Reload()
{
    if (PlayerAnimInstance != nullptr)
    {
        PlayerAnimInstance->SetIsReloading(true);
        Bullets = 60;
        UpdateAmmoText(60);
        ReloadManager();
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("PlayerAnimInstance is Null!"));

    }
}

void AProject1Character::Crouching()
{
    bIsCrouching = true;
    UE_LOG(LogTemp, Warning, TEXT("Crouching : bIsCrouching : %s"), bIsCrouching ? TEXT("true") : TEXT("false"));
}

void AProject1Character::CrouchingEnd()
{
    bIsCrouching = false;
    UE_LOG(LogTemp, Warning, TEXT("CrouchingEnd : bIsCrouching : %s"), bIsCrouching ? TEXT("true") : TEXT("false"));
}

void AProject1Character::SetIsCrouching(bool isCrouchingSetter) {
    bIsCrouching = isCrouchingSetter;
}

void AProject1Character::OnRightMouseButtonPressed()
{
    bIsAiming = true;
    FollowCamera->Deactivate();
    WeaponCamera->Activate();
    WeaponCamera->SetFieldOfView(40.0f);  // 조준 시 좁은 시야
}

void AProject1Character::OnRightMouseButtonReleased()
{
    bIsAiming = false;
    WeaponCamera->Deactivate();
    FollowCamera->Activate();
    FollowCamera->SetFieldOfView(90.0f);  // 기본 시야
}

/*
void AProject1Character::OnRightMouseButtonPressed()
{
    if (PlayerAnimInstance != nullptr)
    {

        PlayerAnimInstance->SetIsAiming(!PlayerAnimInstance->IsAiming);
        UE_LOG(LogTemp, Warning, TEXT("IsAiming : %s"), PlayerAnimInstance->IsAiming ? TEXT("true") : TEXT("false"));
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("PlayerAnimInstance is Null!"));

    }

}
*/


void AProject1Character::OnLeftMouseButtonPressed()
{
    // 마우스 왼쪽 버튼이 눌렸을 때
    bIsFiring = true; // 사격 여부를 true로 설정
    PlayRifleFireMontage();
    if (PlayerAnimInstance != nullptr && CanFire)
    {
        if (GunAnimInstance)
        {
            GunAnimInstance->SetGunIsFiring(true); // 총기 애니메이션
        }
        PlayerAnimInstance->SetIsFiring(bIsFiring);
        // UE_LOG(LogTemp, Warning, TEXT("%d"), GunAnimInstance->IsGunFiring());
        bIsFiring = true;
        if (CanFire)
        {
            StartFiring();
        }
    }
}

void AProject1Character::OnLeftMouseButtonReleased()
{
    // 마우스 왼쪽 버튼이 떼어졌을 때
    bIsFiring = false; // 사격 여부를 false로 설정
    if (GunAnimInstance)
    {
        GunAnimInstance->SetGunIsFiring(false); // 예시로 true를 설정합니다.
    }
    if (PlayerAnimInstance != nullptr)
    {
        PlayerAnimInstance->SetIsFiring(bIsFiring);

    }

    bIsFiring = false;
    StopFiring();
}

void AProject1Character::PlayRifleFireMontage()
{
    if (RifleAnimInstance != nullptr && CanFire)
    {
        RifleAnimInstance->Montage_Play(RifleFireMontage);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("RifleAnimInstance is null"));
    }
}

void AProject1Character::StopFiring()
{
    GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
    bIsFiring = false;
    if (PlayerAnimInstance != nullptr)
    {
        PlayerAnimInstance->SetIsFiring(false);
    }
}

void AProject1Character::UpdateAmmoText(int32 RemainingAmmo)
{
    AProject1GameMode* Project1GameMode = GetWorld()->GetAuthGameMode<AProject1GameMode>();
    if (Project1GameMode)
    {
        UPlayerHUD* PlayerHUDInstance = Project1GameMode->PlayerHUDInstance;
        if (PlayerHUDInstance)
        {
            PlayerHUDInstance->SetAmmoText(RemainingAmmo);
        }
    }
}

void AProject1Character::ReloadManager() {
    if (Bullets == 0) {
        
        CanFire = false;
    }
    else
    {
        CanFire = true;
    }
}

void AProject1Character::ItemPickup() {
    UpdateAmmoText(Bullets);
}

UPlayerHUD* AProject1Character::GetPlayerHUD() const
{
    return PlayerHUD;
}