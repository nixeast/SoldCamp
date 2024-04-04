// Fill out your copyright notice in the Description page of Project Settings.


#include "Target.h"
#include "SoldCampCharacter.h"

// Sets default values
ATarget::ATarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATarget::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentHealth = MaxHealth;
	
	PlayerCharacter0 = Cast<ASoldCampCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	
	if (PlayerCharacter0 != nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("%s"), *PlayerCharacter0->GetName());
	}

}

// Called every frame
void ATarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ATarget::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	CurrentHealth -= DamageToApply;

	UE_LOG(LogTemp, Display, TEXT("Target Current HP:%f"), CurrentHealth);

	if (CurrentHealth <= 0)
	{
		
		ASoldCampCharacter* CurrentCharacter = Cast<ASoldCampCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
		
		if (CurrentCharacter != nullptr)
		{
			CurrentCharacter->SetScoreUp();
		}

		UE_LOG(LogTemp, Display, TEXT("Target Destroyed.."));
		Destroy();
	}

	return DamageToApply;
}

