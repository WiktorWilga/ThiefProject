// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "HeroAnimInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"
#include "GameFramework/Controller.h"
#include "Engine/Engine.h"

// Sets default values
AGameCharacter::AGameCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BackSwordLocation = CreateDefaultSubobject<USceneComponent>(TEXT("BackSwordLocation"));
	BackSwordLocation->SetupAttachment(GetMesh(), "Spine2Socket");

	HandSwordLocation = CreateDefaultSubobject<USceneComponent>(TEXT("HandSwordLocation"));
	HandSwordLocation->SetupAttachment(GetMesh(), "RightHandIndex1Socket");

	Sword = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sword"));
	Sword->SetupAttachment(BackSwordLocation);

	NoiseEmitterComp = CreateDefaultSubobject<UPawnNoiseEmitterComponent>("SenseNoiseEmitter");

}

// Called when the game starts or when spawned
void AGameCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	Sword->OnComponentBeginOverlap.AddDynamic(this, &AGameCharacter::OnWeaponBeginOverlapp); //deteck collision when attacking

	//for events after finish any anim monatge
	/*FScriptDelegate ScriptDelegate;
	ScriptDelegate.BindUFunction(this, "OnAnimMontegeFinished");
	GetMesh()->GetAnimInstance()->OnMontageEnded.Add(ScriptDelegate);*/
}

// Called every frame
void AGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasLockedPawn && !IsDead())
	{
		DoLockedOnPawnBehavior();
	}
	if (IsDodging)
	{
		UpdateDodgeMovement(DeltaTime);
	}
}

// Called to bind functionality to input
void AGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AGameCharacter::OnJump()
{
	if (!IsAttacking())
	{
		Jump(); //standard action from ACharacter

		PlayAnimMontage(JumpAnimMontage);
	}
}

void AGameCharacter::Landed(const FHitResult & Hit)
{
	PlayAnimMontage(JumpAnimMontage, 1.0f, "end_loop");
}

void AGameCharacter::ToggleCrouch()
{
	if (HeroPosture == HP_Stand)
	{
		if (UsingSword)
		{
			ToggleSword();
		}

		HeroPosture = HP_Crouch;
		ActiveCrouchWalkSpeed();
	}
	else
	{
		HeroPosture = HP_Stand;
		if (IsFightStatus)
		{
			ActiveFightWalkSpeed();
		}
		else
		{
			ActiveStandWalkSpeed();
		}
	}

	//change stand/crouch animation
	IHeroAnimInterface::Execute_ToggleCrouch(GetMesh()->GetAnimInstance(), (HeroPosture == HP_Crouch));
}

bool AGameCharacter::IsCrouching()
{
	return (HeroPosture == HP_Crouch);
}

void AGameCharacter::ActiveStandWalkSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void AGameCharacter::ActiveCrouchWalkSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = MaxCrouchSpeed;
}

void AGameCharacter::ActiveFightWalkSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = MaxFightSpeed;
}

void AGameCharacter::SetCharacterMaxWalkSpeed(float NewSpeed)
{
	MaxWalkSpeed = NewSpeed;

	if (HeroPosture == HP_Stand) //to refresh speed if now is standing
	{
		GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	}
}

void AGameCharacter::SetCharacterMaxCrouchSpeed(float NewSpeed)
{
	MaxCrouchSpeed = NewSpeed;

	if (HeroPosture == HP_Stand) //to refresh speed if now is crouching
	{
		GetCharacterMovement()->MaxWalkSpeed = MaxCrouchSpeed;
	}
}

void AGameCharacter::SetCharacterMaxFightSpeed(float NewSpeed)
{
	MaxFightSpeed = NewSpeed;

	if (IsFightStatus && HeroPosture == HP_Stand) //refresh speed but not if crouching because in this situation speed can be lower
	{											  //than fight speed
		GetCharacterMovement()->MaxWalkSpeed = MaxFightSpeed;
	}
}

