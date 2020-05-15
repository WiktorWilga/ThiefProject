// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameCharacter.h"
#include "NPCCharacter.generated.h"

/**
Class for NPCs; can move thanks to navigation invoker and logic to controll this pawn in NPCAIController
*/

UCLASS()
class THIEFPROJECT_API ANPCCharacter : public AGameCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPCCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**Instead of create one huge navi mesh on whole world we create small one for every NPC*/
	UPROPERTY(VisibleAnywhere, Category = "NPCCharacter")
		class UNavigationInvokerComponent* NavigationInvoker;

public:	

	UPROPERTY(EditAnywhere)
		class UBehaviorTree* BehaviorTree;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/**Called when our locked on character is dead; this function resetting target for behavior tree*/
	virtual void OnTargetDead(AActor* DeathActor) override;

	/**True if NPC saw player and is charging him*/
	bool IsInAlert();
};
