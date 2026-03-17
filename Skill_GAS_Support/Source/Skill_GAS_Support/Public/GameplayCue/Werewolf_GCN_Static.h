// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "AbilitySystem/Abilities/Werewolf_Passive_Hit.h"
#include "Werewolf_GCN_Static.generated.h"

struct FHitEffectContext;
/**
 * 
 */
UCLASS()
class WESTERN_MAP_API UWerewolf_GCN_Static : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
protected:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FHitDynamicData GetHitEffectContext(const FGameplayCueParameters& Parameters);
	
	
	
};
