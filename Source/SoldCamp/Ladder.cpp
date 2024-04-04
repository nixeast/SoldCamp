// Fill out your copyright notice in the Description page of Project Settings.


#include "Ladder.h"
#include "Engine/Classes/Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "SoldCampCharacter.h"
#include "Components/TextRenderComponent.h"


// Sets default values
ALadder::ALadder()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MyLadderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MyLadderMesh"));
	SetRootComponent(MyLadderMesh);

	LadderCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LadderCollisionBox"));
	LadderCollisionBox->SetupAttachment(RootComponent);
	LadderCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ALadder::OverlapLadder);
	LadderCollisionBox->OnComponentEndOverlap.AddDynamic(this, &ALadder::EndOverlapLadder);

	LadderLandingCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LadderLandingCollisionBox"));
	LadderLandingCollisionBox->SetupAttachment(RootComponent);
	LadderLandingCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ALadder::OverlapLadderLandZone);
	LadderLandingCollisionBox->OnComponentEndOverlap.AddDynamic(this, &ALadder::EndOverlapLadderLandZone);

	LadderClimbDownCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LadderClimbDownCollision"));
	LadderClimbDownCollision->SetupAttachment(RootComponent);
	LadderClimbDownCollision->OnComponentBeginOverlap.AddDynamic(this, &ALadder::OverlapClimbDownZone);
	LadderClimbDownCollision->OnComponentEndOverlap.AddDynamic(this, &ALadder::EndOverlapClimbDownZone);

	TextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComponent"));
	TextRenderComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ALadder::BeginPlay()
{
	Super::BeginPlay();

	TextRenderComponent->SetVisibility(false);
	
}

// Called every frame
void ALadder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALadder::OverlapLadder(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (Cast<ASoldCampCharacter>(OtherActor) != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor Overlapped.."));
		Cast<ASoldCampCharacter>(OtherActor)->Climbable = true;
		TextRenderComponent->SetVisibility(true);

		Cast<ASoldCampCharacter>(OtherActor)->SetTargetClimbLocation(this);
	}

}

void ALadder::EndOverlapLadder(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
	if (Cast<ASoldCampCharacter>(OtherActor) != nullptr)
	{
		if (Cast<ASoldCampCharacter>(OtherActor)->ShouldClimb == true)
		{
		UE_LOG(LogTemp, Warning, TEXT("Actor Overlap Ended.."));
		Cast<ASoldCampCharacter>(OtherActor)->Climbable = false;
		Cast<ASoldCampCharacter>(OtherActor)->UpdateMovementMode();
		TextRenderComponent->SetVisibility(false);
		}
	}

}

void ALadder::OverlapLadderLandZone(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ASoldCampCharacter>(OtherActor) != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor Overlapped LadderLandZone.."));
		Cast<ASoldCampCharacter>(OtherActor)->LadderLandable = true;

		if (Cast<ASoldCampCharacter>(OtherActor)->ShouldClimb == true && Cast<ASoldCampCharacter>(OtherActor)->LadderLandable == true)
		{
			Cast<ASoldCampCharacter>(OtherActor)->ShouldClimb = false;
			Cast<ASoldCampCharacter>(OtherActor)->LadderLanding();
		}

	}
}

void ALadder::EndOverlapLadderLandZone(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<ASoldCampCharacter>(OtherActor) != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor EndOverlapped LadderLandZone.."));
		Cast<ASoldCampCharacter>(OtherActor)->LadderLandable = false;
	}
}

void ALadder::OverlapClimbDownZone(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ASoldCampCharacter>(OtherActor) != nullptr)
	{
		Cast<ASoldCampCharacter>(OtherActor)->LadderClimbDownable = true;
		UE_LOG(LogTemp, Warning, TEXT("Actor Overlapped LadderClimbDownZone.."));
		Cast<ASoldCampCharacter>(OtherActor)->SetTargetClimbLocation(this);
	}
}

void ALadder::EndOverlapClimbDownZone(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<ASoldCampCharacter>(OtherActor) != nullptr)
	{
		Cast<ASoldCampCharacter>(OtherActor)->LadderClimbDownable = false;
		UE_LOG(LogTemp, Warning, TEXT("Actor EndOverlapped LadderClimbDownZone.."));
	}
}
