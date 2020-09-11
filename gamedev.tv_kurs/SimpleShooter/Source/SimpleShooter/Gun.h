// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

UCLASS()
class SIMPLESHOOTER_API AGun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGun();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void PullTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)	//znaci da je vidljivo svuda ali da sam pokazivac nije editable (?)	
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Mesh;	//iz nekog razloga, asseti za puske koji su nama dati nisu Static meshevi nego Skeletal

	UPROPERTY(EditAnywhere)			//u blueprintu treba da selektujemo koji efekat cemo koristiti za ovo
	UParticleSystem* MuzzleFlash;

	//nas promenljiva koja ce sadrzxati zvuk
	//NOTE: ova promenljiva je tipa USoundBase* jer je to tip koji zahteva funkcija koju cemo pozivati
	//ali audio fajlovi u UE4 su tipa USoundWave (?) ali USoundWave nasledjuje USoundBase tako da je to okej
	//NOTE2: prvo smo uradili ovako standardno, a onda smo napravili Sound Cue asset koji nam daje random zvukove 
	//i onda smo to ubacili u BP_Rifle blueprint tamo gde bi stavili obican zvuk. Nas c++ kod ne mora da se menja zbog toga
	UPROPERTY(EditAnywhere)
	USoundBase* MuzzleSound;

	UPROPERTY(EditAnywhere)			//u blueprintu treba da selektujemo koji efekat cemo koristiti za ovo
	UParticleSystem* ImpactEffect;

	UPROPERTY(EditAnywhere)
	USoundBase* ImpactSound;

	UPROPERTY(EditAnywhere)		
	float MaxRange = 1000.f;		//treba nam za line trace, tj hocemo da zrak ide od puske do maksimalnog dometa

	UPROPERTY(EditAnywhere)	
	float Damage = 10.f;

	//funckija iz lekcije 209, samo refaktorisemo funkciju PullTriger da bude lepsa i citljivija (ali zadrzao sam sve komentare tako da je ovo bilo nepotrebno)
	//oba ova parametra su OUT parametri!
	bool GunTrace(FHitResult& Hit,FVector& ShotDirection);
	//takodje iz 209
	AController* GetOwnerController() const;
};
