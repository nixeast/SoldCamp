// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Shoot.h"
#include "AIController.h"
#include "SoldCampCharacter.h"
#include "EnemyAIController.h"

UBTTask_Shoot::UBTTask_Shoot()
{
	NodeName = TEXT("Shoot");
}

EBTNodeResult::Type UBTTask_Shoot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	Character->Shoot();

	Cast<AEnemyAIController>(OwnerComp.GetAIOwner())->StartAttackSequence();

	return EBTNodeResult::Succeeded;
}
