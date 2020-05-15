// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameCharacter.generated.h"

/**
Class for every character in game. From this class should inherite every playable characters and every NPC.
Here is logic for movement (but not input), fighting, toggling crouch, equipe sword, jump.
*/

DECLARE_MULTICAST_DELEGATE_OneParam(EOnDeathDelegate, AActor*);

enum EHeroPosture { HP_Stand, HP_Crouch };

UCLASS()
class THIEFPROJECT_API AGameCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGameCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**To this component is attaching sword when player isnt use it*/
	UPROPERTY(VisibleAnywhere, Category = "GameCharacter")
		class USceneComponent* BackSwordLocation;

	/**To this component is attaching sword when player is useing it*/
	UPROPERTY(VisibleAnywhere, Category = "GameCharacter")
		class USceneComponent* HandSwordLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameCharacter")
		class UStaticMeshComponent* Sword;

	/**We must play animation for jump from c++, because of we have this one file (instead of three: start, loop, end)*/
	UPROPERTY(EditDefaultsOnly, Category = "GameCharacter")
		class UAnimMontage* JumpAnimMontage;

	/**Animation Montage for take out and put the sword down*/
	UPROPERTY(EditDefaultsOnly, Category = "GameCharacter")
		class UAnimMontage* EqupSwordAnimMontage;

	/**Animation Montage for attacks*/
	UPROPERTY(EditDefaultsOnly, Category = "GameCharacter")
		TArray<class UAnimMontage*> AttacksAnimMontage;

	/**Animation Montage for blocking with a sword*/
	UPROPERTY(EditDefaultsOnly, Category = "GameCharacter")
		class UAnimMontage* BlockngAnimMontage;

	/**Animation Montage for taking damagy of weapon*/
	UPROPERTY(EditDefaultsOnly, Category = "GameCharacter")
		class UAnimMontage* TakeDamageAnimMontage;

	/**Animation Montage for react of blocked atack*/
	UPROPERTY(EditDefaultsOnly, Category = "GameCharacter")
		class UAnimMontage* BlockedAtackReactionAnimMontage;

	/**Animation Montage for dodge*/
	UPROPERTY(EditDefaultsOnly, Category = "GameCharacter")
		class UAnimMontage* DodgeAnimMontage;

	/**Animation Montage for death*/
	UPROPERTY(EditDefaultsOnly, Category = "GameCharacter")
		class UAnimMontage* DeathAnimMontage;

	/**Current Hero posture*/
	EHeroPosture HeroPosture = HP_Stand;

	/**Dont use speed values from character movement, because of his Crouch function doesnt work*/
	UPROPERTY(EditDefaultsOnly, Category = "GameCharacter")
		float MaxWalkSpeed = 600.0f;

	/**Dont use speed values from character movement, because of his Crouch function doesnt work*/
	UPROPERTY(EditDefaultsOnly, Category = "GameCharacter")
		float MaxCrouchSpeed = 300.0f;

	/**Dont use speed values from character movement, because of his Crouch function doesnt work*/
	UPROPERTY(EditDefaultsOnly, Category = "GameCharacter")
		float MaxFightSpeed = 400.0f;

	/**How much time must lerping sword while attaching it to back/hand*/
	UPROPERTY(EditDefaultsOnly, Category = "GameCharacter")
		float AttachingSwordLerpTime = 0.3f;

	/**Name of notify in attacks anims after which character can make combo*/
	UPROPERTY(EditDefaultsOnly, Category = "GameCharacter")
		FName ComboAnimNotify;

	/**True if player is holding sword in hand; false - sword on back*/
	bool UsingSword = false;

	FTimerHandle AttachingSwordTimer;

	/**Current attack (index for AttacksAnimMontage) array*/
	int32 AttackCount = 0;

	/**True if player failed combo and attack should return to first attack; its happen when player press attack key
	too early or to late*/
	bool ComboFailed = false;

	/**True if player can move; false - movement is diabled (e.g. when player is attacking)*/
	bool CanMove = true;

	/**True if player is blocking with a sword*/
	bool IsBlocking = false;

	/**Number of tem in which is this character. If character are in other teams they are enemys*/
	UPROPERTY(EditAnywhere, Category = "GameCharacter")
		int32 MyTeamNumber = 0;

	/**Loudnes for character steps, when he is walking/running (only for NPC Perception)*/
	UPROPERTY(EditAnywhere, Category = "GameCharacter")
		float StepsLoudness = 1.0f;

	/**From how far steps can be hearing for NPC perception*/
	UPROPERTY(EditAnywhere, Category = "GameCharacter")
		float StepsSoundMaxRange = 600.0f;

	/**To make noise data used by the pawn sensing component (not reall sounds)*/
	class UPawnNoiseEmitterComponent* NoiseEmitterComp;

	/**True if character is using weapon*/
	bool IsFightStatus = false;

	/**Pawn on wich our character is locked - rotae always to this pwan*/
	AGameCharacter* LockedPawn = nullptr;

	/**True if our character locked some pawn*/
	bool HasLockedPawn = false;

	/**How much our characte is pushed w=away when he was hited by weapon*/
	UPROPERTY(EditDefaultsOnly, Category = "GameCharacter")
		float TakeDamagePushedAwayStrength = 1000.0f;

	/**How much our characte is move forward when he is make attack*/
	UPROPERTY(EditDefaultsOnly, Category = "GameCharacter")
		float MakeAttckForwardImpulseStrength = 1000.0f;

	/**How much can our character be rotated terms of face to face to attacking actor to block be successed*/
	UPROPERTY(EditDefaultsOnly, Category = "GameCharacter")
		float BlockingAngle = 20.0f;

	/**True if character is now dodging*/
	bool IsDodging = false;

	/**Timer to reset is dodging variable, when character finished dodging*/
	FTimerHandle ResetIsDodgingTimer;

	/**Dodge aniamtion rate, if this value is higgher lenght of dodge will be sorther*/
	UPROPERTY(EditDefaultsOnly, Category = "GameCharacter")
		float DodgeRate = 1.5f;

	/**How fast is moving character when he is dodging*/
	UPROPERTY(EditDefaultsOnly, Category = "GameCharacter")
		float DodgeMoveSpeed = 100.0f;

	/**How much can character has health*/
	UPROPERTY(EditDefaultsOnly, Category = "GameCharacter")
		float MaxHealth = 100.0f;

	/**How much health has character in this moment*/
	float CurrentHealth = MaxHealth;

	/**Delegate handle for current target character; used to remove itself from other character delegate when we are changing target*/
	FDelegateHandle TargetCharacterDelegateHandle;

	/**Used in Death function to run after specified period of time PostDeath function*/
	FTimerHandle PostDeathTimer;

