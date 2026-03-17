// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGSAbility.h"
#include "SGSPassiveAbility.generated.h"

/**
 * 
 */
UCLASS()
class SKILL_GAS_SUPPORT_API USGSPassiveAbility : public USGSAbility
{
	GENERATED_BODY()
	
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	UPROPERTY(EditAnywhere)
	bool bActivateAbilityOnGranted = true;
};
