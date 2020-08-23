// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldPosition.h"
#include "GameFramework/Actor.h"  		//potrebno za AActor* (?)

// Sets default values for this component's properties
UWorldPosition::UWorldPosition()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWorldPosition::BeginPlay()
{
	Super::BeginPlay();

	//za ispisivanje runtime poruka, koristimo UE_LOG, on u log  koji navedemo kao prvi argument, ispise poruku koja je treci argument,
	//drugi argument sluzi za bojenje teksta, Warning-zuta, Error-crvena, Display-siva
	//ove poruke vidimo iz: Window/Developer Tools/Output Log
	//ove poruke ce se prikazati po jednom ZA SVAKOG aktera cija je ovo komponenta, znaci npr 2 aktera -> 6 poruka ukupno
	//UE_LOG(LogTemp, Warning, TEXT("HELLO THERE"));
	//UE_LOG(LogTemp, Error, TEXT("GENERAL KENOBI"));
	//UE_LOG(LogTemp, Display, TEXT("YOU ARE A BOLD ONE"));

	//ispisivanje imena objekta za koji je ova komponenta zakacena
	FString ObjectName = GetOwner()->GetName();
	UE_LOG(LogTemp, Warning, TEXT("Object name is: %s"), *ObjectName);

	//ispisivanje X Y Z koordinata naseg actora koji ima ovu komponentu. GetActorLocation vraca koordinate u obliku FVector (to je strukura cija su polja x, y z koordinate)
	//posto ih zelimo u string obliku, nad time sto je vraceno pozivamo i ToString()
	//imamo i funkciju GetActorTransform koja vraca o koordinate i rotaciju i skaliranje, to ce vratiti nesto sto je oblika FTransform, i onda nad tim mozemo pozvati npr GetLocation da izvucemo vektor koordinata
	//npr: FString ObjectPosition = GetOwner()->GetActorTransform().GetLocation().ToString();
	FString ObjectPosition = GetOwner()->GetActorLocation().ToString();
	UE_LOG(LogTemp, Warning, TEXT("Object is at position: %s"), *ObjectPosition);
	
}


// Called every frame
void UWorldPosition::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

