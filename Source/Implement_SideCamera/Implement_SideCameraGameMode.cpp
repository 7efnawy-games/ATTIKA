// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Implement_SideCameraGameMode.h"
#include "Implement_SideCameraCharacter.h"
#include "UObject/ConstructorHelpers.h"

AImplement_SideCameraGameMode::AImplement_SideCameraGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
