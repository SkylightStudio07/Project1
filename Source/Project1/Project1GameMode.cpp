// Copyright Epic Games, Inc. All Rights Reserved.

#include "Project1GameMode.h"
#include "Project1Character.h"
#include "UObject/ConstructorHelpers.h"

AProject1GameMode::AProject1GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
