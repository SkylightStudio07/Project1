// Fill out your copyright notice in the Description page of Project Settings.


#include "Project1AnimInstance.h"

UProject1AnimInstance::UProject1AnimInstance() {
    IsAiming = false;
    IsFiring = false;
}

void UProject1AnimInstance::SetIsAiming(bool bNewIsAiming)
{
    IsAiming = bNewIsAiming;
}
void UProject1AnimInstance::SetIsFiring(bool bNewIsFiring)
{
    IsFiring = bNewIsFiring;
}
