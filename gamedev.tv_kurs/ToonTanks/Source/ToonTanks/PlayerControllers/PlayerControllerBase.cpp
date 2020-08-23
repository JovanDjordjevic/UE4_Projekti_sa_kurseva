// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControllerBase.h"


void APlayerControllerBase::SetPlayerEnabledState(bool SetPlayerEnabled){
   
    if(SetPlayerEnabled){
        //enable input omogucava da se korisnikov unos registruje
        //argument je APlayerController ciji input zelimo da registrujemo
        GetPawn()->EnableInput( this );
    }
    else{
        //slicno kao gore
        GetPawn()->DisableInput( this );
    }

    //bShowMouseCursor je polje klase (nismo ga mi definisali) za biranje da li se mis vidi na ekranu ili ne
    bShowMouseCursor = SetPlayerEnabled;
}
