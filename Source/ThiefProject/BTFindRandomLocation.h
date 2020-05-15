// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTFindRandomLocation.generated.h"

/**
 * Behavior tree task for search random location in given radius
 */

UCLASS()
class THIEFPROJECT_API UBTFindRandomLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
	/**Start task*/
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	/**Blackboard name of key for variable PatrolLocation*/
	UPROPERTY(EditAnywhere, Category = "Task")
		FName PatrolLocationKey;

	/**How far search point to go*/
	UPROPERTY(EditAnywhere, Category = "Task")
		float Radius;
};
