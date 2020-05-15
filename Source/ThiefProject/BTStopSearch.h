// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTStopSearch.generated.h"

/**
 * Behavior tree task for stop searching player and go pack to patrol
 */

UCLASS()
class THIEFPROJECT_API UBTStopSearch : public UBTTaskNode
{
	GENERATED_BODY()
	
	/**Start task*/
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	/**Blackboard name of key for SearchLocation*/
	UPROPERTY(EditAnywhere, Category = "Task")
		FName SearchLocationKey;
};
