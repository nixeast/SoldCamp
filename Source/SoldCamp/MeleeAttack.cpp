// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeAttack.h"
#include "Kismet/KismetSystemLibrary.h"

void UMeleeAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	UE_LOG(LogTemp, Warning, TEXT("MeleeAttack notify.."));
}
