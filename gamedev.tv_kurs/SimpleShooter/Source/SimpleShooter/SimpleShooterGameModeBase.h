// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SimpleShooterGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class SIMPLESHOOTER_API ASimpleShooterGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	//trenutno se nalazimo u default gamemode klasi koja je napravljena kad je napravljen projekat
	//Pravimo metod koji zelimo da overridujemo u nekoj od child klasa
	virtual void PawnKilled(APawn* PawnKilled);
	
};
