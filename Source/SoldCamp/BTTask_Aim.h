// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Aim.generated.h"

/**
 * 
 */
UCLASS()
class SOLDCAMP_API UBTTask_Aim : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_Aim();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
