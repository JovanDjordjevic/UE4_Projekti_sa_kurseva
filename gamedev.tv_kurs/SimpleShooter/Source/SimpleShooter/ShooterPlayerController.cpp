// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterPlayerController.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"

void AShooterPlayerController::BeginPlay(){
    Super::BeginPlay();

    //CreateWidget iz Blueprint/UserWidgets pravi odgovarajuci widget i vraca pokazivac na njega. argumenti:
    //  1) owning object  - samo neke stvari mogu da budu owner za widget, jedna od tih svari je PlayerController, to je ono sto mi ovde prosledimo sa this
    //  2) TSubclassOf<UUserWidget> UserWidgetClass  -ono sto smo dodali u .h fajlu
    HUD = CreateWidget(this, HUDClass);
    if(HUD != nullptr){
        //sad kada imamo pokazivac na widget, potrebno je da taj widget prikazemo na ekranu
        HUD->AddToViewport();
    }
}

void AShooterPlayerController::GameHasEnded(class AActor* EndGameFocus, bool bIsWinner){
    Super::GameHasEnded(EndGameFocus, bIsWinner);

    //ko god da je pobedio treba da sklonimo HUD
    HUD->RemoveFromViewport();
    
    //ako je nas igrac pobednik pravimo win screen, u suprotnom lose screen
    if (bIsWinner){
        UUserWidget* WinScreen = CreateWidget(this, WinScreenClass);
        if(WinScreen != nullptr){
            WinScreen->AddToViewport();
        }
    }
    else{
        UUserWidget* LoseScreen = CreateWidget(this, LoseScreenClass);
        if(LoseScreen != nullptr){
            LoseScreen->AddToViewport();
        }
    }
    
    //zelimo da se posle nekoliko sekundi (RstartDelay) restartuje igra tako sto se pozove ugradjena RestartLevel funkcija iz PlayerController.h
    GetWorldTimerManager().SetTimer(RestartTimer, this, &APlayerController::RestartLevel, RestartDelay);
}


