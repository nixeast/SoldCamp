// Fill out your copyright notice in the Description page of Project Settings.


#include "ShowerBooth.h"
#include "Kismet/GameplayStatics.h"
#include "MyPlayerController.h"
#include "Engine/Classes/Components/BoxComponent.h"
#include "SoldCampCharacter.h"

// Sets default values
AShowerBooth::AShowerBooth()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ShowerBoothMesh = CreateDefaultSubobject<UStaticMeshComponent>("ShowerBoothMesh");
	SetRootComponent(ShowerBoothMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>("CollisionBox");
	CollisionBox->SetupAttachment(RootComponent);
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AShowerBooth::Overlap_Begin);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &AShowerBooth::Overlap_end);
}

// Called when the game starts or when spawned
void AShowerBooth::BeginPlay()
{
	Super::BeginPlay();
	//UE_LOG(LogTemp, Display, TEXT("ThisShowerBoot Owner:%s"), *this->GetRootComponent()->GetName());
}

// Called every frame
void AShowerBooth::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//CalculateDistanceToPlayer();

	/*if (DistanceToPlayer < 200.0f)
	{
		UE_LOG(LogTemp, Display, TEXT("Player Incomming.."));
		ShouldRotate = true;
	}*/

	if (ShouldRotate == true)
	{
		RotateSelf();
	}

}

void AShowerBooth::Overlap_Begin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Cast<ASoldCampCharacter>(OtherActor)->Washable = true;
}

void AShowerBooth::Overlap_end(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Cast<ASoldCampCharacter>(OtherActor)->Washable = false;
}

void AShowerBooth::RotateSelf()
{
	FRotator Temp = FRotator::ZeroRotator;
	Temp.Yaw = 20.0f * GetWorld()->DeltaTimeSeconds;
	AddActorLocalRotation(Temp, false);
}

void AShowerBooth::CalculateDistanceToPlayer()
{
	FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	DistanceToPlayer = FVector::Dist(PlayerLocation, this->GetActorLocation());
	//UE_LOG(LogTemp, Display, TEXT("DistanceToPlayer:%f"), DistanceToPlayer);
}

