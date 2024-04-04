// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "MeleeAttackState.generated.h"

/**
 * 
 */
UCLASS()
class SOLDCAMP_API UMeleeAttackState : public UAnimNotifyState
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

	class ACharacter* CurrentPlayerCharacter = nullptr;

};
