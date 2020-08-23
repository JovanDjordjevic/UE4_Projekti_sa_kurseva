// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBase.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AProjectileBase::AProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	
	//NE koristimo Tick funkciju prvi put i ovo mozemo da stavimo na false
	PrimaryActorTick.bCanEverTick = false;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	//OnComponentHit je event koji se poziva kada komponenta pogodi (ili bude pogodjena), tj akd ima koliziju
	//AddDynamic je neki marko (?), prosledjujemo mu context objekat, dakle this i funkciju koja se poziva kada se desi OnComponentHit event
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);
	RootComponent = ProjectileMesh;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	//za peojectile movement ne moramo da radimo setup attachment kaos to smo imali do sada zato sto on nije deo nikakve hijerarhije i ne nasledjuju nista

	//podesavamo klasne promenljive ove instance klase UProjectileMovementComponent
	//OBAVEZNO u BP_ProjectileBase stiklirati Simulate Physics i kod Static Mesh stiklirati Movable, u suprotnom ce se projektil spawnovati ali se nece kretati!!!
	ProjectileMovement->InitialSpeed = MovementSpeed;
	ProjectileMovement->MaxSpeed = MovementSpeed;
	//InitialLifespan je polje koje ima svaki actor (?), to je broj sekundi koje ce se zadrzati na sceni, nakon cega ce pozvati Destroy() funkciju i izbrisati se iz sveta
	InitialLifeSpan = 3.0f;

	//kacimo komponentu za efeke traga projektila
	ParticleTrail = CreateDefaultSubobject<UParticleSystemComponent>( TEXT("Particle Trail") );
	ParticleTrail->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
	//posto projektil ne psotoji sve vreme u svetu, zelimo da se pusti launch sound cim se on spawnuje pa mozemo to ovde u BeginPlay
	UGameplayStatics::PlaySoundAtLocation(
		this,      			//context object
		LaunchSound, 		//koji zvuk pustamo
		GetActorLocation() //gde u sceni pustamo zvuk
	);
	//imamo na raspolaganju i UGameplayStatics::PlaySound2D ako hocemo da zvuk bude iste jacine svugde na sceni, mzoemo npr za ingame muziku
}


//pozivace se svaki put kada se pozove OnComponentHit
//ova OnHit funkcija je dynamic delegate (?), a ovi parametri koji se prosledjuju su oni koje prima OnComponentHit (? nisam bas razumeo sta radi, ni sta su pvi dynamic delegates)
//nasao ih je tako sto je desniklik na OnComponentHit / go to definition i skrolovao gore da vidi te parametre 
void AProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit){	

	AActor* MyOwner = GetOwner();
	if( !MyOwner ) {
		return;
	}

	//OtherActor ne sme da bude null, ne sme da bude this (za slucaj da imamo neke ugnjezdene komponente koje mogu da ucestvuju u koliziji 
	//i ne sme da bude MyOwner da bi se izblegla stituacija da se unisti Pawn koji spawnuje projektil
	//ovo sme zato sto smo u PawnBase.cpp stavili 	TempProjectile->SetOwner(this);   (?)
	if( OtherActor!= NULL && OtherActor != this  && OtherActor != MyOwner){
		UGameplayStatics::ApplyDamage(
			OtherActor,  //ono sto treba da bude osteceno
			Damage,      //koliko dmg, deklarisali smo u .h
			MyOwner->GetInstigatorController(),    //Controller koji je odgovoran za dmg (e.g. igrac koji je pucao)
			this,								  //actor koji zapravo izaziva dmg  (e.g. granata,projektil), ovde je this zato sto se nalazimom u projektil klasi
			DamageType				//klasa koja opisuje koji damage se radi
		);
		//efekti, slicno kao u PawnBase.cpp
		UGameplayStatics::SpawnEmitterAtLocation(this, HitParticle, GetActorLocation());
		//zvuk, slicno kao gore u BeginPlay
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());

		//ovako se radi camera shake efekat, ovo HitShake cemo u editoru staviti da bude ona blueprint klasa koju smo napravili za camera shake pre ovoga i u njoj podesili odgovarajuce parametre
		GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake( HitShake );

		//da izbrisemo projektil iz igre
		Destroy();
	}
	
}


