// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "NPCAIController.generated.h"

/**
 * Class for AI controller for NPC; here is logic for random movement in specified radius
 */

UCLASS()
class THIEFPROJECT_API ANPCAIController : public AAIController
{
	GENERATED_BODY()

public:

	ANPCAIController();

protected:

	/**AI perception for NPCs sight*/
	UPROPERTY(VisibleAnywhere, Category = "NPCAIController")
		class UPawnSensingComponent* PawnSensing;

	/**Blackboard component for BehaviorTree data*/
	class UBlackboardComponent* BlackboardComp;

	/**Behavior tree component for AI*/
	class UBehaviorTreeComponent* BehaviorTreeComp;

	/**Timer to start reseting AI sensing, when NPC lose player*/
	FTimerHandle ResetSensesTimer;

	/**Name of blackboard key for Target*/
	UPROPERTY(EditDefaultsOnly, Category = "NPCAIController")
		FName TargetKey;

	/**Name of blackboard key for SerachLocation*/
	UPROPERTY(EditDefaultsOnly, Category = "NPCAIController")
		FName SearchLocationKey;

public:

	void BeginPlay() override;

	/**Used to bind sensing functions, set behaior tree and blackboard*/
	virtual void OnPossess(APawn* Pawn) override;

	/**Claear NPC info about target pawn, has line of sight*/
	void ResetSensesMemory();

	/**PawnSense call it when see player*/
	UFUNCTION()
		void OnSeePlayer(class APawn* ObservedPawn);

	/**PawnSense call it when hear noise from player*/
	UFUNCTION()
		void OnHearNoise(class APawn* PawnInstigator, const FVector& Location, float Volume);

	/**Reset Target and SearchLocation value from behavior tree; it is used to go back npc to patrolling etc*/
	void ResetTarget();

	/**True if NPC saw player and is charging him*/
	bool IsInAlert();
};
