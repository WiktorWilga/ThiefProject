// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCAIController.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "GameCharacter.h"
#include "Perception/PawnSensingComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "NPCCharacter.h"
#include "Perception/PawnSensingComponent.h"

ANPCAIController::ANPCAIController()
{
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));

	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard"));
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTree"));
}

void ANPCAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ANPCAIController::OnPossess(APawn* MyPawn)
{
	Super::OnPossess(MyPawn);

	PawnSensing->OnSeePawn.AddDynamic(this, &ANPCAIController::OnSeePlayer);
	PawnSensing->OnHearNoise.AddDynamic(this, &ANPCAIController::OnHearNoise);

	ANPCCharacter* NPCCharacter = Cast<ANPCCharacter>(MyPawn);
	if (NPCCharacter && NPCCharacter->BehaviorTree  && NPCCharacter->BehaviorTree->BlackboardAsset)
	{
		//Initialize the blackboard values
		BlackboardComp->InitializeBlackboard(*NPCCharacter->BehaviorTree->BlackboardAsset);
		//Start the tree
		BehaviorTreeComp->StartTree(*NPCCharacter->BehaviorTree);
	}
}

void ANPCAIController::ResetSensesMemory()
{
	BlackboardComp->SetValueAsObject("Target", nullptr);
}

void ANPCAIController::OnSeePlayer(APawn* ObservedPawn)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "see");

	AGameCharacter* MyCharacter = Cast<AGameCharacter>(GetPawn());
	AGameCharacter* ObservedCharacter = Cast<AGameCharacter>(ObservedPawn);

	if (MyCharacter && ObservedCharacter && ObservedCharacter->IsMyEnemy(MyCharacter))
	{

		GetWorld()->GetTimerManager().ClearTimer(ResetSensesTimer);

		BlackboardComp->SetValueAsObject(TargetKey, ObservedCharacter);

	}
}

void ANPCAIController::OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "hear");

	AGameCharacter* MyCharacter = Cast<AGameCharacter>(GetPawn());
	AGameCharacter* ObservedCharacter = Cast<AGameCharacter>(PawnInstigator);

	if (MyCharacter && ObservedCharacter && ObservedCharacter->IsMyEnemy(MyCharacter))
	{

		GetWorld()->GetTimerManager().ClearTimer(ResetSensesTimer);

		BlackboardComp->SetValueAsVector(SearchLocationKey, Location);

	}
}

void ANPCAIController::ResetTarget()
{
	BlackboardComp->ClearValue(TargetKey);
	BlackboardComp->ClearValue(SearchLocationKey);
}

bool ANPCAIController::IsInAlert()
{
	return (BlackboardComp->GetValueAsObject(TargetKey) != nullptr);
}