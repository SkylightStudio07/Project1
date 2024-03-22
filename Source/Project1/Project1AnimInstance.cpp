// Fill out your copyright notice in the Description page of Project Settings.


#include "Project1AnimInstance.h"

UProject1AnimInstance::UProject1AnimInstance() {
    IsAiming = false;
}

void UProject1AnimInstance::SetIsAiming(bool bNewIsAiming)
{
    IsAiming = bNewIsAiming;
}