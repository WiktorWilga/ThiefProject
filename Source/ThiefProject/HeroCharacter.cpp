// Fill out your copyright notice in the Description page of Project Settings.


#include "HeroCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Controller.h"
#include "TimerManager.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/BoxComponent.h"
#include "NPCCharacter.h"

// Sets default values
AHeroCharacter::AHeroCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArm->SetupAttachment(RootComponent);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraSpringArm, USpringArmComponent::SocketName);

	WaistKnifeLocation = CreateDefaultSubobject<USceneComponent>(TEXT("WaistKnifeLocation"));
	WaistKnifeLocation->SetupAttachment(GetMesh(), "LeftUpLegSocket");

	HandKnifeLocation = CreateDefaultSubobject<USceneComponent>(TEXT("HandKnifeLocation"));
	HandKnifeLocation->SetupAttachment(GetMesh(), "RightHandIndex1Socket");

	Knife = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Knife"));
	Knife->SetupAttachment(WaistKnifeLocation);

	FrontBoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("FrontBoxCollision"));
	FrontBoxCollision->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AHeroCharacter::BeginPlay()
{
	Super::BeginPlay();

	Knife->OnComponentBeginOverlap.AddDynamic(this, &AHeroCharacter::OnKnifeBeginOverlapp);
}

// Called every frame
void AHeroCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//WARNING: in tick is also called DoLockedOnPawnBehavior (from parent class)
}

// Called to bind functionality to input
void AHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Forward", this, &AHeroCharacter::Forward);
	PlayerInputComponent->BindAxis("Turn", this, &AHeroCharacter::Turn);
	PlayerInputComponent->BindAxis("MouseX", this, &AHeroCharacter::MouseX);
	PlayerInputComponent->BindAxis("MouseY", this, &AHeroCharacter::MouseY);

	PlayerInputComponent->BindAction("JumpDodge", IE_Pressed, this, &AHeroCharacter::OnJumpDodge);
	PlayerInputComponent->BindAction("ToggleCrouch", IE_Pressed, this, &AHeroCharacter::ToggleCrouch);
	PlayerInputComponent->BindAction("EquipSword", IE_Pressed, this, &AHeroCharacter::ToggleSword);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AHeroCharacter::Attack);
	PlayerInputComponent->BindAction("Block", IE_Pressed, this, &AHeroCharacter::StartBlocking);
	PlayerInputComponent->BindAction("Block", IE_Released, this, &AHeroCharacter::StopBlocking);
	PlayerInputComponent->BindAction("LockOn", IE_Pressed, this, &AHeroCharacter::OnLock);
}

void AHeroCharacter::Forward(float Value)
{
	if (CanMove)
	{
		FVector ControllerForwardVector = Controller->GetControlRotation().Vector();
		FVector CapsuleForward;
		CapsuleForward.X = ControllerForwardVector.X;
		CapsuleForward.Y = ControllerForwardVector.Y;
		CapsuleForward.Z = 0.0f;
		CapsuleForward.Normalize();

		GetCharacterMovement()->AddInputVector(CapsuleForward * Value);
	}
}

void AHeroCharacter::Turn(float Value)
{
	if (CanMove)
	{
		FVector ControllerForwardVector = Controller->GetControlRotation().Vector();
		FVector ControllerRightVector = ControllerForwardVector.RotateAngleAxis(90.0f, FVector(0.0f, 0.0f, 1.0f));
		FVector CapsuleRight;
		CapsuleRight.X = ControllerRightVector.X;
		CapsuleRight.Y = ControllerRightVector.Y;
		CapsuleRight.Z = 0.0f;
		CapsuleRight.Normalize();

		GetCharacterMovement()->AddInputVector(CapsuleRight * Value);
	}
}

void AHeroCharacter::MouseX(float Value)
{
	AddControllerYawInput(Value);
}

void AHeroCharacter::MouseY(float Value)
{
	AddControllerPitchInput(Value);
}

void AHeroCharacter::OnLock()
{
	if (!IsFightStatus)
		return;

	if (HasLockedPawn)
	{
		UnlockPawn();
	}
	else
	{
		//draw sphere to find enemy in specified radius
		TArray<FHitResult> OutHits;
		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		FVector MyLocation = GetActorLocation();
		GetWorld()->SweepMultiByObjectType(OutHits, MyLocation, MyLocation + FVector(0.0f, 0.0f, 1.0f), //add this vector to work this sweep
			FQuat(), ObjectQueryParams, FCollisionShape::MakeSphere(600.0f), QueryParams);

		AGameCharacter* NearestEnemy = GetTheNearestEnemyFromHitArray(OutHits);
		if (NearestEnemy)
		{
			LockOnPawn(NearestEnemy); //lock on nearest founded enemy
		}
		
	}
}

AGameCharacter* AHeroCharacter::GetTheNearestEnemyFromHitArray(TArray<struct FHitResult> OutHits)
{
	AGameCharacter* NearestEnemy = nullptr;
	for (FHitResult i : OutHits)
	{
		AGameCharacter* GameCharacter = Cast<AGameCharacter>(i.GetActor());
		if (GameCharacter && IsMyEnemy(GameCharacter))
		{
			if (!NearestEnemy)
			{
				NearestEnemy = GameCharacter;
			}
			else
			{
				float DistanceToGameCharacter = (GetActorLocation() - GameCharacter->GetActorLocation()).Size();
				float DistanceToCurrentNearest = (GetActorLocation() - NearestEnemy->GetActorLocation()).Size();

				if (DistanceToGameCharacter < DistanceToCurrentNearest)
				{
					NearestEnemy = GameCharacter;
				}
			}
		}
	}

	return NearestEnemy;
}

