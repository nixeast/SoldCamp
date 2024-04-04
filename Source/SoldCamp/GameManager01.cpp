// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager01.h"
#include "SoldCampCharacter.h"
#include "EnemyAIController.h"
#include "Item.h"

// Sets default values
AGameManager01::AGameManager01()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	

}

// Called when the game starts or when spawned
void AGameManager01::BeginPlay()
{
	Super::BeginPlay();
	
	//Cast<AEnemyAIController>(Enemy->GetController())->GoalFlag = Flag01;

	//UE_LOG(LogTemp, Warning, TEXT("%s"),*Flag01->GetPathName());
	

	if (FindObject<AActor>(nullptr, TEXT("/Game/ThirdPerson/Maps/ThirdPersonMap.ThirdPersonMap:PersistentLevel.StaticMeshActor_UAID_FCAA149A510229BF01_1305338078")) != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Flag Found.."));
		UE_LOG(LogTemp, Warning, TEXT("Actor name:%s"),*FindObject<AActor>(nullptr, TEXT("/Game/ThirdPerson/Maps/ThirdPersonMap.ThirdPersonMap:PersistentLevel.StaticMeshActor_UAID_FCAA149A510229BF01_1305338078"))->GetActorNameOrLabel());
		TestFlag = FindObject<AActor>(nullptr, TEXT("/Game/ThirdPerson/Maps/ThirdPersonMap.ThirdPersonMap:PersistentLevel.StaticMeshActor_UAID_FCAA149A510229BF01_1305338078"));
		UE_LOG(LogTemp, Warning, TEXT("Location of TestFlag:%s"), *TestFlag->GetActorLocation().ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot found flag.."));
	}
}

// Called every frame
void AGameManager01::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

