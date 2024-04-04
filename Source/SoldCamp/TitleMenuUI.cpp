// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleMenuUI.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "MyPlayerController.h"
#include "SoldCampCharacter.h"

void UTitleMenuUI::NativeOnInitialized()
{
	CurrentPlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
	CurrentPlayer = Cast<ASoldCampCharacter>(CurrentPlayerController->GetPawn());

	Btn_Continue = Cast<UButton>(GetWidgetFromName("Btn_Continue"));
	Btn_ResetData = Cast<UButton>(GetWidgetFromName("Btn_ResetData"));

	Btn_Continue->OnClicked.AddDynamic(this, &UTitleMenuUI::GotoIngame);
	Btn_ResetData->OnClicked.AddDynamic(this, &UTitleMenuUI::SelectResetData);
}

void UTitleMenuUI::GotoIngame()
{
	if (CurrentPlayerController)
	{
		CurrentPlayerController->TuronOffTitleMenu();
	}

	UE_LOG(LogTemp, Display, TEXT("GotoIngame.."));
	UGameplayStatics::OpenLevel(this, FName("ThirdPersonMap"));
}

void UTitleMenuUI::SelectResetData()
{
	if (CurrentPlayer)
	{
		CurrentPlayer->ResetPlayerData();
		UE_LOG(LogTemp, Display, TEXT("Reset completed.."));
	}
}
