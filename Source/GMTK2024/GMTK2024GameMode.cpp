// Copyright Epic Games, Inc. All Rights Reserved.

#include "GMTK2024GameMode.h"
#include "GMTK2024Character.h"
#include "UObject/ConstructorHelpers.h"

AGMTK2024GameMode::AGMTK2024GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
