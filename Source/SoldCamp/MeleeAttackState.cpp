// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeAttackState.h"
#include "Kismet/KismetSystemLibrary.h"

void UMeleeAttackState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	
	FVector Start = MeshComp->GetSocketLocation(TEXT("hand_l_Socket_base"));
	FVector End = MeshComp->GetSocketLocation(TEXT("hand_l_Socket_tip"));
	FHitResult OutHit;

	TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> ActorsToIgnore;

	bool Result = UKismetSystemLibrary::SphereTraceSingleForObjects(
		MeshComp->GetOwner(), Start,End,20.0f, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHit,true);

	if (Result == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit True.."));
	}

}
