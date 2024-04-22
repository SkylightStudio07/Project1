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
#include "Engine/SkeletalMesh.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// AProject1Character

AProject1Character::AProject1Character()
{
    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // Set our turn rates for input
    BaseTurnRate = 45.f;
    BaseLookUpRate = 45.f;

    // Don't rotate when the controller rotates. Let that just affect the camera.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
    GetCharacterMovement()->JumpZVelocity = 600.f;
    GetCharacterMovement()->AirControl = 0.2f;

    // Create a camera boom (pulls in towards the player if there is a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 200.0f; // The camera follows at this distance behind the character    
    CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

    // Create a follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
    FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

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

    // ���⸦ ĳ���� �޽ÿ� �����մϴ�.
    FName WeaponSocket(TEXT("thumb_01_r"));
    Weapon->SetupAttachment(GetMesh(), WeaponSocket);

    // ���⸦ Ư�� ������ ȸ����ŵ�ϴ�.

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

    Bullets = 60;
    CanFire = true;
    bIsCrouching = false;
    AlertGuage = 0;
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
    PlayerInputComponent->BindAction("SetTopView", IE_Pressed, this, &AProject1Character::SetControlModeTopView);

    PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AProject1Character::Crouching);
    PlayerInputComponent->BindAction("CrouchEnd", IE_Released, this, &AProject1Character::CrouchingEnd);

    PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AProject1Character::OnRightMouseButtonPressed);
    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AProject1Character::OnLeftMouseButtonPressed);
    PlayerInputComponent->BindAction("Fire_out", IE_Released, this, &AProject1Character::OnLeftMouseButtonReleased);
}

void AProject1Character::BeginPlay()
{
    Super::BeginPlay();

    // PlayerAnimInstance �ʱ�ȭ
    PlayerAnimInstance = Cast<UProject1AnimInstance>(GetMesh()->GetAnimInstance());

    if (!PlayerAnimInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to initialize PlayerAnimInstance!"));
    }

    FRotator WeaponRotation = FRotator(0.0f, 90.0f, 0.0f); // ȸ���Ϸ��� ������ �����մϴ�.
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
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            SpawnParams.Instigator = GetInstigator();
            AProject1Projectile* Projectile = GetWorld()->SpawnActor<AProject1Projectile>(ProjectileClass, MuzzleLocation, MuzzleDirection.Rotation(), SpawnParams);
            bIsFiring = true;

            if (Projectile)
            {
                // �Ѿ��� �߻��մϴ�.
                Projectile->FireInDirection(MuzzleDirection);

                // �Ѿ� �߻� �� �ִϸ��̼� ���� ����
                if (PlayerAnimInstance != nullptr)
                {
                    PlayerAnimInstance->SetIsFiring(bIsFiring);
                }
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

void AProject1Character::SetControlMode(int32 ControlMode)
{
    if (ControlMode == 0) // TPS
    {
        CameraBoom->TargetArmLength = 450.0f;
        CameraBoom->SetRelativeRotation(FRotator::ZeroRotator);
        CameraBoom->bUsePawnControlRotation = true;
        CameraBoom->bInheritPitch = true;
        CameraBoom->bInheritRoll = true;
        CameraBoom->bInheritYaw = true;
        CameraBoom->bDoCollisionTest = true;
        bUseControllerRotationYaw = false;
    }
    else if (ControlMode == 1) // FPS
    {
        CameraBoom->TargetArmLength = 0.0f;
        CameraBoom->SetRelativeRotation(FRotator::ZeroRotator);
        CameraBoom->bUsePawnControlRotation = true;
        CameraBoom->bInheritPitch = true;
        CameraBoom->bInheritRoll = true;
        CameraBoom->bInheritYaw = true;
        CameraBoom->bDoCollisionTest = true;
        bUseControllerRotationYaw = false;
    }
    else if (ControlMode == 2) // Top View
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
    if (PlayerAnimInstance != nullptr)
    {

        PlayerAnimInstance->SetIsAiming(!PlayerAnimInstance->IsAiming);
        UE_LOG(LogTemp, Warning, TEXT("IsAiming : %s"), PlayerAnimInstance->IsAiming ? TEXT("true") : TEXT("false"));
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("PlayerAnimInstance is Null!"));

    }

}

void AProject1Character::OnLeftMouseButtonPressed()
{
    // ���콺 ���� ��ư�� ������ ��
    bIsFiring = true; // ��� ���θ� true�� ����
    PlayRifleFireMontage();
    if (PlayerAnimInstance != nullptr && CanFire)
    {
        if (GunAnimInstance)
        {
            GunAnimInstance->SetGunIsFiring(true); // ���÷� true�� �����մϴ�.
        }
        PlayerAnimInstance->SetIsFiring(bIsFiring);
        // UE_LOG(LogTemp, Warning, TEXT("%d"), GunAnimInstance->IsGunFiring());
        Fire();      
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
    bIsFiring = false;
    if (PlayerAnimInstance != nullptr)
    {
        PlayerAnimInstance->SetIsFiring(bIsFiring);
    }
}

void AProject1Character::UpdateAmmoText(int32 RemainingAmmo)
{
    if (PlayerHUDInstance)
    {
        PlayerHUDInstance->SetAmmoText(RemainingAmmo);
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