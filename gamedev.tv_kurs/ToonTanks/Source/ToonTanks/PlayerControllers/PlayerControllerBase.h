// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class TOONTANKS_API APlayerControllerBase : public APlayerController
{
	GENERATED_BODY()
	
	public:

		//hocemo da podesimo da negde mozemo a negde ne mozemo da kontrolisemo nas tenk, npr da ne moze da se pomera dok traje countdown na pocetku
		void SetPlayerEnabledState(bool SetPlayerEnabled);
};
