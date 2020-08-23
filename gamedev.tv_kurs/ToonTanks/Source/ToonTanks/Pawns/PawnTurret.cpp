// Fill out your copyright notice in the Description page of Project Settings.


#include "PawnTurret.h"
#include "Kismet/GameplayStatics.h"  //ovo nam dozvoljava da koristimo tajmere i jos neke stvari kao npr GetPlayerPawn funckiju
#include "PawnTank.h"

// Called when the game starts or when spawned
void APawnTurret::BeginPlay()
{
    Super::BeginPlay();
	
    GetWorld()->GetTimerManager().SetTimer(
        FireRateTimerHandle,                //tajmer
        this,                               //koji objekat je u pitanju
        &APawnTurret::CheckFireCondition,   //koaj funkcija se izvrsava zbog tajmera
        FireRate,                           //vremenski interval
        true,                               //da li ce da se loop-uje, mi hocemo da se ponavlja svake dve sekunde(FireRate), da je false funkcija bi se desila samo jednom, kao timed event
        false                               //da li postoji inital delay, nama ne
        );

   //GetPlayerPawn vraca APawn*, pokazivac na igraca kog zelimo (drugi argument, u single player igrici indeks igraca je 0)
   //Prvi argument je neki UObject* WorldContextObjec. World ke top level objekat koji predtavlja mapu ili neki sandbox, u njemu su postavljeni svi actori i komponente
   //World je Object dakle ne postoji kao fizicki objekat  (?) vec je potrebno da damo context actora, tj neki objkat koji
   //fiuzicki postoji u nasem svetu, a na osnovu njega ce funkcija znati u komsvetu se nalazi i naci odgovarajuceg igraca(?), ovde je to ova konkreta instanca klase PawnTurret
   //posto je pokazivac koji je vracen APawn*, a APawnTank nasledjuje APawn, mozemo da uradimo ovo kastovanje 
   PlayerPawn = Cast<APawnTank> ( UGameplayStatics::GetPlayerPawn( this , 0) );
}

void APawnTurret::HandleDestruction(){
        //prvo ce se uci u parent klasu, izvrsiti sta tamo ova metoda radi, a onda ce se nastaviti sa izvrsavanjem ono sto napisemo ovde, sto overriduje to u parent klasi
        Super::HandleDestruction();
        Destroy();
}


// Called every frame
void APawnTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

        if ( !PlayerPawn || ReturnDistanceToPlayer() > FireRange){
                return ;
        }

        RotateTurret( PlayerPawn->GetActorLocation() );
}

void APawnTurret::CheckFireCondition(){
        //da li je igrac == null ili mrtav, ako jeste samo se vratimo nazad jer nema potrebe da turret puca
        if( !PlayerPawn || !PlayerPawn->GetIsPlayerAlive() ){
                return;
        }
        //da li je igrac u dometu
        if( ReturnDistanceToPlayer() <= FireRange){
                Fire();
        }
}

float APawnTurret::ReturnDistanceToPlayer(){
       if( !PlayerPawn ){
                return 0.f;
        } 
        //prvia rgument je FVector polozaja igraca a drugi je FVecrtor polozaja ovog turreta (mozda je moglo i this->GetActorLocation)
        return FVector::Dist(PlayerPawn->GetActorLocation(), GetActorLocation());  
}