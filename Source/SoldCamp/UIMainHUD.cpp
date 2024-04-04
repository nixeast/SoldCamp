// Fill out your copyright notice in the Description page of Project Settings.


#include "UIMainHUD.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "MyPlayerController.h"
#include "Input/Reply.h"
#include "Components/CanvasPanel.h"
#include "SoldCampCharacter.h"

void UUIMainHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	CurrentPlayerController = Cast<AMyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	CurrentPlayerCharacter = Cast<ASoldCampCharacter>(CurrentPlayerController->GetPawn());

	CanvasPanel_MotherBase = Cast<UCanvasPanel>(GetWidgetFromName("CanvasPanel_MotherBase"));
	CanvasPanel_Map = Cast<UCanvasPanel>(GetWidgetFromName("CanvasPanel_Map"));
	CanvasPanel_Mission = Cast<UCanvasPanel>(GetWidgetFromName("CanvasPanel_Mission"));

	Btn_MotherBase = Cast<UButton>(GetWidgetFromName("Btn_MotherBase"));
	Btn_Map = Cast<UButton>(GetWidgetFromName("Btn_Map"));
	Btn_Mission = Cast<UButton>(GetWidgetFromName("Btn_Mission"));

	Btn_Weapons = Cast<UButton>(GetWidgetFromName("Btn_Weapons"));
	Btn_Save = Cast<UButton>(GetWidgetFromName("Btn_Save"));
	Btn_Resume = Cast<UButton>(GetWidgetFromName("Btn_Resume"));
	Btn_Title = Cast<UButton>(GetWidgetFromName("Btn_Title"));

	Btn_SupplyDrop = Cast<UButton>(GetWidgetFromName("Btn_SupplyDrop"));
	Btn_BuddySupport = Cast<UButton>(GetWidgetFromName("Btn_BuddySupport"));
	Btn_MissionList = Cast<UButton>(GetWidgetFromName("Btn_MissionList"));
	
	CanvasPanel_SupplyDrop = Cast<UCanvasPanel>(GetWidgetFromName("CanvasPanel_SupplyDrop"));
	CanvasPanel_BuddySupport = Cast<UCanvasPanel>(GetWidgetFromName("CanvasPanel_BuddySupport"));
	CanvasPanel_MissionList = Cast<UCanvasPanel>(GetWidgetFromName("CanvasPanel_MissionList"));

	Btn_Resume->OnClicked.AddDynamic(this, &UUIMainHUD::ResumeButtonCallnback);
	Btn_Title->OnClicked.AddDynamic(this, &UUIMainHUD::GotoTitle);
	Btn_Save->OnClicked.AddDynamic(this, &UUIMainHUD::SelectSaveDataMenu);

	UpdatePanelVisibility();
	CurrentSelectedBtn = UpdateBtnColor();
	SetCurrentBtn(CurrentSelectedBtn);
	UpdateTabColor();
}

FReply UUIMainHUD::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	CheckInputKey(InKeyEvent);

	CurrentSelectedBtn = UpdateBtnColor();
	SetCurrentBtn(CurrentSelectedBtn);

	UpdateTabColor();
	UpdatePanelVisibility();

	FReply Handle = FReply::Handled();

	return Handle;
}

void UUIMainHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	//UE_LOG(LogTemp, Display, TEXT("CurrentSelectedBtn: %s"), *CurrentSelectedBtn->GetName());
	//UE_LOG(LogTemp, Display, TEXT("CurrentBtn: %s"), *CurrentBtn->GetName());
}


void UUIMainHUD::GotoTitle()
{
	UE_LOG(LogTemp, Display, TEXT("UUIMainHUD::TestButtonCallnback()"));
	UGameplayStatics::OpenLevel(this, FName("Title"));
}

void UUIMainHUD::ResumeButtonCallnback()
{
	if (CurrentPlayerController)
	{
		CurrentPlayerController->TuronOffIngameMenu();
	}
}

void UUIMainHUD::CloseInGameMenu()
{
	if (CurrentPlayerController)
	{
		CurrentPlayerController->TuronOffIngameMenu();
	}
}

