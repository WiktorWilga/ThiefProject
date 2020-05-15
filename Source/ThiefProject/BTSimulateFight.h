// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTSimulateFight.generated.h"

/**
 * 
 */

enum class ESimulateFightStatus 
{
	SFS_NONE,
	SFS_ATTACK,
	SFS_MOVE_BACKWARD,
	SFS_MOVE_LEFT,
	SFS_MOVE_RIGHT
};

UCLASS()
class THIEFPROJECT_API UBTSimulateFight : public UBTTaskNode
{
	GENERATED_BODY()

	ESimulateFightStatus CurrentStatus = ESimulateFightStatus::SFS_NONE;

	FTimerHandle StopMoveTimer;

	class UBehaviorTreeComponent* SavedOwnerComp;
	class AGameCharacter* SavedCharacter;

	/**Start task*/
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UFUNCTION()
		void StopMove();
};
