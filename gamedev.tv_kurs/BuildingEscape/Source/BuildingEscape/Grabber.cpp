// Copyright Jovan Djordjevic 2020

#include "DrawDebugHelpers.h" 
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Grabber.h"

//makro samo za oznacavanje out parametara, ne radi nista sam po sebi
#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandle();
	SetupInputComponent();
}


void UGrabber::SetupInputComponent(){
	//input component nam sluzi za key binding
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if(InputComponent){
		UE_LOG(LogTemp, Warning, TEXT("input cmponent found on %s"), *GetOwner()->GetName());
		//funkciji BindAction, prosledjujemo 
			//1) ime koje mora da bude ISTO kao ime akcije koje smo naveli za action ampping u project settings,
			//2) input event (IE), npr Pressed ako hocemo se nesto desi kada pritisnemo taster, odnosi se na taster koji smo stavili u project settings
			//3) objkeat koji izvrsava kod koji pisemo (ovde this, tj ova konkretna instanca default pawna)
			//4) ADRESA ka funkciji koja treba da se izvrsi, NOTE: ne idu zagrade na kraju funkcije zato sto ne prosledjujemo nikakve argumente vec samo dajemo adresu
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	//else{     //ova provera nam u sustini ne treba zato sto input compnent uvek postoji (?)
	//	UE_LOG(LogTemp, Error, TEXT("objekat %s nema input komponentu!"), *GetOwner()->GetName());
	//}
}


void UGrabber::FindPhysicsHandle(){
	//proveravamo physics handle komponentu koju smo dodali u ans default pawn blueprint, inicijalizovali UPhysicsHandleComponent* u .h fajlu
	//obavezno je bilo potrebno da ga tamo inicijalizujemo na nullptr da bi izbelgi chrash u slucajevima kada se physics handle komponenta ne ucita (?)
	//FindComponentByClass funkcija vraca PRVI objekat klase koju navedemo u <> koju nas Owner poseduje, reminder: getowner vraca objekat koji sadrzi komponentu u kojoj pisemo kod, u ovom slucaju owner je dafault pawn jer smo njemu dali grabber komponentu
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	//zastita od nullpointer chrasha
	if(!PhysicsHandle){
		UE_LOG(LogTemp, Error, TEXT("objekat %s nema physic handle komponentu!"), *GetOwner()->GetName());
	}
}


void UGrabber::Grab(){
	//UE_LOG(LogTemp, Warning, TEXT("graber pressed!"));

	//pokusavamo da dohvatimo bilo kog actora kome je psotavljeno PhysicsBody
	FHitResult HitResult = GetFirstPhysicsBodyInReach();

	//treba nekako da odredimo koji to objekat treba da uhvatimo, on mora biti tipa UPrimitiveComponent* zato sto taj tip trazi funkcija GrabComponentAtLocation()
	//to ce biti ono na sta naidjemo u ray cast
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();

	//ako naidjemo an nesto onda prikacimo physics handle
	AActor* ActorHit = HitResult.GetActor();
	if( ActorHit ){
		if(!PhysicsHandle) return; //zastita od nullptr, ako nema physics handle hocemo da iskocimo odmah iz funkcije Grab, da ne izvrsavamo ovo ispod

		PhysicsHandle->GrabComponentAtLocation(
			ComponentToGrab,	//UPrimitiveComponent* Component -> sta hvatamo
			NAME_None,			//FName InBoneName -> za skeletal meshes gde se nalaze Bone-ovi, mi to ovde ne korsitimo pa samo stavimo NAME_None
			GetPlayersReach()	//FVector GrabLocation -> lokacija objekta kog hvatamo NOTE: misli se na centar objekta, tj onu belu tackicu u koju koristimo za pomeranje npr
		);
	}
}




void UGrabber::Release(){
	//UE_LOG(LogTemp, Warning, TEXT("graber released!"));
	
	if(!PhysicsHandle) return; //zastita od nullptr,

	//pustamo objekat koji smo uhvatili
	PhysicsHandle->ReleaseComponent();
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(!PhysicsHandle) return; //zastita od nullptr,
	
	//ako imamo uhvaceno nesto, grabbed component vraca pokazivac na actora koji je uhvacen
	if(PhysicsHandle->GrabbedComponent){
		//pomeramo objekat koji drzimo
		PhysicsHandle->SetTargetLocation( GetPlayersReach() );
	}
}


FHitResult UGrabber::GetFirstPhysicsBodyInReach() const{
	
	//crtanje linije, hocemo da kada pritisnemo eject vidimo liniju u pravcu u kom igrac gleda, koja ce biti dugacka onoliko koliko nam je dohvat za hvatanje stvari
		//DrawDebugLine(
		//GetWorld(),				// svet
		//PlayerViewpointLocation,  //pcoetak linije
		//LineTraceEnd,             //kraj linije
		//FColor(0, 255, 0),	    //rgb boja linije, FColor(...) koristimo da nebi morali da pravimo posebnu promenljivu za ovo
		//false,					//persistance, da li ostaje iscrtana i u sledecim frejmovima
		//0.f,					    //nesto float lifetime nebitno (?)
		//0,						//depth priority ovde nebitno (?)	
		//5.f					 	//debljina u pikselima
		//);


	//ray cast do odredjene daljine (Reach)
	FHitResult Hit;  //out parametar za ray cast, koristimo funkciju LineTraceSingleByObjectType

	//FCollisionQueryParams je struktura koja sluzi za definisanje parametara koje prosledjujemo funkcijama za kolizije
	//ima raznih konstruktora ali mi ovde koristimo ovaj sa: 
	//			InTraceTag -> (?)  mi ovde ne koristimo tagove (?) za collision queryje nego koristimo EcollisionChannel, pa ovo stavljamo na praznu nisku (?)
	//			bInTraceComplex -> da li koristimo proste kolizije(to je player collision u UE) ili kompleksne(to je visibility collision u UE) 
	//			InIgnoreActor -> sta treba da ignorisemo, u nasem slucaju treba da ignorisemo igraca zatos to zrak krece iz unutrasnjosti igraca i prva kolizija koja ce se desiti je sa samim igracem, igraca dobijamo sa GetOwner zato sto u igracu imamo ovu Grabber komponentu
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner() );

	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,			   //prvi blokirajuci objekat na koji se naidje
		GetPlayersWorldPos(),  //start point
		GetPlayersReach(),	   //end point	
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody), //lista tipova objekata sa kojima moze da se trazi presek, ecollisionchannel je ono sto smo postavili za objekat da pise physics body. To je enum i njegovim poljima pristupamo ovako sa ::
		TraceParams				//dodatni parametri za trace
	);

	//ispisemo naziv actora sa kojim se sece zrak
	//if(Hit.GetActor()){   //provera za nullpointer da nebi chrashovalo
	//	UE_LOG(LogTemp, Warning, TEXT("collision detected with Actor: %s"), *Hit.GetActor()->GetName());
	//}

	return Hit;
}


