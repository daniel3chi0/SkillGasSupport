// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Werewolf_Ability.h"
#include "Werewolf_PassiveAbility.generated.h"

/**
 * 
 */
UCLASS()
class WESTERN_MAP_API UWerewolf_PassiveAbility : public UWerewolf_Ability
{
	GENERATED_BODY()
	
	
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	UPROPERTY(EditAnywhere)
	bool bActivateAbilityOnGranted = true;
};
