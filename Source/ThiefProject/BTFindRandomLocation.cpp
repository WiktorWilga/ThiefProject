// Fill out your copyright notice in the Description page of Project Settings.


#include "BTFindRandomLocation.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GameCharacter.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "AbstractNavData.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTFindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (Controller)
	{
		AGameCharacter* Character = Cast<AGameCharacter>(Controller->GetPawn());
		if (Character)
		{
			UNavigationSystemV1* NaviSystem = UNavigationSystemV1::GetCurrent(Character);
			UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
			if (NaviSystem && BlackboardComp)
			{
				FNavLocation ResultLocation;
				if(NaviSystem->GetRandomReachablePointInRadius(Character->GetActorLocation(), Radius, ResultLocation)) //found location
				{
					BlackboardComp->SetValueAsVector(PatrolLocationKey, ResultLocation.Location);
					return EBTNodeResult::Type::Succeeded;
				}
				else //failed found location
				{
					return EBTNodeResult::Type::Failed;
				}
			}
		}
	}

	return EBTNodeResult::Type::Failed;
}