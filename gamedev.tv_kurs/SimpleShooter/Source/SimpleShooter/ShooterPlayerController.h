// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SIMPLESHOOTER_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	//zelimo da nasa klasa overriduje ovu funkciju iz PlayerController.h
	virtual void GameHasEnded(class AActor* EndGameFocus = nullptr, bool bIsWinner = false) override;

protected:
	virtual void BeginPlay() override;

private:
	//PRIMECEN BUG: su prvi put dodate ove dve linije ispod, kompilacija je uvek vracala linking error
	//nesto je pisalo tipa external symbol error itd. To znaci da nam fali neki dependency, u ovom slucaju
	//treba otici u SimpleShooter.Build.cs i tu u liniji 11 na kraju liste za public dependencie dodati "UMG"
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> LoseScreenClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> WinScreenClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> HUDClass;
	 
	UPROPERTY(EditAnywhere)
	float RestartDelay = 5.f;

	FTimerHandle RestartTimer;

	//posto zelimo da HUD nestane kada se  igra zavrsi potrebno je da on bude klasna promenljiva
	//da bismo mogli da joj pristupimo iz neke druge metode
	UUserWidget *HUD;
};
