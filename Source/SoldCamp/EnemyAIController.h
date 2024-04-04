// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameManager01.h"

#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class SOLDCAMP_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	AEnemyAIController();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	class ASoldCampCharacter* PlayerPawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	class UBehaviorTree* AIBehavior;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool PlayerInAttackRange = false;

	UPROPERTY(EditAnywhere)
	bool PlayerFound = false;

	UPROPERTY(EditAnywhere)
	class AActor* GoalFlag;

	void StartAttackSequence();
	void EndAttackSequence();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	class UAIPerceptionComponent* AIPerception;

	UPROPERTY(EditAnywhere, Category="AI")
	bool SuspectMode = false;

	UFUNCTION()
	void OnPerceptionMessage(const TArray<AActor*>& UpdatedActors);

	void InitBlackboardComponent();

	void UpdateDistanceToPlayer();
	void UpdatePlayerFoundState();
	void TrackPlayerLocation();

	void SetGoalFlag();
	void SetAICharacterMovement();

	void SwitchSuspectMode();
	void SetBlackboardSuspectMode();
	void UpdateLastKnownPlayerLocation();
	void SetBlackboardDeadMode();
	void SetBlackboardHitMode(bool value);

private:
	
	float DistanceToPlayer;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	float AttackSenseRadius = 400.0f;
};
