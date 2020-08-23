// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TankGamemodeBase.generated.h"

class APawnTurret;
class APawnTank;
class APlayerControllerBase;

UCLASS()
class TOONTANKS_API ATankGamemodeBase : public AGameModeBase
{
	GENERATED_BODY()

	private:
		//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
		APawnTank* PlayerTank;
		//broj turreta koje igrac treba da unisti da bi pobedio
		int32 TargetTurrets = 0;

		APlayerControllerBase* PlayerControllerRef;

		int32 GetTargetTurretCount();
		void HandleGameStart();
		void HandleGameOver(bool PlayerWon);

	public:	

		void ActorDied(AActor* DeadActor);

	protected:

		virtual void BeginPlay() override;

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Loop")
		int32 StartDelay = 3;

		//BlueprintImplementableEvent dozvoljava nam da radimo implementaciju funkcija u derived blueprint klasi, jer su tamo neke stvari
		//npr tajmeri i widgeti  lakse za implementaciju (?)
		//ove funkcije ne tereba da imaju implementaciju u kodu vec samo u blueprintu(?)  (misli se na ove sto imaju UFUNCTION specifier(?))
		UFUNCTION(BlueprintImplementableEvent)
		void GameStart();
		UFUNCTION(BlueprintImplementableEvent)		
		void GameOver(bool PlayerWon);
};
