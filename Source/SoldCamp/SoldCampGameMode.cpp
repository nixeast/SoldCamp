// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoldCampGameMode.h"
#include "SoldCampCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASoldCampGameMode::ASoldCampGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
