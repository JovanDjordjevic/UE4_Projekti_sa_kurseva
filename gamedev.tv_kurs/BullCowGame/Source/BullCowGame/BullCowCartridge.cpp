// Fill out your copyright notice in the Description page of Project Settings.
#include "BullCowCartridge.h"
#include "HiddenWordList.h"
#include "Math/UnrealMathUtility.h"

void UBullCowCartridge::BeginPlay() // When the game starts
{
    Super::BeginPlay();

    //nas kod ide obavezno ispod BeginPlay!
    //makro TEXT() omogucava da se string pravilno prikaze na svim sistemim, po defaultu kodira se u ANSI, sto ne podrzavajus vi ssitemi
    //u sustini svaki put kad imamo neki string dobro je da ga ubacimo u ovaj makro

    Isograms = GetValidWords(HiddenWordList);

    InitGame();

    //printf samo radi formatiranje, ne radi ispis, da bi se to sto je fortmatirano ispisalo mora da se stavi u PrintLine

    //PrintLine( FString::Printf(TEXT("hidden word is: %s it is %i characters long"), *HiddenWord, HiddenWord.Len() ) );   //debug line
    //PrintLine( TEXT("hidden word is: %s \nit is %i characters long"), *HiddenWord, HiddenWord.Len() );                   //debug line

    //MORA * da bi se pravilno odstampao FString
    //PrintLine je nasa definisana funkcija (vec dobijena gotova, nisam je ja pisao) koja vec u svojoj definiciji ima ugradjen Prinf
    //tako da u ovom kodu printf moze da se izostavi -->   PrintLine(TEXT("hidden word is: %s"), *HiddenWord);

    PrintLine(TEXT("number of possible words is %i"), HiddenWordList.Num() );

    
}

//& je referenca, znaci ovde se funkciji ne prosledjuje kopija od PlayerInput, nego bas adresa originala, const mora zato sto ne zelimo da se to menja
//ako imamo sledeci kod: int a = 1, int b = 2, int& refA = a, refA = b ono sto ce se desiti je da ce i refA i a dobiti vrednost 2 (tj b)
void UBullCowCartridge::OnInput(const FString& PlayerInput) // When the player hits enter
{   

    if(bGameOver){
        ClearScreen();
        InitGame();
    }
    else{
        ProcessGuess(PlayerInput);
    }  
    
}

void UBullCowCartridge::InitGame(){
    PrintLine(TEXT("Hello there!\n welcome wo the Bull Cow game"));

    HiddenWord = Isograms[FMath::RandRange(0, Isograms.Num()-1)];
    Lives = HiddenWord.Len();
    bGameOver = false;

    PrintLine(TEXT("hidden word is %s"), *HiddenWord);  //debug

    PrintLine(TEXT("guess the %i letter word\nyou have %i lives"), HiddenWord.Len(), Lives);
    PrintLine(TEXT("input your guess and press enter"));

}


void UBullCowCartridge::EndGame(){
    bGameOver = true;
    PrintLine(TEXT("press enter to paly again"));
}


void UBullCowCartridge::ProcessGuess(const FString& Guess){
    if(Guess == HiddenWord){
             PrintLine(TEXT("YOU WIN!"));
             EndGame();
             return;
    }

    if ( HiddenWord.Len() != Guess.Len()){
        PrintLine(TEXT("you have %i lives remaining"), Lives);
        return;
    }


    if( !IsIsogram(Guess) ){
        PrintLine(TEXT("no repeating letters! guess again"));
        return;
    }

    Lives--;
    PrintLine(TEXT("youve lost a life"));

    if(Lives <= 0){
        ClearScreen();
        PrintLine(TEXT("no lives left!\nYOU LOSE!"));
        PrintLine(TEXT("the word was: %s"), *HiddenWord);
        EndGame();
        return;
    } 

    //ako hocemo inicijalizaciju strukture mozemo da kazemo FBullCowCount Score = {2,3}; u {} redom vrednosti za svako polje, mzoemo i da ih menjamo kao promenljive, npr Score.Bulls = 5;
    FBullCowCount Score = GetBullCows(Guess);
    
    PrintLine(TEXT("you ahve %i bulls, and % i cows"), Score.Bulls, Score.Cows);

    PrintLine(TEXT("you have %i lives remaining"), Lives);\
}

bool UBullCowCartridge::IsIsogram(const FString& Word) const{
    
    for (int32 i=0; i<Word.Len(); i++){
        for (int32 j=i+1; j<Word.Len(); j++){
           if(Word[i] == Word[j]) return false; 
        }
    }
    return true;
}

TArray<FString> UBullCowCartridge::GetValidWords(const TArray<FString>& WordList) const{

    TArray<FString> ValidWords;

    for (int32 i = 0;i < WordList.Num(); i++){
        if(WordList[i].Len() >= 4 && WordList[i].Len() <= 8 && IsIsogram(WordList[i])){
            ValidWords.Emplace(WordList[i]);         //za dodavanje elementa na kraj niza imamo .add i .emplace, ali pozeljno je da se za FString koristi emplace (tako kaze dokumentacija), .add moze za sve ostalo
        }
    }
    return ValidWords;
}

FBullCowCount UBullCowCartridge::GetBullCows(const FString& Guess) const{

    FBullCowCount Count = {0, 0};

    for (int32 i = 0; i< Guess.Len(); i++){
        if(Guess[i] == HiddenWord[i]) {
            Count.Bulls++;
            continue;
        }
        for (int32 j = 0; j< HiddenWord.Len(); j++){
            if( j != i && Guess[i] == HiddenWord[j]){
                Count.Cows++;
                break;
            }
        }
    }

    return Count;
}