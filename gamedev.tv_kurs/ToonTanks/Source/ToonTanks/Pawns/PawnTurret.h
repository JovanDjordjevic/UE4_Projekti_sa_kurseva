// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PawnBase.h"
#include "PawnTurret.generated.h"

//forward declare
class APawnTank;

//ovo ce biti klasa za AI turret, ovde nam ne treba SetupPlayerInputComponent funkcija koju smo do sad imali u klasama bas zatos to je AI klasa
UCLASS()
class TOONTANKS_API APawnTurret : public APawnBase
{
	GENERATED_BODY()

	public:	

	//kopirali ovo iz nekog od ostalih .h fajlova zato sto nije auto generisano
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//ovde u child klasi nije bilo obavezno da se stavi virtual, alidobra praksa je da se stavi, a override mora obavezno ako je virutal metoda u parent klasi
	//NOTE: virtual, i override stoje samo u deklaraciji funkcije u headeru, u cpp implementaciji ne pisemo
	virtual void HandleDestruction() override;

	private:

		//ovo cemo proslediti tajmeru, turret ce pucati svake dve sekunde
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		float FireRate = 2.0f;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		float FireRange = 500.0f;

		//da bismo koristili tajmer potrebno nam je FTimerHandle, to ce nam dozvoliti da bindujemo i unbindujemo funkciju tokom izvrsavanja
		FTimerHandle FireRateTimerHandle;
		APawnTank* PlayerPawn;

		//hocemo da turret puca na neki vremenski interval, radicemo sa tajmerima
		void CheckFireCondition();
		float ReturnDistanceToPlayer();
	
	protected:
		// Called when the game starts or when spawned
		virtual void BeginPlay() override;
};
