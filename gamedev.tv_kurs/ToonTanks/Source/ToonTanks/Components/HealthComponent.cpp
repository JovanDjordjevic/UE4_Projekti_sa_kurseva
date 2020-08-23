// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthComponent.h"
#include "ToonTanks/GameModes/TankGamemodeBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	//NE treba nam tick komponenta pa mozemo da stavimo ovo na false
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Health = DefaultHealth;

	//GetGameMode uzima kontekst sveta (?) i vraca trenutni aktivni gamemode, kastujemo jer GetGameMode vraca klasu AGameModeBase
	GameModeRef = Cast<ATankGamemodeBase> (UGameplayStatics::GetGameMode( GetWorld() ));
	
	//slicno kao sto smo imali u projectile.cpp
	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::TakeDamage);
}

void UHealthComponent::TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser){
	//ako je damage koji se prima = 0 onda je to verovatno neki error(?) i ne zelimo da radimo nista
	//a ovo || sluzi da se ova funkcija nebi slucajno pozivala za nekog actora koji je vec mrtav
	if(Damage == 0 || Health <= 0){
		return;
	}
	//clamp radi tako sto ovaj prvi argument izracuna i onda ga "trimuje" levo i desno da upadne u interval koji cine druga dva argumenta
	//npr ako hp-dmg = -30, a donja granica je 0, vratice se 0. Koirscennjem ovoga smo se postarali da hp ne mzoe da padne ispod nule i da ne moze
	//da bude veci od default health, NOTE: drugi i treci argumenti MORAJU da budu float, u suprotnom dobijamo error
	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);
	//UE_LOG(LogTemp, Warning, TEXT("%s ima jos %f hp"), *DamagedActor->GetName(), Health);

	if(Health <= 0){
		if( GameModeRef ){
			//primetimo da health komponentu imaju i tenk i tureti, pisemo ovo tako da radi za oba, i osladnjamo se na nasu gamemode klasu da pravi razliu izmedju toga ko je unisten
			GameModeRef->ActorDied( GetOwner() );
		}else{
			UE_LOG(LogTemp, Warning, TEXT("health komponenta nema reference na gamemode"))
		}
	}
}