void AGameCharacter::ToggleSword()
{
	if (HeroPosture == HP_Crouch)
	{
		ToggleCrouch();
	}

	PlayAnimMontage(EqupSwordAnimMontage);

	UsingSword = !UsingSword;
	SetFightStatus(UsingSword);

	float WaitTime = EqupSwordAnimMontage->Notifies[0].GetTriggerTime(); //get notify time to start attach
	GetWorld()->GetTimerManager().SetTimer(AttachingSwordTimer, this, &AGameCharacter::AttachSword, WaitTime, false);
}

void AGameCharacter::AttachSword()
{
	FAttachmentTransformRules Rules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, false);

	if (UsingSword)
	{
		Sword->AttachToComponent(HandSwordLocation, Rules);
	}
	else
	{
		Sword->AttachToComponent(BackSwordLocation, Rules);
	}

	//lerping sword to new target
	FLatentActionInfo Info;
	Info.CallbackTarget = Sword;
	UKismetSystemLibrary::MoveComponentTo(Sword, FVector(0, 0, 0), FRotator(0, 0, 0), true, true,
		AttachingSwordLerpTime, true, EMoveComponentAction::Move, Info);
}

void AGameCharacter::Attack()
{
	if (UsingSword && !GetCharacterMovement()->IsFalling())
	{
		if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(AttacksAnimMontage[AttackCount])) //combo
		{

			if (IsAttackInComboTime() && !ComboFailed) //new attack for combo successed
			{
				++AttackCount;
				AttackCount %= AttacksAnimMontage.Num();

				PlayAnimMontage(AttacksAnimMontage[AttackCount]);

				//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "OK");
			}
			else //combo failed
			{
				ComboFailed = true;
				//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "FAILED");
			}
		}
		else //start new attack
		{
			AttackCount = 0;
			PlayAnimMontage(AttacksAnimMontage[AttackCount]);

			ComboFailed = false;
		}
	}
}

bool AGameCharacter::IsAttackInComboTime()
{
	float StartComboTime = AttacksAnimMontage[AttackCount]->GetPlayLength(); //as default set end time of anim because of some attacks
																			 //dont have Combo notify and shouldnt allow combo
	for (FAnimNotifyEvent Notify : AttacksAnimMontage[AttackCount]->Notifies) //find combo notify
	{
		if (Notify.GetNotifyEventName().IsEqual(ComboAnimNotify))
		{
			StartComboTime = Notify.GetTriggerTime(); //and set time which allow combo
		}
	}

	float CurrentAnimTime = GetMesh()->GetAnimInstance()->Montage_GetPosition(AttacksAnimMontage[AttackCount]);

	return (CurrentAnimTime >= StartComboTime);

}

void AGameCharacter::SetCanMove(bool NewActive)
{
	CanMove = NewActive;
}

void AGameCharacter::StartBlocking()
{
	if (UsingSword && !IsAttacking() && !IsDodging)
	{
		PlayAnimMontage(BlockngAnimMontage);

		IsBlocking = true;
	}
}

void AGameCharacter::StopBlocking()
{
	StopAnimMontage(BlockngAnimMontage);

	IsBlocking = false;
}

bool AGameCharacter::IsAttacking()
{
	return (GetMesh()->GetAnimInstance()->Montage_IsPlaying(AttacksAnimMontage[AttackCount]));
}

bool AGameCharacter::IsMyTeamEnemyNumber(int32 TeamNum)
{
	return !(MyTeamNumber == TeamNum);
}

bool AGameCharacter::IsMyEnemy(AGameCharacter* CharacterRef)
{
	return !(MyTeamNumber == CharacterRef->GetMyTeamNumber());
}

int32 AGameCharacter::GetMyTeamNumber()
{
	return MyTeamNumber;
}

void AGameCharacter::MakeStepsNoise()
{
	MakeNoise(StepsLoudness, this, GetActorLocation());
}

