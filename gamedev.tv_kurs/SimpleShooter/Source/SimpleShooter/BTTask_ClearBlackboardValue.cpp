// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ClearBlackboardValue.h"
#include "BehaviorTree/BlackboardComponent.h"

//pravimos ami konstruktor da bismo promenili ime koje se automatski generise (?)
UBTTask_ClearBlackboardValue::UBTTask_ClearBlackboardValue(){
    //NodeName je vec polje ove klase i zelimo samo d aga promenimo u enkis tring koji je nama citljiv
    NodeName = TEXT("Clear Blackboard Value");
}

EBTNodeResult::Type UBTTask_ClearBlackboardValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory){
    //uvek kada overridujemo treba da pozovemo Super::
    Super::ExecuteTask(OwnerComp, NodeMemory);

    //GetSelectedBlackboardKey je funkc iz parent klase BTTak_BlackboardBase, ona vraca FName ime Keya koji je trenutno odabran
    //primetimo da je OwnerComp prosledjen kao referenca a ne pokazivac, tako da ne koristimo -> nego .
    //pod "ono sto je selektovano" misli se na onaj Key koji smo stavili u Details od ovog (ClearBlackBoardValue) cvora u nasem Tree-ju
    OwnerComp.GetBlackboardComponent()->ClearValue( GetSelectedBlackboardKey() );

    //return tip je enum koji moze imati vrednosti:
    //Succeeded, Failed, Aborted, InProgress
    //ako vratimo InProgress to ce programu reci da treba da poziva Tick dok mu se ne kaze drugacije, 
    //dok ostale tri opcije ne traze da se Tick ponovo pozove, tj njih koristimo kada nesto treba da se desi jednom
    return EBTNodeResult::Succeeded;
}