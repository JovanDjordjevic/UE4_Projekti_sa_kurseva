// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PawnBase.generated.h"


//forward declare ove klase, (umesto da includujemo ceo header (?) )
class UCapsuleComponent;
class AProjectileBase;
class UHealthComponent;

//ovu klasu ce naslediti tenkic i turret neprijatelji, tako da u nju ubacujemo ono sto je zajednicko za oba

UCLASS()
class TOONTANKS_API APawnBase : public APawn
{
	GENERATED_BODY()

private:
	//kapusulu korsitimo kao root component i collison za pawn (?)

	//trenutno se u blueprint editoru, klikom na svaku od ovih komponenti ne prikazuje nista sa desne strane, zato hocemo da stavimo da bude vidljivo, to radimo ispod

	//Visible anywhere znaci da ce ova osobina biti vidljiva i u base klasi i u njenim instancama (?), BluePrintReadOnly kaze da u blueprintu mzoemo samo da vidimo ove osobine (?)
	//Category dodajemo da bi nam bilo lakse da nadjemo ovu komponentu u Details panelu.
	//Posto smo stavili da su ove promenljive private, dobili bismo compile error zatos to pokusavamo da ih stavimod a budu vidljive blueprintovima koji imaju visibility specifier, zatos to se ocekuje da ce oni biti korisceni samo na public properties (?), zato stavljamo ovu meta osobinu
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* CapsuleComp;

	//nasem tenku(i neprijateljskim turretirma) su potrebni base i turret kao zasebne mesh kompoennte da bi mogli nezavisno da se okrecu (?)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BaseMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TurretMesh;

	//i tenku i neprijateljima treba da se odredi mesto sa koga ce krenuti nas projektil
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* ProjectileSpawnPoint;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UHealthComponent* HealthComponent;
 
	//slicno kao sto smo radili u ProjectileBase.h
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile Type", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AProjectileBase> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* DeathParticle;

	UPROPERTY(EditAnywhere, Category = "Effects")
	USoundBase* DeathSound;

	UPROPERTY(EditAnywhere, Category = "Effects")
	TSubclassOf<UCameraShake> DeathShake;

public:
	// Sets default values for this pawn's properties
	APawnBase();

	//Ovo sto je ispod u/* */ komentaru je auto generisano kad smo napravili BasePawn klasu. Mi hocemo da te metode nemamo ovde nego 
	//u njenoj child klasi PawnTank, tako da smo ih tamo premestili, ovde treba da se izbrisu, jer nam ovde ne trebaju ali sam ih samo zakomentarisao
	/*	// Called every frame
		virtual void Tick(float DeltaTime) override;
		// Called to bind functionality to input
		virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
		// Called when the game starts or when spawned
		virtual void BeginPlay() override;  */

	//virtual specifier radi slicno kao abstract u javi, uz malu razliku. Ako je neka funkcija virutal, to znaci da ona u toj klasi ima neku
	//default implementaciju ali da neka child klasa MOZE ALI NE MORA tu implementaciju da overriduje, dok abstract metode u javi nemaju nikakav kod u sebi i child klase MORAJU da je overriduju
	virtual void HandleDestruction();
	
protected:

	//ovde u baznoj klasi deklarisemo stvari okje su zajednicke za sve child klase
	void RotateTurret(FVector LookAtTarget);
	void Fire();
};
