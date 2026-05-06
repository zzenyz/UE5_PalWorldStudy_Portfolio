// Copyright Epic Games, Inc. All Rights Reserved.

#include "PalWorld_StudyGameMode.h"
#include "PalWorld_StudyCharacter.h"
#include "UObject/ConstructorHelpers.h"

APalWorld_StudyGameMode::APalWorld_StudyGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
