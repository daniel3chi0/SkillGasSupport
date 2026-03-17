// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Werewolf_Ability.h"
#include "WerewolfAbility_AIDead.generated.h"

/**
 * 
 */
UCLASS()
class WESTERN_MAP_API UWerewolfAbility_AIDead : public UWerewolf_Ability
{
	GENERATED_BODY()
	
	
public:
	UFUNCTION(BlueprintCallable)
	void GeneralStop();
	
	UFUNCTION(BlueprintImplementableEvent)
	void K2_PreMove();

	
	UFUNCTION(BlueprintImplementableEvent)
	void K2_PostMove();

	UPROPERTY(EditAnywhere,Category = "Time")
	float PreMoveDelayTime = 0.0f;

	
	UPROPERTY(EditAnywhere,Category = "Time")
	float PostMoveDelayTime = 0.0f;
protected:
	UFUNCTION()
	void MoveToCemetery(); // 延迟后执行的逻辑
	UFUNCTION()
	void PostMove();
};
