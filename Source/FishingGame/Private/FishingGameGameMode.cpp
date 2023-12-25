// Copyright Epic Games, Inc. All Rights Reserved.

#include "FishingGameGameMode.h"
#include "FishingGamePlayerController.h"
#include "FishingGameCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFishingGameGameMode::AFishingGameGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/FishingGame/Blueprints/Character/BP_FishingCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}