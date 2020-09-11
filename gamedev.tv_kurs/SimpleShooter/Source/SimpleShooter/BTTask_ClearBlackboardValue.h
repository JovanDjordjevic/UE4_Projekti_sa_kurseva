// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ClearBlackboardValue.generated.h"

/**
 * 
 */
UCLASS()
class SIMPLESHOOTER_API UBTTask_ClearBlackboardValue : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	//pravimo sami konstruktor za ovu klasu da bismo mogli da joj damo neko citljivo ime, u suprotnom ce se automatski generisati neko ruzno ime (?)
	UBTTask_ClearBlackboardValue();

protected:
	//ovo je funkcija iz parent klase UBTTakNode, tojest nasa klasa nasledjuje BTTask_BlackboardBase a ta klasa nasledjuje UBTTaskNode
	//NOTE: posto u UBTTaskNode ova funkcija nije definisana ni u jednom bloku (znaci nije ni u public, nije ni u private itd) to znaci da je u nasoj klasi koju pisesmo stavljamo u protected!
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
