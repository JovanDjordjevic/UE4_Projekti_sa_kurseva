// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ShooterAIController.generated.h"

//NOTE: kada smo napravili ove klase nisu postojale protected/private/public sekcije niti funkcije u njima, sve smo morali rucno da pisemo
UCLASS()
class SIMPLESHOOTER_API AShooterAIController : public AAIController
{
	GENERATED_BODY()	

public:
	virtual void Tick(float DeltaSeconds) override;
	bool IsDead() const;

protected:
	virtual void BeginPlay() override;

private:
	//UPROPERTY(EditAnywhere)	
	//float AcceptanceRadius = 200.f;	//za MoveTo funkciju, koliko blizu sme AI da pridje

	UPROPERTY(EditAnywhere)	
	//moramo da forward declarujemo BehavirTree kalsu
	class UBehaviorTree* AIBehavior;

};
