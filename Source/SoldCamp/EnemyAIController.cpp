// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "SoldCampCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense.h"

AEnemyAIController::AEnemyAIController()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	AIPerception->OnPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnPerceptionMessage);
	
	PlayerPawn = Cast<ASoldCampCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	//DistanceToPlayer = FVector::Dist(GetPawn()->GetActorLocation(), PlayerPawn->GetActorLocation());

	if (AIBehavior == nullptr)
	{
		return;
	}

	RunBehaviorTree(AIBehavior);

	InitBlackboardComponent();

	SetGoalFlag();

	SetAICharacterMovement();

}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TrackPlayerLocation();
	UpdateDistanceToPlayer();
	UpdatePlayerFoundState();
	UpdateLastKnownPlayerLocation();

}

void AEnemyAIController::OnPerceptionMessage(const TArray<AActor*>& UpdatedActors)
{
	if (UpdatedActors.Num() > 0)
	{
		for (int i = 0; i < UpdatedActors.Num(); i++)
		{
			PlayerPawn = Cast<ASoldCampCharacter>(UpdatedActors[i]);
			if (PlayerPawn == nullptr) return;

			SwitchSuspectMode();
			
		}
	}
}

void AEnemyAIController::UpdateLastKnownPlayerLocation()
{
	if (SuspectMode == true)
	{
		GetBlackboardComponent()->SetValueAsVector(TEXT("LastPlayerLocation"), PlayerPawn->GetActorLocation());
	}
}

void AEnemyAIController::SetBlackboardDeadMode()
{
	GetBlackboardComponent()->SetValueAsBool(TEXT("bb_dead"), true);
}

void AEnemyAIController::SetBlackboardHitMode(bool value)
{
	GetBlackboardComponent()->SetValueAsBool(TEXT("bb_hit"), value);
}

void AEnemyAIController::InitBlackboardComponent()
{
	GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
	GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());
	GetBlackboardComponent()->SetValueAsBool(TEXT("PlayerNotice"), PlayerFound);
}

void AEnemyAIController::UpdateDistanceToPlayer()
{
	DistanceToPlayer = FVector::Dist(GetPawn()->GetActorLocation(), PlayerPawn->GetActorLocation());
	//UE_LOG(LogTemp, Display, TEXT("Dist2Player:%f"), DistanceToPlayer);
}

void AEnemyAIController::UpdatePlayerFoundState()
{
	if (DistanceToPlayer < AttackSenseRadius)
	{
		PlayerFound = true;

		if (GetBlackboardComponent()->GetValueAsBool(TEXT("CurrentAttack")) == false)
		{
			GetBlackboardComponent()->SetValueAsBool(TEXT("CurrentAttack"), true);
		}

		if (GetBlackboardComponent()->GetValueAsBool(TEXT("PlayerNotice")) == false)
		{
			GetBlackboardComponent()->SetValueAsBool(TEXT("PlayerNotice"), PlayerFound);
		}
	}
	else
	{
		PlayerFound = false;
		
		if (GetBlackboardComponent()->GetValueAsBool(TEXT("CurrentAttack")) == true)
		{
			GetBlackboardComponent()->SetValueAsBool(TEXT("CurrentAttack"), false);
		}

		if (GetBlackboardComponent()->GetValueAsBool(TEXT("PlayerNotice")) == true)
		{
			GetBlackboardComponent()->SetValueAsBool(TEXT("PlayerNotice"), PlayerFound);
			GetBlackboardComponent()->SetValueAsBool(TEXT("CurrentAttack"), true);
		}
	}
}

void AEnemyAIController::TrackPlayerLocation()
{
	if (AIBehavior != nullptr)
	{
		GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
	}
}

void AEnemyAIController::SetGoalFlag()
{
	if (GoalFlag != nullptr)
	{
		GetBlackboardComponent()->SetValueAsVector(TEXT("GoalLocation"), GoalFlag->GetActorLocation());
	}

	if (Cast<ASoldCampCharacter>(this->GetPawn())->TargetToPatrol != nullptr)
	{
		//UE_LOG(LogTemp, Display, TEXT("Owner:%s"), *Cast<ASoldCampCharacter>(this->GetPawn())->GetName());
		GoalFlag = Cast<ASoldCampCharacter>(this->GetPawn())->TargetToPatrol;
		GetBlackboardComponent()->SetValueAsVector(TEXT("GoalLocation"), GoalFlag->GetActorLocation());
	}

}

void AEnemyAIController::SetAICharacterMovement()
{
	Cast<ASoldCampCharacter>(GetPawn())->GetCharacterMovement()->MaxWalkSpeed = 100.0f;
	Cast<ASoldCampCharacter>(GetPawn())->bUseControllerRotationYaw = true;
}

void AEnemyAIController::SwitchSuspectMode()
{
	if (SuspectMode == false)
	{
		SuspectMode = true;
		UE_LOG(LogTemp, Display, TEXT("SuspectMode starts.."));
	}
	else if (SuspectMode == true)
	{
		SuspectMode = false;
		UE_LOG(LogTemp, Display, TEXT("SuspectMode ends.."));
	}
	
	SetBlackboardSuspectMode();

}

void AEnemyAIController::SetBlackboardSuspectMode()
{
	GetBlackboardComponent()->SetValueAsBool(TEXT("bb_SuspectMode"), SuspectMode);
}

void AEnemyAIController::StartAttackSequence()
{
	GetBlackboardComponent()->SetValueAsBool(TEXT("CurrentAttack"), true);
}

void AEnemyAIController::EndAttackSequence()
{
	GetBlackboardComponent()->SetValueAsBool(TEXT("CurrentAttack"), false);
}
