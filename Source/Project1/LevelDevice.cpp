// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelDevice.h"

// Sets default values
ALevelDevice::ALevelDevice()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALevelDevice::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALevelDevice::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ALevelDevice::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