void AGameCharacter::SetFightStatus(bool NewStatus)
{
	IsFightStatus = NewStatus;

	if (IsFightStatus)
	{
		if (HeroPosture == HP_Stand)
		{
			ActiveFightWalkSpeed();
		}
	}
	else
	{
		if (HeroPosture == HP_Stand)
		{
			ActiveStandWalkSpeed();
		}
		else
		{
			ActiveCrouchWalkSpeed();
		}

		UnlockPawn();
	}

	//change animation
	IHeroAnimInterface::Execute_ToggleFightStatus(GetMesh()->GetAnimInstance(), IsFightStatus);
}

void AGameCharacter::LockOnPawn(AGameCharacter * LockPawn)
{
	if (LockPawn && !LockPawn->IsDead())
	{
		if (LockedPawn) //unbind ours function when we changing target
		{
			LockedPawn->OnDeathDelegate.Remove(TargetCharacterDelegateHandle);
			TargetCharacterDelegateHandle.Reset();
		}

		HasLockedPawn = true;
		LockedPawn = LockPawn;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		TargetCharacterDelegateHandle = LockedPawn->OnDeathDelegate.AddUFunction(this, "OnTargetDead");
	}
}

void AGameCharacter::UnlockPawn()
{
	HasLockedPawn = false;

	LockedPawn = nullptr;

	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AGameCharacter::DoLockedOnPawnBehavior()
{
	if (!IsDodging && !IsAttacking())
	{
		FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LockedPawn->GetActorLocation());

		FRotator InterpRotation = UKismetMathLibrary::RInterpTo(GetActorRotation(), NewRotation, GetWorld()->DeltaTimeSeconds, 4.0f);
		InterpRotation.Roll = GetActorRotation().Roll;
		InterpRotation.Pitch = GetActorRotation().Pitch;

		SetActorRotation(InterpRotation);
	}
}

FVector AGameCharacter::GetHeadLocation()
{
	FVector Value = GetActorLocation();
	Value.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	return Value;
}

