// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HeroAnimInterface.generated.h"

/**
Interface for heros animation blueprint; it is necessery because we must have access to this bp from c++ to change his
animation from stand/crouch (instead of this we can check every frame if it was changed, but is not good for performance)
*/

UINTERFACE(BlueprintType)
class THIEFPROJECT_API UHeroAnimInterface : public UInterface
{
	GENERATED_BODY()
};

class THIEFPROJECT_API IHeroAnimInterface
{
	GENERATED_BODY()

public:

	/**Send info about toggle crouch to heros anim bp*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HeroAnimInterface")
		void ToggleCrouch(bool NewToggle);

	/**Send info about change figt pose of character*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HeroAnimInterface")
		void ToggleFightStatus(bool NewToggle);

};