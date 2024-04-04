// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Aim.h"
#include "AIController.h"
#include "SoldCampCharacter.h"
#include "EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Aim::UBTTask_Aim()
{
	NodeName = TEXT("Aim");
}

EBTNodeResult::Type UBTTask_Aim::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (OwnerComp.GetAIOwner() == nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("OwnerComp.GetAIOwner() == nullptr"));
		return EBTNodeResult::Failed;
	}

	ASoldCampCharacter* Character = Cast<ASoldCampCharacter>(OwnerComp.GetAIOwner()->GetPawn());

	if (Character == nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("Character == nullptr"));
		return EBTNodeResult::Failed;
	}

	Character->Aimed();

	/*if (Cast<AEnemyAIController>(OwnerComp.GetAIOwner())->GetBlackboardComponent()->GetValueAsBool(TEXT("CurrentAttack")) == true)
	{
		Cast<AEnemyAIController>(OwnerComp.GetAIOwner())->EndAttackSequence();
	}*/


	return EBTNodeResult::Succeeded;
}
