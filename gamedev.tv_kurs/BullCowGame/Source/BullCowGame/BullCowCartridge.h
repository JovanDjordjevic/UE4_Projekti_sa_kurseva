// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Console/Cartridge.h"
#include "BullCowCartridge.generated.h"

//definisanje nasih striktura ide ovde posle include-ova (?) po defaultu strukture su public
//po dogovoru strukture u UE pocinju slovom F
struct FBullCowCount{
	//clanove strukture mozemo a i ne mroamo da inicijalizujemo, ovde jesmo za svaki slucaj
	int32 Bulls = 0;
	int32 Cows = 0;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BULLCOWGAME_API UBullCowCartridge : public UCartridge
{
	GENERATED_BODY()

	public:
	virtual void BeginPlay() override;
	virtual void OnInput(const FString& Input) override;
	void InitGame();
	void EndGame();
	void ProcessGuess(const FString& Guess);
	//const funkcije su funkcije koje ne menjaju member varijable, one u sebi mogu da pozivaju samo druge const funkcije
	//to const ide na kraj i ovde u deklaraciji i u definiciji
	bool IsIsogram(const FString &Word) const;
	TArray<FString> GetValidWords(const TArray<FString>& WordList) const;
	//bull i cow count u sledecoj funkc su "out parameters". i dalje je const funkcija jer necemo menjati nista od dole deklarisanih member promenljivih
	//void GetBullCows(const FString& Guess, int32& BullCount, int32& CowCount) const;  tokom lekcije je promenjeno u ovo ispod:
	FBullCowCount GetBullCows(const FString& Guess) const;

	// Your declarations go below!
	private:
	//FString je stringovni tip u UE umesto String, slicno umesto int imacemo int32 
	FString HiddenWord;
	int32 Lives;
	bool bGameOver;
	TArray<FString> Isograms;
};


