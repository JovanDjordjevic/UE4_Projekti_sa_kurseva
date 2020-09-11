// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SimpleShooterGameModeBase.h"
#include "KillEmAllGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SIMPLESHOOTER_API AKillEmAllGameMode : public ASimpleShooterGameModeBase
{
	GENERATED_BODY()

public:
	//overridujemo ovu funkciju iz bazne gamemode klase
	//ovaj killemall gamemode cemo staviti kao default gamemode (prvo treba da napravimo BP koji nasledjuje ovu klasu i onda taj da stavimo kao default
	//tako sto u main viewportu gore kliknemo Blueprints/tamo gde je Gamemode/Select Gamemode Base Class i odaberemo taj blueprint,
	//takodje u samom blueprintu treba da se postaramo da je ova klasa parent kalsa tom blueprintu kao i da su postavljene dobre default vrednosti za pawn itd)
	virtual void PawnKilled(APawn* PawnKilled) override;	

private:
	//nasa funckija u kojoj cemo pozvati ugradjenu GameHasEnded funkciju za sve Controllere (i nase i AI)
	void EndGame(bool bIsPlayerWinner);
};
