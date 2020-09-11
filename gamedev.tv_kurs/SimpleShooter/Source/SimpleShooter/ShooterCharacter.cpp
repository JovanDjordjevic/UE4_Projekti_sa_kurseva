// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "Gun.h"
#include "Components/CapsuleComponent.h"
#include "SimpleShooterGameModeBase.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;

	//hocemo da spawnujemo pusku kad igra pocne, SpawnActor prima (?)
	//ovo GunClass cemo da setujemo u blueprintu, stavicemo da bude BP_Rifle
	Gun = GetWorld()->SpawnActor<AGun>( GunClass );
	//mi zelimo da nasu pusku ubacimo karakteru u ruke, ali default mesh vec ima svoju pusku. Moramo nekako da je uklonimo.
	//Ako pogledamo skeletal mesh karaktera, videcemo da je puska povezana sa rukom preko koske, tako da posto se sada nalazimo u ShooterCharacter.cpp
	//mozemo da dohvatimo mesh i u njemu sakrijemo tu kosku, samim tim i pusku
	//prvi arg je ime koske koju krijemo, a drugi je vrednosti iz enuma EPhysBodyOp, ne kosristi se ali je potpis funkcije takav da moramo da prosledimo (?)
	GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None);
	//sada kada je sakriveno, treba nekako da ubacimo nasu pusku. to radimo pomocu socketa
	//u skeletal meshu nadjemo kosku weapon_r koju smo sad sakrili, desni klik i add socket, a ovde u cpp cemo ga zapravo iskoristiti
	//posto ovde treba da zakacimo pusku u runtime, ne mozemo da radimo sa SetupAttachment kao do sada zato sto se ta funkcuja poziva u kontruktoru!!!!!!
	//argumenti su: parent na koga kacimo, FAttachmentTransformRules - pravila za transformaciju(guglaj koji sve postoje) ,ime soketa na koji kacimo (NOTE: ako kacimo neki static mesh, ovaj argument ce biti ignorisan jer static meshevi nemaju skelet)
	Gun->AttachToComponent( GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket") );
	//treba samo jos da stavimo da je owner ove puske ova klasa, Actori nemaju nikakvu hijerarhiju izmedju sebe (u smislu transformova), ali imaju ownership
	//NOTE: ovaj korak je bitan za multiplayer i za primanje dmg 
	Gun->SetOwner(this);
}

bool AShooterCharacter::IsDead() const{
	return Health <= 0;
}

float AShooterCharacter::GetHealthPercent() const{
	//vracamo health u intervalu [0,1] jer je to tako potrebno za progress bar koji predstavlja health u nasem HUD-u
	return Health / MaxHealth;
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AShooterCharacter::MoveForward);
	//iammo funkciju iz Pawn klase AddControllerPitchInput koja pomera nas pravac gledanja po Y osi, posto bismo
	//tu funkc pozvali samo sa AddControllerPitchInput( AxisValue ); bez ikakve modifikacie, za ralziku od onoga sto smo radili u MoveForwad
	//ovde bind funkciji mozemo direktno da prosledimo ovu vec psotojecu funkc, samo je bitno da znamo kojoj klasi ona pripada. Ako nismo sihurni, klik na funkc, klik F12 i scroll gore do imena klase
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AShooterCharacter::MoveRight);
	//slicno i za gledanje levo/desno, imamo funkc AddControllerYawInput
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &APawn::AddControllerYawInput);
	//jump je akcija a ne axis, imamo u kalsi Character vec ugradjenu funkc za skok pa mozemo nju da pozovemo
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);

	//za dzojstik
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AShooterCharacter::LookUpRate);
	PlayerInputComponent->BindAxis(TEXT("LookRightRate"), this, &AShooterCharacter::LookUpRight);

	//zelimo da se pucanje odvija u Gun klasi, ali posto bindujemo input u ShooterCharacter, potrebna nam je intermediary funkcija koju pozivamo ovde 
	//u ShooterCharacter imamo pristup toj pusci zato sto smo je u kodu iznad spawnovali i sacuvali referencu na nju, a kad imamo tu referencu mozemo pozvati funkciju iz te klase
	PlayerInputComponent->BindAction(TEXT("Shoot"), IE_Pressed, this, &AShooterCharacter::Shoot);
}

