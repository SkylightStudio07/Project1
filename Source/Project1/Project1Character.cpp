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
    // �浹 ĸ�� ũ�� ����
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // �Է��� ���� �⺻ ȸ�� �ӵ� ����
    BaseTurnRate = 45.f;
    BaseLookUpRate = 45.f;

    // ��Ʈ�ѷ��� ȸ���� �� ĳ���Ͱ� ȸ������ �ʵ��� ����. ī�޶� ȸ��
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // ĳ���� ������ ����
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
    GetCharacterMovement()->JumpZVelocity = 600.f;
    GetCharacterMovement()->AirControl = 0.2f;

    // ī�޶� �� ���� (�浹 �� �÷��̾� ������ �����)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 200.0f;
    CameraBoom->bUsePawnControlRotation = true;

    // �ȷο� ī�޶� ����
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    // �÷��̾� �ִ� �ν��Ͻ� ����
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

    // ����  ����
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
    WeaponCamera->Deactivate();  // �⺻������ ��Ȱ��ȭ

    Bullets = 60;
    CanFire = true;
    bIsCrouching = false;
    ControlMode = 1;


    MaxWalkSpeed = 600.0f;
    MaxWalkSpeedCrouched = 300.0f;  // ��ũ���� �ȱ� �ӵ� ����

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

    // PlayerAnimInstance �ʱ�ȭ
    PlayerAnimInstance = Cast<UProject1AnimInstance>(GetMesh()->GetAnimInstance());

    if (!PlayerAnimInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to initialize PlayerAnimInstance!"));
    }

    FRotator WeaponRotation = FRotator(0.0f, 82.0f, 0.0f); // ȸ���Ϸ��� ������ �����մϴ�.
    Weapon->SetRelativeRotation(WeaponRotation); // ���⸦ ������ ������ ȸ����ŵ�ϴ�.

    // ����� ����� �߰��Ͽ� ������ ȸ�� ���� Ȯ���մϴ�.
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
    SetControlMode(ControlMode); // ī�޶� ���� �Լ� ȣ��
}

void AProject1Character::ChangeMovementSpeed(float NewSpeed)
{
    // �̵� �ӵ� ����
    MaxWalkSpeed = NewSpeed;
    GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void AProject1Character::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    if (bIsFiring)
    {
        // ī�޶��� ȸ���� ��������
        FRotator CameraRotation = FollowCamera->GetComponentRotation();
        CameraRotation.Roll = 0.0f; // Roll�� ������ �� �ֽ��ϴ�.

        // ĳ���� ȸ���� ī�޶��� ȸ���� ��ġ��Ŵ
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
            // �ѱ� ��ġ�� ������ �����ɴϴ�.
            FVector MuzzleLocation = Weapon->GetSocketLocation(TEXT("MuzzleFlash"));
            FRotator MuzzleRotation = Weapon->GetSocketRotation(TEXT("MuzzleFlash"));

            // �Ѿ��� �߻�� ������ ���� �������� �����մϴ�.
            FVector MuzzleDirection = MuzzleRotation.Vector();

            // �Ѿ��� �����ϰ� �߻��մϴ�.
            Bullets--;
            UpdateAmmoText(Bullets);
            UE_LOG(LogTemp, Warning, TEXT("Fire"));

            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            SpawnParams.Instigator = GetInstigator();
            AProject1Projectile* Projectile = GetWorld()->SpawnActor<AProject1Projectile>(ProjectileClass, MuzzleLocation, MuzzleDirection.Rotation(), SpawnParams);
            

            if (Projectile)
            {
                // �Ѿ� ����ü �߻�
                Projectile->FireInDirection(MuzzleDirection);

                // FPS���� ó��
                if (ControlMode == 1 || bIsAiming == true) {
                    // �Ѿ� �߻� �� �ִϸ��̼� ���� ����
                    bIsFiring = true;
                    if (PlayerAnimInstance != nullptr)
                    {
                        PlayerAnimInstance->SetIsFiring(bIsFiring);
                    }
                }
                else if (ControlMode == 0) {
                    // �Ѿ� �߻� �� �ִϸ��̼� ���� ����
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
                // �Ѿ� ���� ���� �� ó��
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
    Fire();  // ù �߻�
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
    else // ���⼭�� �Ѿ� ���� �� ó��
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
    WeaponCamera->SetFieldOfView(40.0f);  // ���� �� ���� �þ�
}

void AProject1Character::OnRightMouseButtonReleased()
{
    bIsAiming = false;
    WeaponCamera->Deactivate();
    FollowCamera->Activate();
    FollowCamera->SetFieldOfView(90.0f);  // �⺻ �þ�
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
    // ���콺 ���� ��ư�� ������ ��
    bIsFiring = true; // ��� ���θ� true�� ����
    PlayRifleFireMontage();
    if (PlayerAnimInstance != nullptr && CanFire)
    {
        if (GunAnimInstance)
        {
            GunAnimInstance->SetGunIsFiring(true); // �ѱ� �ִϸ��̼�
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
    // ���콺 ���� ��ư�� �������� ��
    bIsFiring = false; // ��� ���θ� false�� ����
    if (GunAnimInstance)
    {
        GunAnimInstance->SetGunIsFiring(false); // ���÷� true�� �����մϴ�.
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