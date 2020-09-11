// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Shoot.h"
#include "AIController.h"
#include "ShooterCharacter.h"

UBTTask_Shoot::UBTTask_Shoot(){
    NodeName = TEXT("Shoot");
}

EBTNodeResult::Type UBTTask_Shoot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory){
    //uvek kada overridujemo treba da pozovemo Super::
    Super::ExecuteTask(OwnerComp, NodeMemory);

    //zelimo da pozovemo Shoot metodu iz ShooterCharacter klase, zato mroamo prvo da daohvatimo pawn
    //neka od baznih klasa od UBehaviorTreeComponent ima funkciju GetAIOwner() koja vraca pokazivac na AAIController koji mozemo 
    //da kastujemo u ASHooterCharacter
    //obavezna provera da li je pokazivac null
    if( OwnerComp.GetAIOwner() == nullptr ){
        return EBTNodeResult::Failed;
    }

    //OwnerComp je vlasnik ovog Behavior Tree-ja (?), ne razumem ovu liniju bas najbolje (?)
    AShooterCharacter* Character = Cast<AShooterCharacter> ( OwnerComp.GetAIOwner()->GetPawn() );
    //GetPawn ce vratiti Pawna kog dati AIcontroller kontrolise ili null ako ne kontrolise nijedan, pa moramo da proverimo to
    if(Character == nullptr) return EBTNodeResult::Failed;

    Character->Shoot();

    return EBTNodeResult::Succeeded;
}