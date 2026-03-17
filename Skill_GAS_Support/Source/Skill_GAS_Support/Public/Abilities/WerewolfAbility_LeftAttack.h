// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Werewolf_Ability.h"
#include "WerewolfAbility_LeftAttack.generated.h"

/**
 * 
 */
UCLASS()
class WESTERN_MAP_API UWerewolfAbility_LeftAttack : public UWerewolf_Ability
{
	GENERATED_BODY()

	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cooldown")
	FScalableFloat CooldownDuration_01;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cooldown")
	FScalableFloat CooldownDuration_02;

	UPROPERTY(BlueprintReadWrite,Category="Cooldown")
	bool AttackOrder = true;
	


	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	
};
