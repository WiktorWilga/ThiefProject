// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameCharacter.h"
#include "HeroCharacter.generated.h"

/**
Class for plyable hero; all logic for movement
*/

UCLASS()
class THIEFPROJECT_API AHeroCharacter : public AGameCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHeroCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**Spring arm for basic camer*/
	UPROPERTY(VisibleAnywhere, Category = "HeroCharacter")
		class USpringArmComponent* CameraSpringArm;

	/**Basic camera*/
	UPROPERTY(VisibleAnywhere, Category = "HeroCharacter")
		class UCameraComponent* FollowCamera;

	/**How hight should be camera as default when our hero is locked on other character*/
	UPROPERTY(EditDefaultsOnly, Category = "HeroCharacter")
		float LockedOnPawnCameraHeight = 250.0f;

	/**How much can camera rotate in Y axis our hero locked on other character*/
	UPROPERTY(EditDefaultsOnly, Category = "HeroCharacter")
		float LockedCameraMinPichAngle = -30.0f;

	/**How much can camera rotate in Y axis our hero locked on other character*/
	UPROPERTY(EditDefaultsOnly, Category = "HeroCharacter")
		float LockedCameraMaxPichAngle = -15.0f;

	/**How far must be locked character to our her to break locked on pawn*/
	UPROPERTY(EditDefaultsOnly, Category = "HeroCharacter")
		float MaxDistanceToLockedOnPawn = 1000.0f;

	/**Animation Montage for equipe knife*/
	UPROPERTY(EditDefaultsOnly, Category = "HeroCharacter")
		class UAnimMontage* EquipeKnifeAnimMontage;

	/**Animation Montage for knife with attack*/
	UPROPERTY(EditDefaultsOnly, Category = "HeroCharacter")
		class UAnimMontage* KnifeAttackAnimMontage;

	/**Timer to wait when hero equipe knife to attack with it*/
	FTimerHandle WaitForEquipeKnifeTimer;

	/**Static mesh for knife (used when attacking in crouch)*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HeroCharacter")
		class UStaticMeshComponent* Knife;

	/**To this component is attaching knife when player isnt use it*/
	UPROPERTY(VisibleAnywhere, Category = "HeroCharacter")
		class USceneComponent* WaistKnifeLocation;

	/**To this component is attaching knife when player use it*/
	UPROPERTY(VisibleAnywhere, Category = "HeroCharacter")
		class USceneComponent* HandKnifeLocation;

	/**Box collision for check if befor hero is any enemy*/
	UPROPERTY(VisibleAnywhere, Category = "HeroCharacter")
		class UBoxComponent* FrontBoxCollision;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/**Player movement*/
	UFUNCTION()
		void Forward(float value);

	/**Player movement*/
	UFUNCTION()
		void Turn(float value);

	/**Player camera rotating and turning*/
	UFUNCTION()
		void MouseX(float value);

	/**Player camera rotating*/
	UFUNCTION()
		void MouseY(float value);

	/**Sarch enemy pawn near hero and lock on it or unlock hero already locked someone*/
	UFUNCTION()
		void OnLock();

	/**Return the nearset enemy from given hit result array, if there isnt any enymy return nullptr*/
	class AGameCharacter* GetTheNearestEnemyFromHitArray(TArray<struct FHitResult> OutHits);

	/**Function to update rotation of hero to look at locked pawn and update controller rotation (so also spring arm rotation)
	to be directed on locked pawn, but not allow to rotate too much.
	WARNING: this function is called in Tick of parent class, so cant call it in this Tick*/
	virtual void DoLockedOnPawnBehavior() override;

	/**Input for jump and dodge*/
	UFUNCTION()
		void OnJumpDodge();

	/**Rotation hero for dodge should be advanced. It provide special behavior when hero is locked on other character*/
	virtual void RotateCharacterForDodge() override;

	/**Called when player is striking with a sword; extend functionality to knife attack*/
	virtual void Attack() override;

	/**True if is making knife attack at this moment; it include check if is equipe knife or knife attack playing*/
	bool IsKnifeAttack();

	/**Make an attack with knife after equipe it; play animation*/
	UFUNCTION()
	void MakeKnifeAttack();

	/**Equipe knife, play animation and start timer for MakeKnifeAttack*/
	UFUNCTION()
		void EquipeKnife();

	/**Attach knife to waist/hand depended on current anim. Called in Animation blueprint*/
	UFUNCTION(BlueprintCallable, Category = "HeroCharacter")
		void AttachKnife();

	/**True if npc didnt see player and is player enemy*/
	bool MeetsConditionsForKnifeAttack(class ANPCCharacter* Character);

	/**Called when knife begin overlapp*/
	UFUNCTION()
		void OnKnifeBeginOverlapp(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/**Enable/disable knife collision; it is using while character is attacking to deteck collision only in right moment.
	Called in animation blueprint*/
	UFUNCTION(BlueprintCallable, Category = "HeroCharacter")
		void SetKnifeCollisionEnabled(bool Enabled);

};
