// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Engine/Classes/Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	RootComponent = ItemMesh;

	Item_BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Item_BoxCollision"));
	Item_BoxCollision->SetupAttachment(RootComponent);
	Item_BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AItem::Item_Overlap);
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RotateSelf();
}

void AItem::RotateSelf()
{
	FRotator Temp = FRotator::ZeroRotator;
	Temp.Yaw = 20.0f * GetWorld()->DeltaTimeSeconds;
	AddActorLocalRotation(Temp, false);
}

void AItem::Item_Overlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Destroy();
}

void AItem::Item_EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}
