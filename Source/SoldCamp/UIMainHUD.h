// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIMainHUD.generated.h"

/**
 * 
 */
UCLASS()
class SOLDCAMP_API UUIMainHUD : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	// Panels in MainMenu
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCanvasPanel* CanvasPanel_MotherBase;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCanvasPanel* CanvasPanel_Map;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCanvasPanel* CanvasPanel_Mission;

	// Tab Buttons
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UButton* Btn_MotherBase;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UButton* Btn_Map;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UButton* Btn_Mission;
	
	// Buttons on MotherBase Tab
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	class UButton* Btn_Title;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UButton* Btn_Weapons;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UButton* Btn_Save;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UButton* Btn_Resume;

	// Buttons on Mission Tab
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UButton* Btn_SupplyDrop;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UButton* Btn_BuddySupport;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UButton* Btn_MissionList;

	// Panels in Mission Tab Buttons
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCanvasPanel* CanvasPanel_SupplyDrop;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCanvasPanel* CanvasPanel_BuddySupport;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCanvasPanel* CanvasPanel_MissionList;


protected:
	virtual void NativeOnInitialized() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UFUNCTION(BlueprintCallable)
	void GotoTitle();

	UFUNCTION(BlueprintCallable)
	void ResumeButtonCallnback();

	UFUNCTION(BlueprintCallable)
	void SelectSaveDataMenu();
	
	void CloseInGameMenu();
	void UpdateTabColor();
	void UpdatePanelVisibility();
	void CheckInputKey(const FKeyEvent& InKeyEvent);
	void ButtonEnter();
	void SetCurrentBtn(UButton* SelectedBtn);

	class AMyPlayerController* CurrentPlayerController;
	class ASoldCampCharacter* CurrentPlayerCharacter;

	int32 CurrentTabNum = 0;
	int32 CurrentSelectedNum = 0;
	int32 CurrentTabLastBtnNum = 3;

	UButton* CurrentBtn = nullptr;
	UButton* CurrentSelectedBtn = nullptr;
	UButton* UpdateBtnColor();

	
};
