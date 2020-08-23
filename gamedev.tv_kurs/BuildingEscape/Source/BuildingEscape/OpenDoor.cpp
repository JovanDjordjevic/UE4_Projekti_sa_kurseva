// Copyright Jovan Djordjevic 2020

#include "Components/AudioComponent.h"
#include "Components/PrimitiveComponent.h"
#include "OpenDoor.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

//out makro, isto kao uu Grabber.cpp, samo za oznacavanje out aprametra
#define OUT  

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	InitialYaw = GetOwner()->GetActorRotation().Yaw;
	CurrentYaw = InitialYaw;
	OpenAngle += InitialYaw;

	FindPressurePlate();
	FindAudioComponent();

	//NOTE: ova linija ispod nam ne treba vise, ali necu da je obrisem
	//GetWorld vraca pokazivac na nivo, GetFirstPlayerController nam ovde treba jer radimo single player igricu, GetPawn vraca lika koji taj prvi kontroler kontrolise
	//posto Pawn nasledjuje Actor klasu, a mi imamo actora koji treba da otvori vrata, tom akteru dodelimo ono sto vrate nase funkcije ispod
	//ActorThatOpens = GetWorld()->GetFirstPlayerController()->GetPawn();
}

void UOpenDoor::FindPressurePlate(){
	//zastita od null pointer chrasha
	if(!PressurePlate){
		UE_LOG(LogTemp, Error, TEXT("Actor: %s , has the open door component but no pressure plate!"), *GetOwner()->GetName());
	}
}

void UOpenDoor::FindAudioComponent(){
	//nadjemo audio component naseg objekta
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();
	if( !AudioComponent ){
		UE_LOG(LogTemp, Error, TEXT("%s has no audio component!"), *GetOwner()->GetName());
	}
}


// Called every frame
//DeltaTime je vreme izmedju frejmova koji su renderovani (tj to je frametime,60fps -> 16.66ms)
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//presure plate i actor treba da budu dodeljeni OpenDoor komponenti vrata unutar editora, inace imamo NULL pokazivace i nedefinisano ponasanje
	//if(PressurePlate && PressurePlate->IsOverlappingActor(ActorThatOpens)){  NOTE: Prvo smo radili sa ovim if-om pa smo promenili logiku programa
	if(TotalMassOfActors() > MassToOpenDoors){
		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds(); //funkc vraca broj sekundi otkako je zapoceta igrica, koristimo ovo kao neki timestamp
	}
	else{   // <=>  if(PressurePlate && !PressurePlate->IsOverlappingActor(ActorThatOpens)){
		if( (GetWorld()->GetTimeSeconds() - DoorLastOpened) > DoorCloseDelay ){
			CloseDoor(DeltaTime);
		}
	}

}

void UOpenDoor::OpenDoor(float DeltaTime){

	//UE_LOG(LogTemp, Warning, TEXT("%s"), *GetOwner()->GetActorRotation().ToString());
	//UE_LOG(LogTemp, Warning, TEXT("%f"), GetOwner()->GetActorRotation().Yaw);

	//1-integer, 1.0-double, 1.0f-float moze da se pise i 1.f
	//!!! x-roll y-pitch z-yaw
	//FRotator je struct koji sadrzi redom vrednosti za pitch, yaw, roll  !!!!!RAZLICITO OD TOGA KAKO SU NAVEDENI U ENGINEU
	//FRotator CurrentRotation = GetOwner()->GetActorRotation();  }  prvi nacin da se-
	//CurrentRotation.Yaw = 90.f;								  }   -nesto rotira(ovo rotira odmah cim pritisnemo Play, drugi nacin je FMath::Lerp ispod)
	//FRotator OpenDoor {0.f, 90.f , 0.f};  // pravimo FRotator koji cemo proslediti funkciji ispod, inace moze i {} i () u inicijalizaciji (?) 
	//GetOwner()->SetActorRotation(OpenDoor);	//setActorRotation prima FRotator na osnovu kog rotira objekat u cuijoj se klasi nalazimo

	//FRotator OpenDoor{0.f, OpenAngle, 0.f};
	//Lerp ce da primi 3 argumenta, starting_pos(u nasem slucaju trenutna pozicija), target_pos, % koliko ce se promeniti svaki put(float vrednost izmedju  0 i 1)
		//OpenDoor.Yaw = FMath::Lerp(CurrentYaw, OpenAngle, 0.02);
	//posto ovaj Lerp zavisi of FPS-a, tj na brzem racunaru brze ce se izvrsiti animacija, bolje je da koristimo FMath::FInterpConstantTo
	//FIinterpConstantTo radi linearnu interpolaciju, znaci u svakom koraku pomera za isto (?)
	//DeltaTime je argument koji se prosledjuje funkciji TickComponent (iznad), a ovo 45 je koliko stepeni zelimo da se pomeri za taj DeltaTime
		//OpenDoor.Yaw = FMath::FInterpConstantTo(CurrentYaw, OpenAngle, DeltaTime, 45);
	//primetimo da je ponasanje u igrici malo drugacije, ako radimo sa Lerp, animacija pocne brzo pa onda uspori pri kraju dok sa FInterpConstantTo, animacija je sve vreme iste brzine
	//ako zelimo da ipak uspori pri karaju imamo na raspolaganju i funkciju FMath::FInterpTo, samo njen cetvrti arg treba da bude manji nego za proslu:
	//OpenDoor.Yaw = FMath::FInterpTo(CurrentYaw, OpenAngle, DeltaTime, 2);
	//GetOwner()->SetActorRotation(OpenDoor);

	//ovde radimo sa relativnim pozicijama, hocemo da otvorimo sva vrata na sceni, ono do sad je otvaralo samo prva zatos to su druga vec psotavljena na yaw=90 stepeni
	CurrentYaw = FMath::Lerp(CurrentYaw, OpenAngle, DeltaTime * DoorOpenSpeed);  //ovde mnozimo sa DeltaTime da bi bilo nezavisno od FPS-a
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);

	CloseDoorSound = false;

	if(!AudioComponent) return;

	if(!OpenDoorSound){
		AudioComponent->Play();
		OpenDoorSound = true;
	}
}

void UOpenDoor::CloseDoor(float DeltaTime){
	CurrentYaw = FMath::Lerp(CurrentYaw, InitialYaw, DeltaTime * DoorCloseSpeed);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);

	OpenDoorSound = false;

	if (!AudioComponent) return;

	if( !CloseDoorSound){
		AudioComponent->Play();
		CloseDoorSound = true;
	}
}

float UOpenDoor::TotalMassOfActors() const{
	 float TotalMass = 0.f;

	 //nadjemo sve actore koji se preklapaju, ovaj niz je out parametar za funckiju GetOverlappingActors
	TArray<AActor*> OverlappingActors;
	if(!PressurePlate) return TotalMass; //zastita od nullptr,
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);

	for(AActor* Actor : OverlappingActors){
		//nas actor ima vise komponenti, i u komponenti UPrimitiveComponent se nalazi njegova masa, tako da nju treba da nadjemo i pozovemo GetMass()
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
		//UE_LOG(LogTemp, Warning, TEXT("%s is on the rpessure palte"), *Actor->GetName() );
	}
	return TotalMass;
}