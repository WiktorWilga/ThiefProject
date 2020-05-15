// Fill out your copyright notice in the Description page of Project Settings.


#include "BTSimulateFight.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GameCharacter.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "AbstractNavData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"

EBTNodeResult::Type UBTSimulateFight::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	bNotifyTick = true;
	SavedOwnerComp = &OwnerComp;
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (Controller)
	{
		AGameCharacter* Character = Cast<AGameCharacter>(Controller->GetPawn());
		UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
		SavedCharacter = Character;
		if (Character && BlackboardComp)
		{
			int32 Randed = rand() % 4;
			switch (Randed)
			{
			case 0:
				Character->Attack();
				CurrentStatus = ESimulateFightStatus::SFS_ATTACK;
				return EBTNodeResult::Type::InProgress;
				break;
			case 1:
				GetWorld()->GetTimerManager().SetTimer(StopMoveTimer, this, &UBTSimulateFight::StopMove, 1.0f, false);
				CurrentStatus = ESimulateFightStatus::SFS_MOVE_BACKWARD;
				return EBTNodeResult::Type::InProgress;
				break;
			case 2:
				GetWorld()->GetTimerManager().SetTimer(StopMoveTimer, this, &UBTSimulateFight::StopMove, 1.0f, false);
				CurrentStatus = ESimulateFightStatus::SFS_MOVE_LEFT;
				return EBTNodeResult::Type::InProgress;
				break;
			case 3:
				GetWorld()->GetTimerManager().SetTimer(StopMoveTimer, this, &UBTSimulateFight::StopMove, 1.0f, false);
				CurrentStatus = ESimulateFightStatus::SFS_MOVE_RIGHT;
				return EBTNodeResult::Type::InProgress;
				break;
			}
		}
	}

	return EBTNodeResult::Type::Failed;
}

void UBTSimulateFight::TickTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	switch (CurrentStatus)
	{
	case ESimulateFightStatus::SFS_NONE:
		break;
	case ESimulateFightStatus::SFS_ATTACK:
		if (!SavedCharacter->IsAttacking())
		{
			FinishLatentTask(*SavedOwnerComp, EBTNodeResult::Type::Succeeded);
		}
		break;
	case ESimulateFightStatus::SFS_MOVE_BACKWARD:
		SavedCharacter->GetCharacterMovement()->AddInputVector(-SavedCharacter->GetActorForwardVector());
		break;
	case ESimulateFightStatus::SFS_MOVE_LEFT:
		SavedCharacter->GetCharacterMovement()->AddInputVector(-SavedCharacter->GetActorRightVector());
		break;
	case ESimulateFightStatus::SFS_MOVE_RIGHT:
		SavedCharacter->GetCharacterMovement()->AddInputVector(SavedCharacter->GetActorRightVector());
		break;
	}

}

void UBTSimulateFight::StopMove()
{
	FinishLatentTask(*SavedOwnerComp, EBTNodeResult::Type::Succeeded);
}
