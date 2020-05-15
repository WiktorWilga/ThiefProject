// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTLockOnPawn.generated.h"

/**
 * Behavior tree task for locking on target character. 
 */

UCLASS()
class THIEFPROJECT_API UBTLockOnPawn : public UBTTaskNode
{
	GENERATED_BODY()
	
	/**Start task*/
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
