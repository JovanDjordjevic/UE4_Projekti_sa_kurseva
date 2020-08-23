// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"

//forawrd declare
class UProjectileMovementComponent;

UCLASS()
class TOONTANKS_API AProjectileBase : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))	
	UStaticMeshComponent* ProjectileMesh;

	//UParticleSystemComponent je scene komponenta, mozemo da je dodamo nekom actoru, moze da ima poziciju u svetu itd
	//ovo ne radimo sa UParticleSystem* kao ostale efekte do sada zatos to zelimo se kontinualno tokom izvrsavanja spawnuje trag iza projektila u zavisnosti od njegove pozicije (?)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))	
	UParticleSystemComponent* ParticleTrail;
	
	//implementiramo template klasu. Ovo je nacin da se tip cuva kao vrednost u promenljivoj, navedemo bazni tip koji moze da 
	//se sadrzi, u nasem slucaju UDamageType (?), videcemo ovo u editoru kad otvorimo BP_ProjectileBase sa desne strane u odeljku Damage i tu mozemo da odaberemo, mi biramo DamageType, to je default(?)
	//EditDefaultsOnly kaze da ovo sme da se menja iz editora samo u arhetipu (?)
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move", meta = (AllowPrivateAccess = "true"))
	float MovementSpeed = 1300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	float Damage = 50.0f;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* HitParticle;

	UPROPERTY(EditAnywhere, Category = "Effects")
	USoundBase* HitSound;
	UPROPERTY(EditAnywhere, Category = "Effects")
	USoundBase* LaunchSound;

	UPROPERTY(EditAnywhere, Category = "Effects")
	TSubclassOf<UCameraShake> HitShake;

	//dynamic delegates svi moraju da se deklarisu kao UFUNCTION()
	UFUNCTION()
	//HitComp - komponenta koja biva udarena (ova nasa (?))
	//OtherActor - komponenta koju nasa komponenta udara (?)
	//OtherComp - specificna komponenta koju smo udarili u OtherActoru, za slucaj kada je OtherActor sastavljen od neki submesheva ili subclasseva (?)
	//NormalImpules -  }   ove dve stvari sluze za brzinu
	//Hit			-  }   i silu kojom je sudareno, i jos neke stvari ali ne bavimo se time ovde(?)
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:	
	// Sets default values for this actor's properties
	AProjectileBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	//ovo u /**/ je auto generisano i ne treba nam u ovoj klasi
	/*public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;*/

};
