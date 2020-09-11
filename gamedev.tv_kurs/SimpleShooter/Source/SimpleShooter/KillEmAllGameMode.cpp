// Fill out your copyright notice in the Description page of Project Settings.


#include "KillEmAllGameMode.h"
#include "EngineUtils.h"
#include "GameFramework/Controller.h"
#include "ShooterAIController.h"

void AKillEmAllGameMode::PawnKilled(APawn* PawnKilled){
    Super::PawnKilled(PawnKilled);
    //UE_LOG(LogTemp, Warning, TEXT("pawn %s is dead!"), *PawnKilled->GetName());

    //dohvatamo controller pawna koji je ubijen
    APlayerController* PlayerController = Cast<APlayerController>(PawnKilled->GetController());
    if(PlayerController != nullptr) {
        //pozivamo nasu funkciju koja ce prekinuti igru za sve Controllere
        EndGame(false);
    }

    //hocemo da u petlji prodjemo kroz sve ShooterAI u svetu, i za njih da proverimo da li su mrtvi i tako racunamo da li je iorac pobedio
    //NOTE: ovo je bolje nego da imamo neki staticki brojac koliko AI ima u nivou zato sto taj broj mora uvek biti sinhronizovan sa onim sto je u nivou, 
    //a ako nije sinhronizovan mo,ze doci do problema
    //TActorRange vraca range objekat svih objekata tipa koji navedemo u <> u nivou koji stavimo kao argument
    //nama ce ovde vratiti range svih AI controllera u nasem nivou
    for(AShooterAIController* Controller : TActorRange<AShooterAIController>(GetWorld())){
        //ako naidjemo na neki AI koji je ziv znaci da igrac sigurno nije jos pobedio
        if( !Controller->IsDead()){
            return;
        }
    }
    //ako smo prosli kroz velu petlji znaci da su svi AI mrtvi znaci igrac je pobedio
    EndGame(true);
}

void AKillEmAllGameMode::EndGame(bool bIsPlayerWinner){
    for(AController* Controller : TActorRange<AController>(GetWorld())){
        //zelimo da za svaki controller u igri pozovemo ugradjenu funkciju GameHasEnded iz PlayerController.h
        //IsPlayerController() je ugradjena funkcija iz Controller.h koja za dati Controller proverava da li je to igrac
        //bIsWinner cemo izracunavati i proslediti kao drugi argument za GameHasEnded, logika je sledeca:
        //ako je trenutni controller igrac i ako je igrac pobedio, znaci da ide true, tj GameHasEnded oznacava da je ovaj controller pobednik
        //ako trenutni controller nije igrac i nije pobedio, to znaci da smo na nekom AI controlleru koji treba da bude pobednik
        //ostali slucajevi (jeste igrac i nije pobedio  i  nije igrac i jeste pobedio) daju false
        bool bIsWinner = Controller->IsPlayerController() == bIsPlayerWinner;
        Controller->GameHasEnded(
            Controller->GetPawn(),      //AActor* EndGameFocus - na kog actora treba da pokazuje kamera (moze i na nas, i na AI itd), moze da se prosledi i nullptr, onda je ponasanje: (?)
            bIsWinner                   //bool vrednost, true ako je ovaj kontroler pobedio
        );
        
    }
}