void AGameCharacter::SetWeaponCollisionEnabled(bool Enabled)
{
	if (Enabled)
	{
		Sword->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	else
	{
		Sword->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AGameCharacter::OnWeaponBeginOverlapp(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
										   int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AGameCharacter* Character = Cast<AGameCharacter>(OtherActor);
	if (Character && Character != this && IsMyEnemy(Character) && !Character->IsDead())
	{
		Character->OnHitedByWeapon(this, 20.0f);
	}
}

void AGameCharacter::OnHitedByWeapon(AActor* InstigatorActor, float Damage)
{
	if (IsDodging) //if we are dodging not do anything
	{
		return;
	}

	if (IsBlocking && IsInBlockingAngle(InstigatorActor)) //block successed
	{
		PlayAnimMontage(BlockedAtackReactionAnimMontage);
	}
	else //not blocking or block failed
	{
		PlayAnimMontage(TakeDamageAnimMontage);
		AddDamage(Damage);
	}

	//charcter is pushed away
	FVector ImpulseDirection = GetActorLocation() - InstigatorActor->GetActorLocation();
	ImpulseDirection.Normalize();
	ImpulseDirection *= TakeDamagePushedAwayStrength;
	//GetCharacterMovement()->StopActiveMovement(); //maybe needed, bacuse of behavior tree
	GetCharacterMovement()->AddImpulse(ImpulseDirection, true);

	SetCanMove(true); //needed because if we started attack, but not finish them (because of take damage) we dont return movement 
}

bool AGameCharacter::IsUsingSword()
{
	return UsingSword;
}

void AGameCharacter::AddAttackForwardImpulse()
{
	FVector ImpulseDirection = GetActorForwardVector();
	ImpulseDirection *= MakeAttckForwardImpulseStrength;
	//GetCharacterMovement()->StopActiveMovement(); //maybe needed, bacuse of behavior tree
	GetCharacterMovement()->AddImpulse(ImpulseDirection, true);
}

bool AGameCharacter::IsInBlockingAngle(AActor * AttackingActor)
{
	//make rotators for Yaw axis
	FRotator FaceToFaceRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), AttackingActor->GetActorLocation());
	FaceToFaceRotation.Roll = FaceToFaceRotation.Pitch = 0.0f;
	FRotator CurrentRotation = GetActorRotation();
	CurrentRotation.Roll = CurrentRotation.Pitch = 0.0f;

	//forward vectors for rotators
	FVector FaceVector = FaceToFaceRotation.Vector();
	FVector CurrentVector = CurrentRotation.Vector();

	//angle between both vectors
	float Angle = FMath::RadiansToDegrees(acos(FVector::DotProduct(FaceVector, CurrentVector)));

	return (Angle <= BlockingAngle);
}

void AGameCharacter::UpdateBlockingAnim()
{
	if (IsBlocking && !GetMesh()->GetAnimInstance()->Montage_IsPlaying(BlockngAnimMontage))
	{
		PlayAnimMontage(BlockngAnimMontage);
	}
	
}

//for events after finish any anim monatge
/*void AGameCharacter::OnAnimMontegeFinished(UAnimMontage* Montage, bool Interrupted)
{
	if (Montage == BlockedAtackReactionAnimMontage || Montage == TakeDamageAnimMontage)
	{
		UpdateBlockingAnim();
	}
}*/

void AGameCharacter::OnDodge()
{
	if (!IsDodging)
	{
		RotateCharacterForDodge();

		PlayAnimMontage(DodgeAnimMontage, DodgeRate);

		IsDodging = true;

		float DodgeTime = DodgeAnimMontage->GetPlayLength() / DodgeRate;
		GetWorld()->GetTimerManager().SetTimer(ResetIsDodgingTimer, this, &AGameCharacter::ResetIsDodging, DodgeTime, false);
	}
}

void AGameCharacter::ResetIsDodging()
{
	IsDodging = false;
}

void AGameCharacter::UpdateDodgeMovement(float DeltaTime)
{
	FVector Direction = GetActorForwardVector() * DeltaTime * DodgeMoveSpeed;
	GetCharacterMovement()->AddInputVector(Direction);
}

void AGameCharacter::RotateCharacterForDodge()
{
	SetActorRotation((-GetActorForwardVector()).Rotation());
}

void AGameCharacter::AddDamage(float Damage)
{
	CurrentHealth -= Damage;

	if (CurrentHealth <= 0.0f)
	{
		Death();
	}
}

void AGameCharacter::Death()
{
	CurrentHealth = 0.0f;

	PlayAnimMontage(DeathAnimMontage);

	Controller->UnPossess();

	if (OnDeathDelegate.IsBound())
	{
		OnDeathDelegate.Broadcast(this);
	}

	if (LockedPawn) //unbind ours function from target character
	{
		LockedPawn->OnDeathDelegate.Remove(TargetCharacterDelegateHandle);
		TargetCharacterDelegateHandle.Reset();
	}

	float DeathAnimTime = 1.0f;
	GetWorld()->GetTimerManager().SetTimer(ResetIsDodgingTimer, this, &AGameCharacter::PostDeath, DeathAnimTime, false);
}

bool AGameCharacter::IsDead()
{
	return (CurrentHealth <= 0.0f);
}

void AGameCharacter::OnTargetDead(AActor * DeathActor)
{
	if (LockedPawn == DeathActor)
	{
		LockedPawn->OnDeathDelegate.Remove(TargetCharacterDelegateHandle); //unbind ours function from target character
		TargetCharacterDelegateHandle.Reset();

		UnlockPawn();
	}
}

void AGameCharacter::PostDeath()
{
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Sword->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	Sword->SetSimulatePhysics(true);
	Sword->SetCollisionProfileName("BlockAll");
}
