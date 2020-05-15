// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTSetWalkSpeed.generated.h"

/**
 * Behavior tree task for set character walk/run speed
 */

UCLASS()
class THIEFPROJECT_API UBTSetWalkSpeed : public UBTTaskNode
{
	GENERATED_BODY()

	/**Start task*/
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	/**New value to set*/
	UPROPERTY(EditAnywhere, Category = "Task")
		float NewSpeed;
};
