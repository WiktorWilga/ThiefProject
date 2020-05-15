// Fill out your copyright notice in the Description page of Project Settings.


#include "BTEquipeSword.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GameCharacter.h"
#include "AIController.h"

EBTNodeResult::Type UBTEquipeSword::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (Controller)
	{
		AGameCharacter* Character = Cast<AGameCharacter>(Controller->GetPawn());
		if (Character)
		{
			if (Equipe)
			{
				if (!Character->IsUsingSword()) //equipe sword only if it isnt done yet
				{
					Character->ToggleSword();
				}
			}
			else
			{
				if (Character->IsUsingSword()) //unequipe sword only if it isnt done yet
				{
					Character->ToggleSword();
				}
			}

			return EBTNodeResult::Type::Succeeded;
		}
	}

	return EBTNodeResult::Type::Failed;
}