public:	

	/**Delegate to bind functions which should be called when our character is dead. Eg for other character who has locked on ours*/
	EOnDeathDelegate OnDeathDelegate;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/**Jump action, start animation*/
	UFUNCTION()
		void OnJump();

	/**Change jump animation*/
	UFUNCTION()
		void Landed(const FHitResult& Hit) override;

	/**Change player posture, crouch/stand*/
	UFUNCTION()
		void ToggleCrouch();

	/**Getter for character posture*/
	UFUNCTION(BlueprintCallable, Category = "GameCharacter")
		bool IsCrouching();

	/**Active character walk/run speed; when he is standing*/
	UFUNCTION()
		void ActiveStandWalkSpeed();

	/**Active character crouch speed; when he is crouching*/
	UFUNCTION()
		void ActiveCrouchWalkSpeed();

	/**Active character fight speed; when he is holding weapon*/
	UFUNCTION()
		void ActiveFightWalkSpeed();

	/**Set max character walk/run speed; use it instead of unreals function because of problems with crouching*/
	UFUNCTION(BlueprintCallable, Category = "GameCharacter")
		void SetCharacterMaxWalkSpeed(float NewSpeed);

	/**Set max character crouch speed; use it instead of unreals function because of problems with crouching*/
	UFUNCTION(BlueprintCallable, Category = "GameCharacter")
		void SetCharacterMaxCrouchSpeed(float NewSpeed);

	/**Set max character speed when he is holding weapon; use it instead of unreals function because of problems with crouching*/
	UFUNCTION(BlueprintCallable, Category = "GameCharacter")
		void SetCharacterMaxFightSpeed(float NewSpeed);

	/**Take out and put the sword down*/
	UFUNCTION()
		void ToggleSword();

	/**Attach sword to right componenet (back/hand), and lerp to this location*/
	void AttachSword();

	/**Called when player is striking with a sword*/
	UFUNCTION()
		virtual void Attack();

	/**Check if current attack is in time which allow do next attack for combo*/
	bool IsAttackInComboTime();

	/**Setter for CanMove variable*/
	UFUNCTION(BlueprintCallable, Category = "GameCharacter")
		void SetCanMove(bool NewActive);

	/**Called when player started blocking with a sword*/
	UFUNCTION()
		void StartBlocking();

	/**Called when player stopped blocking with a sword*/
	UFUNCTION()
		void StopBlocking();

	/**True if player is attacking with a sword*/
	UFUNCTION()
		bool IsAttacking();

	/**True if given number of team is enemy team*/
	UFUNCTION()
		bool IsMyTeamEnemyNumber(int32 TeamNum);

	/**True if given character is enemy*/
	UFUNCTION()
		bool IsMyEnemy(AGameCharacter* CharacterRef);

	/**Getter for MyTeamNumber*/
	int32 GetMyTeamNumber();

	/**Create 'noise' for NPC Perception Hearing. It doesnt play any sound; only for perception.*/
	UFUNCTION(BlueprintCallable, Category = "GameCharacter")
		void MakeStepsNoise();

	/**Setter for IsFightStatus and send info to animation blueprint; called when toggle weapon*/
	void SetFightStatus(bool NewStatus);

	/**Lock charactar on another pawn; then  our character is always rote face to locked pawn*/
	void LockOnPawn(AGameCharacter* LockPawn);

	/**Call to unlock our character for looking always on some actual locked pawn*/
	void UnlockPawn();

	/**This function is called in tick to do right actions when character lock on other pawn. It can be override to 
	provide specail behavior for child class. This version only rotate our character to locked pawn.
	WARNING: this function is called in this Tick, so autamaticlly will be called in child class Tick*/
	virtual void DoLockedOnPawnBehavior();

	/**Return the highest point of capsule. It is approximation of head location.*/
	FVector GetHeadLocation();

	/**Enable/disable weapon collision; it is using while character is attacking to deteck collision only in right moment.
	Called in animation blueprint*/
	UFUNCTION(BlueprintCallable, Category = "GameCharacter")
	void SetWeaponCollisionEnabled(bool Enabled);

	/**Called when weapon begin overlapp*/
	UFUNCTION()
	void OnWeaponBeginOverlapp(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
							   int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/**Called when character was hited with a weapon*/
	void OnHitedByWeapon(AActor* InstigatorActor, float Damage);

	/**Getter for is using sword value*/
	bool IsUsingSword();

	/**Move character forward when he is make attack. Called in animation blueprint, when attack anim reach right notify*/
	UFUNCTION(BlueprintCallable, Category = "GameCharacter")
	void AddAttackForwardImpulse();

	/**True if AttackingActor is in area where our character can block his attack. For examble it should return false when
	our characters back is to AttackingActor, but true if is face to face. It uses BlockingAngle variable. */
	bool IsInBlockingAngle(class AActor* AttackingActor);

	/**Start blocking animation if character is blocking. It is useful when we break blocking animation eg by attack blocked 
	reaction animation and character back to default anim not to blocking anim, but relly he is still blocking.
	Called in anim blueprint.*/
	UFUNCTION(BlueprintCallable, Category = "GameCharacter")
	void UpdateBlockingAnim();

	/**Function called when finished (or interrupted) any anim montage. Used some anim motages when were finished to 
	update blocking anim
	For events after finish any anim monatge*/
	//UFUNCTION()
	//	void OnAnimMontegeFinished(class UAnimMontage* Montage, bool Interrupted);

	/**Start dodge if character now is not dodging*/
	UFUNCTION()
		void OnDodge();

	/**Set IsDodging variable to false. Called by timer when dodging finished*/
	UFUNCTION()
		void ResetIsDodging();

	/**Move character when he is dodging.*/
	void UpdateDodgeMovement(float DeltaTime);

	/**Rotate character to right side, in which he will be dodging. Basicly it always rotate to back, but it can be override
	in child class, and change behavior*/
	virtual void RotateCharacterForDodge();

	/**Add damage to character. This function should depends on characters armor etc; also it check if character should dead.*/
	void AddDamage(float Damage);

	/**Called when character died. It play right montage, unposses character and set simulate physics*/
	void Death();

	/**True if is allready dead*/
	bool IsDead();

	/**Called by target character when he is dead. It unlock pawn and reset Taget value for behavior tree*/
	UFUNCTION()
	virtual void OnTargetDead(AActor* DeathActor);

	/**Called after death; set simulate physics, collision etc*/
	UFUNCTION()
		void PostDeath();

};
