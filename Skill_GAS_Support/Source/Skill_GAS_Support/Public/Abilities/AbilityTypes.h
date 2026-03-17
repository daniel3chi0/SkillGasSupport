#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "AbilityTypes.generated.h"

struct FGameplayEffectSpecHandle;
class UGameplayEffect;
class UTargetType;

USTRUCT(BlueprintType)
struct FGEContainerGEWarp
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> TargetEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FGameplayTag, float> SetByCallerMagnitudeMap;
};

USTRUCT(BlueprintType)
struct FGameplayEffectContainer
{
	GENERATED_BODY()
public:
	FGameplayEffectContainer(){}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameplayEffectContainer")
	TSubclassOf<UTargetType> TargetType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameplayEffectContainer")
	TArray<FGEContainerGEWarp> TargetGameplayEffectClasses;
};

USTRUCT(BlueprintType)
struct FGameplayEffectContainerSpec
{
	GENERATED_BODY()

public:
	FGameplayEffectContainerSpec() {}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameplayEffectContainer")
	FGameplayAbilityTargetDataHandle TargetData;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameplayEffectContainer")
	TArray<FGameplayEffectSpecHandle> TargetGameplayEffectSpecs;
	
	bool HasValidEffects() const;
	
	bool HasValidTargets() const;
	
	void AddTargets(const TArray<FGameplayAbilityTargetDataHandle>& InTargetData, const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors);
	
	void ClearTargets();
};