FVector UGrabber::GetPlayersReach() const{
	FVector PlayerViewpointLocation;	//pozicija samog igraca u odnosu na koordinatni pocetak(tj ovo je vektor od koord pocetka do igraca)
	FRotator PlayerViewpointRotation;  
	 //da bismo videli u sta nas igrac gleda, BITNO, ovaj poslednji get je void, vratice vrednosti preko izlaznih parametara koje cemo proslediti
	 //OUT je makro koji smo mi definisali cisto da bi oznacili da su ovo izlazni parametri, tj da ce dobiti vrednost tokom izvrsavanja funkcije, sam makro ne radi nista
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewpointLocation, OUT PlayerViewpointRotation); 
	
	//UE_LOG(LogTemp, Warning, TEXT("viewpoint location: %s"), *PlayerViewpointLocation.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("viewpoint rotation: %s"), *PlayerViewpointRotation.ToString());

	//da bismo dobili liniju koja predstavlja duz od igraca u smeru gledanja, radimo kao sabiranje vektora, ovaj prvi PlayerViewPortLocation je vekor iz koord pocetka, do igraca,
	//ovaj rotation.vector(), nas FRotator pretvara u JEDINICNI!! vektor u pravcu gledanja, mnozimo ga sa Reach koji smo definisali u header fajlu koji kaze koji nam je domet za hvatanje stvari
	//taj drugi sabirak je znaci vektor od igraca do najdalje tacke u pravcu gledanja na kojoj mozemo da uhvatimo nesto na sceni
	//kada se saveru to bude vektor od koord pocetka do te krajnje tacke i to prosledjujemo funkciji DrawDebugLine kao krajnju tacku
	FVector LineTraceEnd = PlayerViewpointLocation + PlayerViewpointRotation.Vector() * Reach;
	return LineTraceEnd;
}


FVector UGrabber::GetPlayersWorldPos() const{
	FVector PlayerViewpointLocation;	
	FRotator PlayerViewpointRotation;  
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewpointLocation, OUT PlayerViewpointRotation); 
	return PlayerViewpointLocation;
}