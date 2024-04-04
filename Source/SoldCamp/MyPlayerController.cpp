// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"


void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
		
	HUD = CreateWidget(this, MyHUDClass);
	HUD->AddToViewport();

	InGameUI = CreateWidget(this, InGameMenuClass);
	InGameUI->AddToViewport();

	TitleMenuUI = CreateWidget(this, TitleMenuClass);
	TitleMenuUI->AddToViewport();

	UE_LOG(LogTemp, Display, TEXT("CurrentLevel:%s"), *GetWorld()->GetMapName());	

	if (GetWorld()->GetMapName() == TEXT("UEDPIE_0_ThirdPersonMap"))
	{
		HUD->SetVisibility(ESlateVisibility::Hidden);
		InGameUI->SetVisibility(ESlateVisibility::Hidden);
		TitleMenuUI->SetVisibility(ESlateVisibility::Hidden);
	}
	else if (GetWorld()->GetMapName() == TEXT("UEDPIE_0_Title"))
	{
		HUD->SetVisibility(ESlateVisibility::Hidden);
		InGameUI->SetVisibility(ESlateVisibility::Hidden);
		TitleMenuUI->SetVisibility(ESlateVisibility::Visible);
		TuronOnTitleMenu();
	}
}

void AMyPlayerController::TurnOnHUD()
{
	if (HUD != nullptr)
	{
		HUD->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMyPlayerController::TurnOffHUD()
{
	if (HUD != nullptr)
	{
		HUD->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMyPlayerController::TuronOnIngameMenu()
{
	if (InGameUI != nullptr)
	{
		SetShowMouseCursor(false);
		SetInputMode(FInputModeUIOnly());
		InGameUI->SetVisibility(ESlateVisibility::Visible);
		InGameUI->SetKeyboardFocus();
	}
}

void AMyPlayerController::TuronOffIngameMenu()
{
	if (InGameUI != nullptr)
	{
		SetInputMode(FInputModeGameOnly());
		SetShowMouseCursor(false);
		InGameUI->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMyPlayerController::TuronOnTitleMenu()
{
	if (TitleMenuUI != nullptr)
	{
		SetInputMode(FInputModeUIOnly());
		SetShowMouseCursor(true);
		TitleMenuUI->SetVisibility(ESlateVisibility::Visible);
		TitleMenuUI->SetKeyboardFocus();
	}
}

void AMyPlayerController::TuronOffTitleMenu()
{
	if (TitleMenuUI != nullptr)
	{
		SetInputMode(FInputModeGameOnly());
		SetShowMouseCursor(false);
		TitleMenuUI->SetVisibility(ESlateVisibility::Hidden);
		UE_LOG(LogTemp, Display, TEXT("TuronOffTitleMenu.."));
	}
}
