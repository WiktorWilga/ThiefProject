// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCCharacter.h"
#include "NavigationInvokerComponent.h"
#include "AIController.h"
#include "NPCAIController.h"

// Sets default values
ANPCCharacter::ANPCCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	NavigationInvoker = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavigationInvoker"));
}

// Called when the game starts or when spawned
void ANPCCharacter::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ANPCCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANPCCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ANPCCharacter::OnTargetDead(AActor * DeathActor)
{
	Super::OnTargetDead(DeathActor);

	ANPCAIController* MyController = Cast<ANPCAIController>(Controller);
	if (MyController)
	{
		MyController->ResetTarget();
	}
}

bool ANPCCharacter::IsInAlert()
{
	ANPCAIController* MyController = Cast<ANPCAIController>(Controller);
	if (MyController)
	{
		return MyController->IsInAlert();
	}
	return false;
}
