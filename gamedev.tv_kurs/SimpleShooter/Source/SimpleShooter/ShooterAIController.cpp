// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ShooterCharacter.h"

void AShooterAIController::BeginPlay(){
    Super::BeginPlay();
    
    if(AIBehavior != nullptr){
        //RunBehaviorTree je ugradjena funck iz AIController.h, moze da vrati bool vrednost u zavisnosti da li smo uispeli da pokrenemo nas Behavior Tree
        RunBehaviorTree(AIBehavior);

        //treba pawn koga igrac kontrolise koji ima ovaj controller, argumenti su context i indeks kontrolera
        APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn( GetWorld(), 0);

        //zelimo da napravimo novi Key na nasem blackboardu,prvo  ga dohvatimo funkcijom GetBlackboardComponent iz BlackboardComponent.h
        //onda pozovemo SetValueAsX, gde je X tip Key-a, damo mu ime i vrednost (NOTE: ovaj key se nece automatski napraviti, vec je potrebno da ga rucno napravimo
        //u blackboardu i on mora tamo imati isto ime i tip kao ovde, a onda ce ova funkcija tom Key-u dodeliti vrednost koju kazemo ovde)
        //GetPawn() vraca pawna kog kontrolise ovaj kontroler
        GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation() );
    }
}


void AShooterAIController::Tick(float DeltaSeconds){
    Super::Tick(DeltaSeconds);

    /*NOTE: koemntarisani kod ispod je prva verzija AI koju smo implementirali, nakon toga smo presli na Beahaviour Tree i Blackboard*/
    /*
        //treba pawn koga igrac kontrolise koji ima ovaj controller, argumenti su context i indeks kontrolera
        APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn( GetWorld(), 0);
        
        //LineOfSightTo proverava da li AI vidi actora koji je prosledjen kao argument i vraca true ako vidi
        //ta funkcija vec za nas uzima u obzir i polozaj kamere itd. Ima jos neke parametre ali oni imaju default vrednost i sada nam nisu potrebni
        if( LineOfSightTo(PlayerPawn) ){
            //hocemo da se AI fokusira na tog pawna, ovo SetFocus je funkcija iz AIController.h interfejsa
            //prima dva argumenta, jedan je AActor* na koga treba da se fokusra, a drugi je polje enuma EAIFocusPriority koji daje prioritet, on po defaultu
            //ima vrednost Gamplay sto je najvisi prioritet, ne moramo rucno da prosledjujemo
            //inace nije bitno da li cemo fokus staviti pre ili posle MoveTo zato sto MoveTo valjda automatski da neki fokus (on mozda nece biti istog prioriteta kao onaj koji mi rucno stavimo)
            SetFocus( PlayerPawn );
            //iammo i MoveTo funckije kao sto su MoveToActor i MoveToLocation, koje kazu gde AI treba da ide, naravno prvo moramo da podesimo Nav Mesh da bi AI znao gde sme a gde ne sme da ide
            //ako ne pzoovemo ovu funkciju u Tick funkciji, desice se da ce AI doci do nas i onda nakon toga prestati da pokusava da nas prati
            MoveToActor( 
                PlayerPawn,         //do kog actora ide
                AcceptanceRadius    //ovo je acceptance radius, po defaultu je -1, to je koliko blizu sme da pridje actoru
            );
        }else{
            //ako nije in line of sight, hocemo da prestane da se fokusira. Posto set focus kao drugi argument prima fokus, a posto ga nismo prosledili
            //dobio je default vrednost EAIFocusPriority::Gameplay, u ClearFocus treba da prosledimo onaj fokus koji je proledjen u SetFocus (?)
            ClearFocus(EAIFocusPriority::Gameplay);
            //funkcija iz AIController interfejsa, ako AI ne viid igraca prestane da se krece
            StopMovement();
        }
    */

    //NOTE 2: komentarisani deo ispod je druga verzija AI koju smo napisali, ali smo posle toga presli na BT service, i napisali dve nove klase
    //BTService_PlayerLocation i BTService_PlayerLocationSeen i onda smo te servise ugradili u nas BehaviorTree
    /* 
    //treba pawn koga igrac kontrolise koji ima ovaj controller, argumenti su context i indeks kontrolera
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn( GetWorld(), 0);

    if( LineOfSightTo(PlayerPawn) ){
        GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation() );
        GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownPlayerLocation"), PlayerPawn->GetActorLocation() );
    }
    else{
        //ClearValue brise vrednost koja se nalazi u Key-u koji prosledimo
        GetBlackboardComponent()->ClearValue( TEXT("PlayerLocation") );
    }
    */
}

bool AShooterAIController::IsDead()  const{
    //posto vec imamo IsDead funckiju u ShooterCharacter.h dovoljno je da ovde dohvatimo pawn,
    //castujemo u AShooterCharacter i odatle pozovemo isDead
    AShooterCharacter* ControllerCharacter = Cast<AShooterCharacter>(GetPawn());
    if (ControllerCharacter != nullptr){
        return ControllerCharacter->IsDead();
    }

    //ako ovaj AiController ne kontrolise pawn-a (tj ako je taj pawn nullptr) to prakticno znaci da je mrtav
    //inace pawn se automatski otkaci od controllera kada on umre
    return true;
}