void AHeroCharacter::DoLockedOnPawnBehavior()
{
	float Distance = GetDistanceTo(LockedPawn);
	if (Distance > MaxDistanceToLockedOnPawn) //unlock pawn if is too far
	{
		UnlockPawn();
		return;
	}

	Super::DoLockedOnPawnBehavior(); //rotate our character to locked character

	//We will change controller rotation instead of camera rotation, because after unlock camera will be in the same place;
	//if we were rotate camera without change controll rotation after unlock camera will return to current controll rotation

	//We must use camra location because spring arm use controll rotation and rotate to this (so also camera rotate);
	//here is also automatic lerping camera to right position if it is in other when starting lock

	FVector CameraLocationWithDelta = FollowCamera->GetComponentLocation() + FVector(0.0f, 0.0f, LockedOnPawnCameraHeight);

	FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(CameraLocationWithDelta, LockedPawn->GetHeadLocation());

	NewRotation.Pitch = FMath::Clamp<float>(NewRotation.Pitch, LockedCameraMinPichAngle, LockedCameraMaxPichAngle);
					  //calaculat this angle again (first in parent) because when hero is dodging we dont change his rotation
	NewRotation.Yaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LockedPawn->GetHeadLocation()).Yaw; 

	Controller->SetControlRotation(NewRotation);
}

void AHeroCharacter::OnJumpDodge()
{
	if (IsFightStatus || HeroPosture == HP_Crouch)
	{
		OnDodge();
	}
	else
	{
		OnJump();
	}
}

void AHeroCharacter::RotateCharacterForDodge()
{
	if (HasLockedPawn)
	{
		float ForwardValue = InputComponent->GetAxisValue("Forward");
		float RightValue = InputComponent->GetAxisValue("Turn");

		FVector Forward = GetActorForwardVector() * ForwardValue;
		FVector Right = GetActorRightVector() * RightValue;

		FVector Direction = (Forward + Right);
		Direction.Normalize();

		if (Direction.IsNearlyZero())
		{
			Direction = (-GetActorForwardVector()); //if there isnt input, do dodget to back
		}

		SetActorRotation((Direction).Rotation());
	}
	else
	{
		SetActorRotation((GetActorForwardVector()).Rotation());
	}
}

void AHeroCharacter::Attack()
{
	Super::Attack(); //attack with sword

	if (HeroPosture == HP_Crouch && !IsKnifeAttack()) //knife attack
	{
		TArray<AActor*> OverlappigActors;
		FrontBoxCollision->GetOverlappingActors(OverlappigActors);
		
		ANPCCharacter* KnifeAttackedCharacter = nullptr; //looking for NPC which meet conditions
		for (AActor* i : OverlappigActors)
		{
			ANPCCharacter* NPC = Cast<ANPCCharacter>(i);
			if (NPC)
			{
				if (MeetsConditionsForKnifeAttack(NPC))
				{
					KnifeAttackedCharacter = NPC;
					break;
				}
			}
		}

		if (KnifeAttackedCharacter)
		{
			EquipeKnife();
		}
	}
}

bool AHeroCharacter::IsKnifeAttack()
{
	return (GetMesh()->GetAnimInstance()->Montage_IsPlaying(EquipeKnifeAnimMontage) ||
			GetMesh()->GetAnimInstance()->Montage_IsPlaying(KnifeAttackAnimMontage));
}

void AHeroCharacter::MakeKnifeAttack()
{
	PlayAnimMontage(KnifeAttackAnimMontage);
}

void AHeroCharacter::EquipeKnife()
{
	PlayAnimMontage(EquipeKnifeAnimMontage);
	float EquipeTime = EquipeKnifeAnimMontage->GetPlayLength() - 0.5f;
	GetWorld()->GetTimerManager().SetTimer(AttachingSwordTimer, this, &AHeroCharacter::MakeKnifeAttack, EquipeTime, false);
}

void AHeroCharacter::AttachKnife()
{
	FAttachmentTransformRules Rules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, false);

	if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(EquipeKnifeAnimMontage)) //beform perform attack
	{
		Knife->AttachToComponent(HandKnifeLocation, Rules);
	}
	else //after perform attack
	{
		Knife->AttachToComponent(WaistKnifeLocation, Rules);
	}

	//lerping knife to new target
	FLatentActionInfo Info;
	Info.CallbackTarget = Knife;
	UKismetSystemLibrary::MoveComponentTo(Knife, FVector(0, 0, 0), FRotator(0, 0, 0), true, true,
		AttachingSwordLerpTime, true, EMoveComponentAction::Move, Info);
}

bool AHeroCharacter::MeetsConditionsForKnifeAttack(ANPCCharacter* Character)
{
	if (Character)
	{
		return (IsMyEnemy(Character) && !Character->IsInAlert());
	}
	return false;
}

void AHeroCharacter::OnKnifeBeginOverlapp(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ANPCCharacter* Character = Cast<ANPCCharacter>(OtherActor);
	if (Character && OtherActor != this)
	{
		if (!Character->IsInAlert())
		{
			Character->Death();
		}
	}
}

void AHeroCharacter::SetKnifeCollisionEnabled(bool Enabled)
{
	if (Enabled)
	{
		Knife->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	else
	{
		Knife->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}
