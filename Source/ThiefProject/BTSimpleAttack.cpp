// Fill out your copyright notice in the Description page of Project Settings.


#include "BTSimpleAttack.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GameCharacter.h"
#include "AIController.h"

EBTNodeResult::Type UBTSimpleAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (Controller)
	{
		AGameCharacter* Character = Cast<AGameCharacter>(Controller->GetPawn());
		if (Character)
		{
			Character->Attack();

			return EBTNodeResult::Type::Succeeded;
		}
	}

	return EBTNodeResult::Type::Failed;
}
