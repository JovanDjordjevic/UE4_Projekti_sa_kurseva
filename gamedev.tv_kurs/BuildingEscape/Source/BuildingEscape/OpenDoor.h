// Copyright Jovan Djordjevic 2020

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TriggerVolume.h"
#include "OpenDoor.generated.h"   //obavezno na dno svih includova!


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGESCAPE_API UOpenDoor : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOpenDoor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void OpenDoor(float DeltaTime);
	void CloseDoor(float DeltaTime);
	float TotalMassOfActors() const;
	void FindPressurePlate();
	void FindAudioComponent();

private:
	float InitialYaw;
	float CurrentYaw;

	UPROPERTY(EditAnywhere)
	float MassToOpenDoors = 50.f;

	UPROPERTY(EditAnywhere)  //BEZ ; ovo nam dozvoljava da ovu vrednost ispod (u ovom slucaju TargetYaw) menjamo direktno u UE editoru. videce se sa desne strane nako sto kliknemo na vrata i odaberemo komponentu OpenDoor
	float OpenAngle = 90.f;  //ovo sto ubacimo default vrednost ce biti vidljivo u UE editoru nakon kompilacije
							 //ovo nam omogucava sledece: psoto svaka vrata imaju svoje OpenDoor komponente, mozemo za svaku psoebno ubaciti u editoru TargetYaw i samo dotle ce se otvoriti
	float DoorLastOpened = 0.f;

	UPROPERTY(EditAnywhere)
	float DoorCloseDelay = 0.5f;

	UPROPERTY(EditAnywhere)
	float DoorOpenSpeed = 0.8f;

	UPROPERTY(EditAnywhere)
	float DoorCloseSpeed = 2.f;

	UPROPERTY(EditAnywhere)
	ATriggerVolume* PressurePlate = nullptr;  //presure plate koji ce otvoriti vrata

	//NOTE: tokom razvoja je promenjena logika i ne koristi se vise ali necu da izbrisem
	//UPROPERTY(EditAnywhere)
	//AActor* ActorThatOpens = nullptr;    //ono sto ce otvoriti vrata. Igrac us vetu je klasa APawn, ali posto ta klasa nasledjuje AActor mi ovde stavljamo to da bi bilo uopstenije

	//UE standard kaze da je pozeljno da sve sto ima prefiks U bude izlozeno editoru, tj da se stavi ovaj UPROPERTY, ne mora nuzno EditAnywhere(?)
	UPROPERTY()
	UAudioComponent* AudioComponent = nullptr;

	//ovi boolovi su nap porebni da bismo samo jednom pustili zvuk, u suprotnom ce se vrteti vise puta
	bool OpenDoorSound = false;
	bool CloseDoorSound = true;
};