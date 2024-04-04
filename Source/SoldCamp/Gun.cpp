// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"



// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGun::GunTriggered()
{
	if (MuzzleParticle != nullptr)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleParticle, Mesh, TEXT("MuzzleFlashSocket"));

		if (Sfx_RevolverFire)
		{
			UGameplayStatics::PlaySoundAtLocation(this, Sfx_RevolverFire, this->GetActorLocation());
		}
	}
}

void AGun::RifleTriggered()
{
	if (MuzzleParticle != nullptr)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleParticle, Mesh, TEXT("MuzzleFlashSocket")
			, FVector(1000, 0, 0));

		if (Sfx_RevolverFire)
		{
			UGameplayStatics::PlaySoundAtLocation(this, Sfx_RevolverFire, this->GetActorLocation());
		}
	}
}

void AGun::PlayReloadSound()
{
	if (Sfx_RevolverReload)
	{
		UGameplayStatics::PlaySoundAtLocation(this, Sfx_RevolverReload, this->GetActorLocation());
	}
}

void AGun::PlayEmptySound()
{
	if (Sfx_Empty)
	{
		UGameplayStatics::PlaySoundAtLocation(this, Sfx_Empty, this->GetActorLocation());
	}
}