UButton* UUIMainHUD::UpdateBtnColor()
{
	if (CurrentTabNum == 0)
	{
		if (CurrentSelectedNum == 0)
		{
			Btn_Weapons->SetBackgroundColor(FLinearColor::Blue);
			Btn_Save->SetBackgroundColor(FLinearColor::White);
			Btn_Resume->SetBackgroundColor(FLinearColor::White);
			Btn_Title->SetBackgroundColor(FLinearColor::White);

			return Btn_Weapons;
		}
		else if (CurrentSelectedNum == 1)
		{
			Btn_Weapons->SetBackgroundColor(FLinearColor::White);
			Btn_Save->SetBackgroundColor(FLinearColor::Blue);
			Btn_Resume->SetBackgroundColor(FLinearColor::White);
			Btn_Title->SetBackgroundColor(FLinearColor::White);

			return Btn_Save;
		}
		else if (CurrentSelectedNum == 2)
		{
			Btn_Weapons->SetBackgroundColor(FLinearColor::White);
			Btn_Save->SetBackgroundColor(FLinearColor::White);
			Btn_Resume->SetBackgroundColor(FLinearColor::Blue);
			Btn_Title->SetBackgroundColor(FLinearColor::White);

			return Btn_Resume;
		}
		else if (CurrentSelectedNum == 3)
		{
			Btn_Weapons->SetBackgroundColor(FLinearColor::White);
			Btn_Save->SetBackgroundColor(FLinearColor::White);
			Btn_Resume->SetBackgroundColor(FLinearColor::White);
			Btn_Title->SetBackgroundColor(FLinearColor::Blue);

			return Btn_Title;
		}
	}
	else if (CurrentTabNum == 2)
	{
		if (CurrentSelectedNum == 0)
		{
			Btn_SupplyDrop->SetBackgroundColor(FLinearColor::Blue);
			Btn_BuddySupport->SetBackgroundColor(FLinearColor::White);
			Btn_MissionList->SetBackgroundColor(FLinearColor::White);

			return Btn_SupplyDrop;
		}
		else if (CurrentSelectedNum == 1)
		{
			Btn_SupplyDrop->SetBackgroundColor(FLinearColor::White);
			Btn_BuddySupport->SetBackgroundColor(FLinearColor::Blue);
			Btn_MissionList->SetBackgroundColor(FLinearColor::White);
			return Btn_BuddySupport;
		}
		else if (CurrentSelectedNum == 2)
		{
			Btn_SupplyDrop->SetBackgroundColor(FLinearColor::White);
			Btn_BuddySupport->SetBackgroundColor(FLinearColor::White);
			Btn_MissionList->SetBackgroundColor(FLinearColor::Blue);

			return Btn_MissionList;
		}
	}
	
	return nullptr;

}

void UUIMainHUD::UpdateTabColor()
{
	if (CurrentTabNum == 0)
	{
		Btn_MotherBase->SetBackgroundColor(FLinearColor::Blue);
		Btn_Map->SetBackgroundColor(FLinearColor::White);
		Btn_Mission->SetBackgroundColor(FLinearColor::White);
	}
	else if (CurrentTabNum == 1)
	{
		Btn_MotherBase->SetBackgroundColor(FLinearColor::White);
		Btn_Map->SetBackgroundColor(FLinearColor::Blue);
		Btn_Mission->SetBackgroundColor(FLinearColor::White);
	}
	else if (CurrentTabNum == 2)
	{
		Btn_MotherBase->SetBackgroundColor(FLinearColor::White);
		Btn_Map->SetBackgroundColor(FLinearColor::White);
		Btn_Mission->SetBackgroundColor(FLinearColor::Blue);
	}
}

