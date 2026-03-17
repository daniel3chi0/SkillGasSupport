#pragma once

#include "Abilities/AbilityTypes.h"
#include "GameplayEffectContainerFunctionLibrary.generated.h"


struct FGameplayEventData;

UCLASS()
class SKILL_GAS_SUPPORT_API UGameplayEffectContainerFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "GameplayEffect|Container", meta = (AutoCreateRefTerm = "EventData"))
	static void GetDataFromTargetType(TSubclassOf<UTargetType> TargetType, const FGameplayEventData& EventData, TArray<FGameplayAbilityTargetDataHandle>& OutTargetData,
		TArray<FHitResult>& TargetHitResults, TArray<AActor*>& TargetActors);
	
	UFUNCTION(BlueprintCallable, Category = "GameplayEffect|Container", meta = (AutoCreateRefTerm = "EventData"))
	static FGameplayEffectContainerSpec MakeEffectContainerSpec(const FGameplayEffectContainer& Container, const FGameplayEventData& EventData,
																	int32 OverrideGameplayLevel = -1, UGameplayAbility* SourceAbility = nullptr);
	
	UFUNCTION(BlueprintCallable, Category = "Ability|EffectContainer",meta = (DefaultToSelf="ExecutingAbility"))
	static TArray<FActiveGameplayEffectHandle> ApplyEffectContainerSpec(UGameplayAbility* ExecutingAbility, const FGameplayEffectContainerSpec& ContainerSpec);

	UFUNCTION(BlueprintCallable, Category = "Ability|EffectContainer",meta = (DefaultToSelf="ExecutingAbility"))
	static TArray<FActiveGameplayEffectHandle> ApplyExternalEffectContainerSpec(const FGameplayEffectContainerSpec& ContainerSpec);
};
