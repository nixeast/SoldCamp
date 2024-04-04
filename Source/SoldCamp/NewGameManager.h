// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "NewGameManager.generated.h"

/**
 * 
 */
UCLASS()
class SOLDCAMP_API UNewGameManager : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Mesh;

};
