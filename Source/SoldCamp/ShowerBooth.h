// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShowerBooth.generated.h"

UCLASS()
class SOLDCAMP_API AShowerBooth : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShowerBooth();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* ShowerBoothMesh;

	bool ShouldRotate = false;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	UFUNCTION()
	void Overlap_Begin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void Overlap_end(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	void RotateSelf();
	float DistanceToPlayer;
	void CalculateDistanceToPlayer();
};
