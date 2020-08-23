// Fill out your copyright notice in the Description page of Project Settings.


#include "PawnBase.h"
#include "Components/CapsuleComponent.h"
#include "ToonTanks/Actors/ProjectileBase.h"
#include "ToonTanks/Components/HealthComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APawnBase::APawnBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//ovo dodje kao neki konstruktor  za nas CapsuleComp (?)
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsulle Collider"));
	RootComponent = CapsuleComp; //sve klase imaju svoj root component, posto je za sada capsule component jedino sto je napravljeno, UE ce sam psotaviti to kao Root ali bolje da mi eksplicitno navedemo

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	//zelimo da mesh nasledi movement i rotaciju od CapsuleComponent, tj da pravilno pozicioniramo BaseMesh u hijerarhiji, settupAttachment uzima jedan argument i to je novi parent component
	BaseMesh->SetupAttachment(RootComponent);

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret Mesh"));
	//zelimo da turret bude attachment od BaseMesh
	TurretMesh->SetupAttachment(BaseMesh);

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(TurretMesh);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Compoennt"));
	//za health neammo setup attachment zato sto je to actor compoennt i nema fizicko prisustvo u svetu (?)
}

void APawnBase::RotateTurret(FVector LookAtTarget){
	//azuriramo TurretMesh rotaciju tako da gleda ka LookAtTarget lokaciji (?)
	//LookAtTargetCleaned zelimo da se pomera samo horizontalno, tj da Z koordinata ostane nepromenjena
	FVector LookAtTargetCleaned = FVector(LookAtTarget.X, LookAtTarget.Y, TurretMesh->GetComponentLocation().Z);
	FVector StartLocation = TurretMesh->GetComponentLocation();
	
	FRotator TurretRotation = FVector( LookAtTargetCleaned - StartLocation ).Rotation();
	//postavljamo rotaciju turreta na osnovu rotatora koji smo napravili iznad
	TurretMesh->SetWorldRotation( TurretRotation );
}

void APawnBase::Fire(){
	//UE_LOG(LogTemp, Warning, TEXT("firing!"));
	//zastita od null pointera, ako zaboravimo da podesimo ovo u editoru, slicno kao ostale null pointer provere
	if( ProjectileClass ){
		//primeti da ne trazimo lokaciju i rotaciju sa GetOwner()->GetActorLocation/Rotation zato sto (?)
		//ProjectileSpawnPoint je komponenta sceme tako da njenu poziciju trazimo sa GetComponentLocation()
		FVector SpawnLocation = ProjectileSpawnPoint->GetComponentLocation();
		FRotator SpawnRotation = ProjectileSpawnPoint->GetComponentRotation();
		
		//spawnuje actora klase u <> na datoj poziciji
		AProjectileBase* TempProjectile = GetWorld()->SpawnActor<AProjectileBase>(
			ProjectileClass,      //klasa koju pokusavamo da spawnujemo
			SpawnLocation,		  //lokacija na kojoj spawnujemo
			SpawnRotation		  //sa kojom rotacijom spawnujemo
		);

		//posto ova instanca spawnuje projektil ima smisla da ona bude njen owner, ovo ce nam takodje spreciti racunanje kolizije
		//projektila i njegovog ownera, dakle ne moze projektil da osteti tenk/turret koji ga je ispalio
		TempProjectile->SetOwner(this);
	}
}

void APawnBase::HandleDestruction(){
	//spawnuje particle emiter na datoj lokaciji, za efekte
	UGameplayStatics::SpawnEmitterAtLocation(
		this, 			  	//objekat iz kog moze da se izvuce world context
		DeathParticle, 	  	//koje cestice spawnuje
		GetActorLocation()	//gde ih spawnuje
		//ima i verzija ove funkcije koja prima rotaciju ali nama to ovde ne treba pa korsitimo ovaj overload, ako ne navedemo stavice se rotacija 0 po svakoj osi po defaultu (?)
	);
	//slicno kao sto smo radili u projektilu
	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	//ovako se radi camera shake efekat, ovo DeathShake cemo u editoru staviti da bude ona blueprint klasa koju smo napravili za camera shake pre ovoga i u njoj podesili odgovarajuce parametre
	GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake( DeathShake );
}



	//Ovo sto je ispod u/* */ komentaru je auto generisano kad smo napravili BasePawn klasu. Mi hocemo da te metode nemamo ovde nego 
	//u njenoj child klasi PawnTank, tako da smo ih tamo premestili, ovde treba da se izbrisu, jer nam ovde ne trebaju ali sam ih samo zakomentarisao
	/*// Called when the game starts or when spawned
	void APawnBase::BeginPlay(){
		Super::BeginPlay();	
	}
	// Called every frame
	void APawnBase::Tick(float DeltaTime){
		Super::Tick(DeltaTime);
	}
	// Called to bind functionality to input
	void APawnBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent){
		Super::SetupPlayerInputComponent(PlayerInputComponent);
	}*/

