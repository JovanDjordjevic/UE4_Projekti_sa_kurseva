// Fill out your copyright notice in the Description page of Project Settings.


#include "PawnTank.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

APawnTank::APawnTank(){

    //spring arm je kao drzac za kameru. kameru dodamo kao dete od spring arm da bismo pomeranjem spring arma u editoru pomerali i kameru zajedno sa njim
    //generalno ako imamo parent-child objekte, pomeranjem parent objekta u UE editoru pomeracemo i dete

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
    SpringArm->SetupAttachment(RootComponent); //ovde smo se oslonili na UE da on sam odredi root component, a u PawnBase.cpp smo sami stavili

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void APawnTank::BeginPlay()
{
	Super::BeginPlay();
	PlayerControllerRef = Cast<APlayerController>( GetController() );
}

void APawnTank::HandleDestruction(){
    Super::HandleDestruction();
    bIsPlayerAlive = false;

    //sakrivamo igraca i prekidamo svaku tick funkcionalnost (jer nam vise ne treba)
    //pre nego sto smo napisali ovo, kada izgubimo desavalo se da ako je u tom trenutku bio pritisnut aster za kretanje, da tenk nastavi da se krece
    //u tom pravcu bez naseg inputa, a posto je Move() u tick funckiji, ovaj poziv prekida to kretanje
    SetActorHiddenInGame(true);
    SetActorTickEnabled(false);
}


bool APawnTank::GetIsPlayerAlive(){
    return bIsPlayerAlive;
}


// Called every frame
void APawnTank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    Rotate();
    Move();

    if(PlayerControllerRef){
        //GetHitResultUnderCursor kao da pusti zrak iz pozicije na ekranu na kojoj se nalazi kursor kroz scenu
        //i vrati prvi objekat na koji se naidje na tom zraku
        FHitResult TraceHitResult;
        PlayerControllerRef->GetHitResultUnderCursor(
            ECC_Visibility,     //koji collision channel se koristi
            false,              //da li se radi comlex collision
            TraceHitResult      //out parametar (?) ne pise nigde u dokumentaciji sta je, u ovo se valjda upise rezulat ove funkcije
        );

        //ImpactPoint je lokacija koju nas zrak pogadja, za nas je to lokacija ka kojoj zelimo da kupola tenka gleda
        FVector HitLocation = TraceHitResult.ImpactPoint;

        //u parent klasi u ovoj funkciji koju smo napisali pre ovoga smo stavili da se rotacija menja samo po X i Y koordinati a da Z ostane ista, 
        //tako da ovde prosledjujemo samo ovaj HitLocation bez dalje modifikacije
        //to je bitno zato sto bi u suprotnom u zavisnosti od toga kako nam ide zrak iz kursora pomerali kupolu gore i dole sto ne zelimo
        RotateTurret( HitLocation );
    }
}

// Called to bind functionality to input
void APawnTank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    //mapiranje inputa, slicno kao u BuildingEscape samo sto je sad BindAxis jer smo tako stavili u project settings
    PlayerInputComponent->BindAxis("MoveForward", this, &APawnTank::CalculateMoveInput);
    PlayerInputComponent->BindAxis("Turn", this, &APawnTank::CalculateRotateInput);
    //bind action za ove
    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APawnTank::Fire);
}

void APawnTank::CalculateMoveInput(float Value){
    //pravimo novi vektor za pravac kretanja, posto se pomeramo napred-nazad, a u UE je napred-nazad zapravo X osa, ostale dve ose stavljamo na 0
    //za X osu, prosledjeno value(tj napred ili nazad) mnozimo sa MoveSpeed i mnozimo sa DeltaTimeSeconds da bi bilo nezavisno od FPS!
    MoveDirection = FVector(Value * MoveSpeed * GetWorld()->DeltaTimeSeconds ,0 ,0);
}

void APawnTank::CalculateRotateInput(float Value){
    //value je smer u kome igrac zeli da se okrene
    float RotateAmount = Value * RotateSpeed * GetWorld()->DeltaTimeSeconds;
    //mi hocemo da promenimo Yaw ugao, znaci prvi i treci argument koji su pitch i roll stavljamo na 0
    FRotator Rotation = FRotator(0, RotateAmount, 0);
    //FQuat nam je potreban zato sto to trazi funkcija koja ce uraditi rotaciju
    RotationDirection = FQuat(Rotation);
}

void APawnTank::Move(){
    //dodaje delta polozaj na polozaj objekta koji hocemo da pomerimo
    //ova funkcija u dokumentaciji ima 4 argumenta ali izgleda postoji overload koji nam daje da koristimo samo ova dva
    AddActorLocalOffset(
        MoveDirection,   //FVector DeltaLocation - promena lokacije koju izvrsavamo
        true           //bool bSweep - da li zelimo da se prilikom pomeranja proveravaju kolizije (npr da je false prolazili bismo kroz zidove)
    );
}

void APawnTank::Rotate(){
    //dodaje delta vrednost na rotaciju objekta koji hocemo da rotiramo
    //ova funkcija u dokumentaciji ima 4 argumenta ali izgleda postoji overload koji nam daje da koristimo samo ova dva
    AddActorLocalRotation(
        RotationDirection,   //const FQuat & DeltaRotation - promena u rotaciji
        true                 //bool bSweep - da li proveravamo kolizije
    );
}