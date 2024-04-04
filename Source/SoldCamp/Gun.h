// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

UCLASS()
class SOLDCAMP_API AGun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void GunTriggered();
	void RifleTriggered();

	void PlayReloadSound();
	void PlayEmptySound();

	UPROPERTY(EditAnywhere)
	UParticleSystem* MuzzleParticle;

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	class USoundBase* Sfx_RevolverFire;
	
	UPROPERTY(EditAnywhere)
	class USoundBase* Sfx_RevolverReload;

	UPROPERTY(EditAnywhere)
	class USoundBase* Sfx_Empty;

private:

	UPROPERTY(EditAnywhere)
	USceneComponent* Root;

	

};
