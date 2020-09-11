// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

//forward declare, ovo nam daje mogucnost da smanjimo trajanje kompilacije time sto ne ukljucimo ceo header fajl
class AGun;

UCLASS()
class SIMPLESHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Actor klasa ima ugradjenu metodu za primanje dmg, mi hocemo da je overridujemo u nasoj klasi
	//iz Actor.h smo prekopirali definiciju, nije obavezno da u child klasi pisemo virtual i override ali smo to ostavili za svaki slucaj, i da bi bilo jasnije
	//kada overridujemo virtual funkcije, obavezno moramo da pazimo da li je ta funkcija public/private itd, tj kada nadjemo njenu definiciju u header fajlu treba da skrolujemo
	//da vidimo u kom se odeljku nalazi i u tom istom odeljku da ga ubacimo u nasoj klasi
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void Shoot();

	UFUNCTION(BlueprintPure)	//UFUNCTION omogucava da je ova funkcija vidljiva u blueprintu, ali posto nama treba da se ovo poziva u blueprintu, moramo da dodamo i BlueprintCallable (ili BluePrintPure, koja podrazumeva da je i BlueprintCallable + jos nesto)
	bool IsDead() const;      //za dohvatanje bool vrednosti u AnimGraphu naseg ABP_ShooterCharacter blueprinta, const zatos to ne zelimo da ova funkcija menja bilo sta
	//BluePrintPure se razlikuje od BlueprintCallable po tome sto taj cvor nece imati onaj beli execution pin na vrhu (?), tj nece menjati apsolutno nista pri svom pozivu vec ce samo imati neki output (?)
	
	//u WBP_HUD blueprintu hocemo da nekako povezemo nas health bar sa vrednoscu health promenljive
	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);

	//za koriscenje dzojstika
	UPROPERTY(EditAnywhere)
	float RotationRate = 10.f;
	void LookUpRate(float AxisValue);
	void LookUpRight(float AxisValue);

	UPROPERTY(EditDefaultsOnly)  //zato sto ne zelimo da ovo editujemo in runtime, jer je na pcoetku vec pozvan BeginPlay i puska vec spawnovana
	TSubclassOf<AGun> GunClass;  //treba nam Gun klasa
	UPROPERTY()		//ako ne stavimo nista, ovaj UPROPERTRY je skriven (?)
	AGun* Gun;		//kada imamo instancu Gun klase ovde cemo da je cuvamo

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 100.f;
	UPROPERTY(VisibleAnywhere)
	float Health;	
};