//overridujemo ovu funkciju iz Actor klase
float AShooterCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser){
	//OBAVEZNO pozvati Super konstruktor kada overridujemo klasu ako zelimo da zadrzimo funkcionalnost iz ove funkcije u parent klasi (?)
	//samo prosledjujemo vec dobijene argumente nadklasi
	//takodje moramo da pazimo na povratne vrednosti. Ako funckija koju overridujemo ima povratnu vrednost, onda poziv Super:: takodje ce imati povratnu i moramo negde da je sacuvamo
	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	//iako ce nama u ovom pozivu DamageToApply biti jednak DamageAmount, dobra praksa je da koristimo povratnu vrednost iz Super:: ako postoji
	DamageToApply = FMath::Min(Health, DamageToApply);  //da ne odemo u minus, mogli smo nesto i sa FMath::Clamp, kao u ToonTanks
	Health -= DamageToApply;
	UE_LOG(LogTemp, Warning, TEXT("preostalo hp: %f"), Health);

	if(IsDead()){

		//GetAuthGameMode vraca referencu na trenutni gamemode, a posto smonaveli <>, to sto ce vratiti bice kastovano u ASimpleShooterGameModeBase, to moze jer je to bazna gamemode klasa (najvisa u hijerarhiji)
		ASimpleShooterGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ASimpleShooterGameModeBase>();
		if(GameMode != nullptr){
			//pozivamo PawnKilled funkciju koju smo implementirali da bismo obavestili gamemode da je ovaj pawn mrtav (bilo da je to AI ili nas igrac, oba su ShooterCharacter)
			GameMode->PawnKilled(this);
		}

		//posto je character mrtav ima smisla otkaciti kontroler da vise ne moze da se kontrolise
		//psoto i nas lik i AI imaju ovu funckiju ovo ce uticati na smrt oba
		//NOTE: ako bismo slucajno Detach pozvali pre ovog if-a gore,PawnKilled se nebi lepo izvrsio jer bi controller uvek bio null
		DetachFromControllerPendingDestroy();
		//takodje treba iskljuciti kolizije sa mrtvim igracem
		GetCapsuleComponent()->SetCollisionEnabled( ECollisionEnabled::NoCollision );
	}

	return DamageToApply;
}


//u AddMovementInput pozivima, ne moramo da mnozimo sa DeltaSeconds kao sto smo morali u AddControllerPitchInput kad smo radili sa dzojstikom
//zato sto ta funkcija vec to radi za nas (?), ona pretpostavlja da je argument neki Rate, a ne zapravo distanca koja je predjena
void AShooterCharacter::MoveForward(float AxisValue){
	//AddMovementInput je funkcija klase APawn, znaci imaju je svi Actori i Characteri, koja pomera naseg lika duz vektora koji prosledimo
	//AxisValue je ono sto vidimo u Project Settings/Input pored naseg bindinga, npr stavili smo W-1.0, S- -1.0 za napred i nazad
	//GetActorForwardVector() samo vraca vektor pravca u kom actor gleda
	AddMovementInput( GetActorForwardVector() * AxisValue);
}
void AShooterCharacter::MoveRight(float AxisValue){
	//GetActorRightVector je vec igradjena funkc, slicno kao za move forward
	AddMovementInput( GetActorRightVector() * AxisValue );
}

//sledece dve funkcije su za dzojstik
void AShooterCharacter::LookUpRate(float AxisValue){
	//Axis value ce biti vrednost izmedju 0 i 1, kad pomnozimo sa nasim RotationRate dobijamo brzinu, 
	//ali nama je potrebna distanca koja ce da se predje pa mnozimo sa vremenom
	//DeltaSeconds je kolicina vremena koja je bila potreban za zadnji frejm, tj frametime
	AddControllerPitchInput( AxisValue * RotationRate *  GetWorld()->GetDeltaSeconds() );
}
void AShooterCharacter::LookUpRight(float AxisValue){
	AddControllerYawInput( AxisValue * RotationRate *  GetWorld()->GetDeltaSeconds() );
}

void AShooterCharacter::Shoot(){
	Gun->PullTrigger();
}