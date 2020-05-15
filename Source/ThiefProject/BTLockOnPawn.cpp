// Fill out your copyright notice in the Description page of Project Settings.


#include "BTLockOnPawn.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GameCharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTLockOnPawn::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (Controller)
	{
		AGameCharacter* Character = Cast<AGameCharacter>(Controller->GetPawn());
		UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
		if (Character && BlackboardComp)
		{
			AGameCharacter* TargetCharacter = Cast<AGameCharacter>(BlackboardComp->GetValueAsObject("Target"));
			if (TargetCharacter)
			{
				Character->LockOnPawn(TargetCharacter);
				return EBTNodeResult::Type::Succeeded;
			}

		}
	}

	return EBTNodeResult::Type::Failed;
}
