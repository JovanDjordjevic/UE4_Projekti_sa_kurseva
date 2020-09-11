// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent> (TEXT("Root"));
	SetRootComponent(Root);
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
}

void AGun::PullTrigger(){
	//UE_LOG(LogTemp, Warning, TEXT("PEW!"));
	//zelimo ponasanje da svaki put kada pucamo napravimo novi particle effect na vrhu cevi puske
	UGameplayStatics::SpawnEmitterAttached(
		MuzzleFlash,     //koji particle system
		Mesh,			 //za sta kacimo
		TEXT("MuzzleFlashSocket")	//socket na koji kacimo. Ovaj socket je vec postojao u Meshu nase puske (da nije, morali bismo samo da sami dodamo novi socket),
									//mozemo da vidimo ovo kad iz content browsera otvorimo Rifle skeletal mesh fajl
	);

	//funkcija za zvucni efekat, ranije smo videli SpawnSoundAtLocation, ali ova funkcija nas zvuk kaci za odredjeni socket u meshu
	//tako da ce se uvek kretati zajedno sa onim na sta ga nakacimo
	//prvi argument je USoundBase* zvuk koji hocemo, a druga 2 su ista kao u gore u SpawnEmmiterAttached
	//NOTE: koristimo isti socket kao za SpawnEmmiterAttached, to je skroz validno, na socketu moze biti vise komponenti
	UGameplayStatics::SpawnSoundAttached(MuzzleSound, Mesh, TEXT("MuzzleFlashSocket"));

	//iz lekcije 209, radili smo refactor, GunTrace sada radi ono sto je bio ovde, a Hit i ShotDirection su OUT parametri
	FHitResult Hit;
	FVector ShotDirection; 
	bool bSuccess = GunTrace(Hit, ShotDirection);

	if (bSuccess){
		//hocemo da iscrtamo hit mark na mestu gde smo naisli na presek sa zrakom
		//za pocetak samo DebugPoint a posle i pravi particle effect
		/*DrawDebugPoint(
			GetWorld(),		//u kom nivou
			Hit.Location,	//gde
			20,				//velicina
			FColor::Red,	//boja
			true			//presistent
		);*/

		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),				 //context
			ImpactEffect,			 //nas particle system
			Hit.Location,			 //gde
			ShotDirection.Rotation() //rotacija, slicno kao sto od rotacije dobijamo vektor tako od vektora dobijamo rotaciju
		);
		//na mestu gde se desio Hit hocemo nekakav zvuk
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),		//world context object
			ImpactSound,	//USoundBase* koji zvuk pustamo na lokaciji
			Hit.Location,	//gde se pusta zvuk
			0.4				//volume multiplier, float vrednost [0,1], ovo ima default vrednost, mozglo je i bez ovoga, ali bilo je glasno
		);

		//primanje dmg	
		//dohvatamo actora koji treba da primi dmg, moramo samo da proverimo null!
		AActor* HitActor =  Hit.GetActor();
		if ( HitActor != nullptr){
			//FPointDamageEvent je struktura koja handjule dmg sa jednom impact lokacijom, ovo je njen konstruktor
			FPointDamageEvent DamageEvent(
				Damage,		//koliko
				Hit,		//HitResult
				ShotDirection,	//odakle je hitac dosao
				nullptr		//TSubclassOf<UDamageType> InDamageTypeClass, ovaj argument koristimo kada imamo vise razlicitih vrsta dmg, npr fire dmg, ice dmg itd, mi ne koristimo to ovde pa prosledjujemo null
			);

			AController* OwnerController = GetOwnerController();

			HitActor->TakeDamage(
				Damage,		 //koliko dmg prima
				DamageEvent, //Damage event struct koju smo napravili iznad
				OwnerController,  //controller koji je uzrok primanja dmg
				this			  //instigator/izazivac, uzrok dmg, tj ova puska
			);
		}
	}
	//hocemo da ilustrujemo kako funkcionise DrawDebugCamera, kad pozovemo ovu funkciju iscrtace se crvena kamera na ekranu na onom mestu 
	//koje prosledimo kao argument, mi hocemo da se stvori tamo gde je pozicija kamere iz koje gledamo
	/*	DrawDebugCamera(
			GetWorld(),			//u kom nivou 
			Location,			//gde ce se iscrtati
			Rotation,     		//sa kojom rotacijom
			90,					//FOV
			2,					//Scale
			FColor::Red,		//boja
			true				//persistent lines
		);
	*/   
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AGun::GunTrace(FHitResult& Hit,FVector& ShotDirection){

	AController* OwnerController = GetOwnerController();
	if( OwnerController == nullptr ){ 
		//ako nemamo OwnerController ne mzoemo da uradimo GunTrace pa samim tim sigurno ne mzoemo da pogodimo nista
		return false;
	}

	FVector Location; 		// }out parametri
	FRotator Rotation;		// }za GetPlayerViewPoint
	//ova funkc vraca lokaciju i rotaciju kamere iz koje gledamo scenu
	//hocemo na neki nacin ovo da iskoristimo kad pucamo. Za first i third person igrice prirodno je da zrak za ray casting ide iz view pointa do mete a ne iz vrha cevi do mete (?)
	OwnerController->GetPlayerViewPoint(Location, Rotation);

	//vektor koji cuva odakle je hitac dosao, slicno kao gore samo sada nam je dovoljan jedinicni vektor a posto je suprotnog smera onda samo - ispred
	//OUT PARAMETAR!
	ShotDirection = -Rotation.Vector();

	//LineTrace ByObjectType ide niz liniju i gleda da li nailazi na objekat datog tipa, ali to je i nedostatak jer ce samo da sece ove tipove (?)
	//LineTrace ByChannel nam daje vise mogucnosti za presek, mzoemo da napravimo nas Channel koji kaze koji tipovi objekata blokiraju zrak a koji ga propustaju
	//nov Trace Channel pravimo u ProjectSettings/Engine/Collision, kad ga napravimo i klikenmo na njega, i ovorimo Preset ispod, mozemo da navedemo kako ce reagovati kad naidje na neki objekat (?)
	//primetimo da je kanal Enum, neki vec postoje ugradjeni. Za nas Bullet Channel koji smo napravili dodeljen je jedan od ovih enuma. Taj dodeljeni trazimo tako sto otvorimo nas folder gde nam je projekat
	//otvorimo Config i otvorimo DefaultEngine.ini, tu ctrl+f Bullet (tj ime kanala) i trebalo bi da u prvom rezultatu imamo nesto 	Channel=...
	//u nasem projektu dodeljeno je ovako:   Channel=ECC_GameTraceChannel1
	//trebaju nam argumenti za LineTrace, End ce biti kraj, ovo sto smo napisali Rotation.Vector() vraca jedinicni vektor usmeren u pravcu ove rotacije
	//mnozenje sa MaxRange je da bismo odredili daljinu zraka
	FVector End = Location + Rotation.Vector() * MaxRange;

	//hocemo nekako da oznacimo da puska ne moze da pogodi neke actore. Ako nemamo ovo, desava se da nas AI moze sam sebe da upuca zato se zrak krece iz njegovog oka
	//i onda se sece sa njegovom collision kapsulom, dakle actori koji treba da se ignorisu je sama puska i owner te puske
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());

	//LineTraceSingleByChannel  vraca true ako naidje na presek
	//NOTE: OVAJ HIT JE OUT PARAMETAR I U GunTrace() I U LineTraceSingleByChannel
	return GetWorld()->LineTraceSingleByChannel(
		Hit,		//prvi objekat na koji naidjemo koji blokira zrak  
		Location,	//odakle krece zrak, u nasem slucaju hocemo da krene iz viewpointa
		End,		//dokle zrak ide
		ECollisionChannel::ECC_GameTraceChannel1,	//onaj kanal iz DefaultEngine.ini
		Params		//dodatni parametri, mi smo naveli koji actori zelimo da budu ignorisani
	);
}

AController* AGun::GetOwnerController() const{
	//treba nam controller. Posto smo Gun klasu spawnovali pri izvrsavanju i stvili u ShooterCharacter.cpp da je on owner ove klase
	//ovde mozemo da pozovemo GetOwner(), ali moramo prvo da ga kastujemo u AShooterCharacter, tojest bolje u malo opstiju klasu APawn, jer GetOwner 
	//vraca nesto sto je tipa AActor, a nama treba APawn zato sto samo pawnovi mogu da imaju controller
	APawn* OwnerPawn = Cast<APawn>( GetOwner() );
	if( OwnerPawn == nullptr ) 
		return nullptr;

	return OwnerPawn->GetController();	
}