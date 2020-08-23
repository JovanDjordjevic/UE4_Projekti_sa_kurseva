// Fill out your copyright notice in the Description page of Project Settings.


#include "TankGamemodeBase.h"
#include "ToonTanks/Pawns/PawnTank.h"
#include "ToonTanks/Pawns/PawnTurret.h"
#include "Kismet/GameplayStatics.h"
#include "ToonTanks/PlayerControllers/PlayerControllerBase.h"

void ATankGamemodeBase::BeginPlay(){
    Super::BeginPlay();

    HandleGameStart();
}

void ATankGamemodeBase::ActorDied(AActor* DeadActor){
    //UE_LOG(LogTemp, Warning, TEXT("pawn died"));

    if(DeadActor == PlayerTank){
        PlayerTank->HandleDestruction();
        HandleGameOver(false);

        //nakon sto je tenk unisten, zelimo da vise ne mozemo da se pomeramo (u suprotnom bi se videlo pomeranje kamere uz ans input) pa koristimo nasu funkciju
        if( PlayerControllerRef ){
            PlayerControllerRef->SetPlayerEnabledState(false);
        }
    }
    //ako je u pitanju turret,pitamo se tako sto pokusamo dead actora da castujemo u turret, ako cast uspe, tj pokazivac nije null znamo da je turret
    APawnTurret* DestroyedTurret = Cast<APawnTurret> (DeadActor);
    if ( DestroyedTurret ){
        DestroyedTurret->HandleDestruction();
        TargetTurrets--;
        if(TargetTurrets == 0){      //moze i (--TargetTurrets == 0)
            HandleGameOver(true);
        }
    }
}


void ATankGamemodeBase::HandleGameStart(){
    //nadjemo koliko tih turreta ima u nivou, ovo je da nebi morali rucno da hardcoodujemo broj turreta vec da mozemo da menjamo njihov broj tokom razvoja
    TargetTurrets = GetTargetTurretCount();
    //GetPlayerPawn uzima context object i indeks igraca i vraca igraca
    PlayerTank = Cast<APawnTank>( UGameplayStatics::GetPlayerPawn(this, 0) );
    //
    PlayerControllerRef = Cast<APlayerControllerBase>( UGameplayStatics::GetPlayerController(this, 0) );

    //poziv blueprint verzije GameStart()
    GameStart();

    //zelimo da ne mozemo da se pomeramo odmah na pocetku dok traje tajmer i nakon sto istekne vreme da ponovo mozemo da se pomeramo
    if( PlayerControllerRef ){
        PlayerControllerRef->SetPlayerEnabledState(false);

        //(?) ne razumem bas sta radi ovde, nije objasnio
        FTimerHandle PlayerEnableHandle;
        FTimerDelegate PlayerEnableDelegate = FTimerDelegate::CreateUObject(PlayerControllerRef, &APlayerControllerBase::SetPlayerEnabledState, true);
        GetWorld()->GetTimerManager().SetTimer(
            PlayerEnableHandle,         //tajmer
            PlayerEnableDelegate,       //delegate
            StartDelay,                 //float delay
            false                       //loop boolean
        );

    }
}

void ATankGamemodeBase::HandleGameOver(bool PlayerWon){
    //ako player unisti sve turrete show win result
    //else ako turret ubije igraca show lose result
    //poziv blueprint verzije GameOver()
    GameOver(PlayerWon);
}

int32 ATankGamemodeBase::GetTargetTurretCount(){
    //GetAllActorsOfClass vraca sve actore u SVETU koji su date klase
    TArray<AActor*> TurretActors;
    //::StaticClass() samo znaci da ne trazimo da zapravo postoji instanca ove klase, ova funkcija vraca UClass actorovog tipa, ovde ce da vrati APawnTurret
    UGameplayStatics::GetAllActorsOfClass(
        GetWorld(),                     //prvi arg je world context object,
        APawnTurret::StaticClass(),     //drugi arg je klasa koju zelimo da nadjemo, 
        TurretActors                    //TArray out parametar u koji ce biti smesteni trazeni objekti
    );

    return TurretActors.Num();
}



