// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"


/**
 * 
 */
UCLASS()
class SOLDCAMP_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	// To add mapping context
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> MyHUDClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> InGameMenuClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> TitleMenuClass;

	UUserWidget* HUD;
	UUserWidget* InGameUI;
	UUserWidget* TitleMenuUI;

public:

	void TurnOnHUD();
	void TurnOffHUD();
	void TuronOnIngameMenu();
	void TuronOffIngameMenu();
	void TuronOnTitleMenu();
	void TuronOffTitleMenu();

};