void UUIMainHUD::UpdatePanelVisibility()
{
	if (CurrentTabNum == 0)
	{
		CanvasPanel_MotherBase->SetVisibility(ESlateVisibility::Visible);
		CanvasPanel_Map->SetVisibility(ESlateVisibility::Hidden);
		CanvasPanel_Mission->SetVisibility(ESlateVisibility::Hidden);
	}
	else if (CurrentTabNum == 1)
	{
		CanvasPanel_MotherBase->SetVisibility(ESlateVisibility::Hidden);
		CanvasPanel_Map->SetVisibility(ESlateVisibility::Visible);
		CanvasPanel_Mission->SetVisibility(ESlateVisibility::Hidden);
	}
	else if (CurrentTabNum == 2)
	{
		CanvasPanel_MotherBase->SetVisibility(ESlateVisibility::Hidden);
		CanvasPanel_Map->SetVisibility(ESlateVisibility::Hidden);
		CanvasPanel_Mission->SetVisibility(ESlateVisibility::Visible);

		if (CurrentSelectedNum == 0)
		{
			CanvasPanel_SupplyDrop->SetVisibility(ESlateVisibility::Visible);
			CanvasPanel_BuddySupport->SetVisibility(ESlateVisibility::Hidden);
			CanvasPanel_MissionList->SetVisibility(ESlateVisibility::Hidden);
		}
		else if (CurrentSelectedNum == 1)
		{
			CanvasPanel_SupplyDrop->SetVisibility(ESlateVisibility::Hidden);
			CanvasPanel_BuddySupport->SetVisibility(ESlateVisibility::Visible);
			CanvasPanel_MissionList->SetVisibility(ESlateVisibility::Hidden);
		}
		else if (CurrentSelectedNum == 2)
		{
			CanvasPanel_SupplyDrop->SetVisibility(ESlateVisibility::Hidden);
			CanvasPanel_BuddySupport->SetVisibility(ESlateVisibility::Hidden);
			CanvasPanel_MissionList->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UUIMainHUD::CheckInputKey(const FKeyEvent& InKeyEvent)
{
	FKey PressedKey = InKeyEvent.GetKey();

	UE_LOG(LogTemp, Display, TEXT("CurrentSelectedNum: %s"), *PressedKey.ToString());

	if (PressedKey == FKey(TEXT("S")))
	{
		if (CurrentSelectedNum < CurrentTabLastBtnNum)
		{
			CurrentSelectedNum++;
		}
	}
	else if (PressedKey == FKey(TEXT("W")))
	{
		if (CurrentSelectedNum > 0)
		{
			CurrentSelectedNum--;
		}
	}
	else if (PressedKey == FKey(TEXT("One")))
	{
		if (CurrentTabNum > 0)
		{
			CurrentTabNum--;
			CurrentSelectedNum = 0;

			if (CurrentTabNum == 0)
			{
				CurrentTabLastBtnNum = 3;
			}
		}
	}
	else if (PressedKey == FKey(TEXT("Three")))
	{
		if (CurrentTabNum < 2)
		{
			CurrentTabNum++;
			CurrentSelectedNum = 0;

			if (CurrentTabNum == 2)
			{
				CurrentTabLastBtnNum = 2;
			}
		}
	}
	else if (PressedKey == FKey(TEXT("Tab")))
	{
		CloseInGameMenu();
	}
	else if (PressedKey == FKey(TEXT("Enter")))
	{
		ButtonEnter();
	}

	UE_LOG(LogTemp, Display, TEXT("CurrentNum: %d"), CurrentSelectedNum);
	UE_LOG(LogTemp, Display, TEXT("CurrentTab: %d"), CurrentTabNum);

}

void UUIMainHUD::ButtonEnter()
{
	if (CurrentBtn != nullptr)
	{
		CurrentBtn->OnClicked.Broadcast();
		UE_LOG(LogTemp, Display, TEXT("CurrentBtn->OnClicked.Broadcast()"));
	}
}

void UUIMainHUD::SetCurrentBtn(UButton* SelectedBtn)
{
	CurrentBtn = SelectedBtn;
	if (CurrentBtn != nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("CurrentBtn: %s"), *CurrentBtn->GetName());
	}
}

void UUIMainHUD::SelectSaveDataMenu()
{
	if (CurrentPlayerCharacter != nullptr)
	{
		CurrentPlayerCharacter->SavePlayerData();
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Can't find PlayerCharacter"));
	}
}
