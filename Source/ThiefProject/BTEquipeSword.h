// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTEquipeSword.generated.h"

/**
 * Behavior tree task for ()unequipe sword. It dont unequipe sword when sword is allready equiped and call this task and conversely.
 */

UCLASS()
class THIEFPROJECT_API UBTEquipeSword : public UBTTaskNode
{
	GENERATED_BODY()

	/**True equipe sword, false - unequipe*/
	UPROPERTY(EditAnywhere, Category = "Task")
		bool Equipe;

	/**Start task*/
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
