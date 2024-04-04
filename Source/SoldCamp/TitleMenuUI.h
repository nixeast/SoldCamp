// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TitleMenuUI.generated.h"

/**
 * 
 */
UCLASS()
class SOLDCAMP_API UTitleMenuUI : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnInitialized() override;

public:
	UFUNCTION(BlueprintCallable)
	void GotoIngame();

	UFUNCTION(BlueprintCallable)
	void SelectResetData();

protected:
	class UButton* Btn_Continue;
	class UButton* Btn_ResetData;

private:
	class AMyPlayerController* CurrentPlayerController;
	class ASoldCampCharacter* CurrentPlayer;
};
