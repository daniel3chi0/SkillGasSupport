#pragma once
#include "TargetType.generated.h"

struct FGameplayAbilityTargetDataHandle;
struct FGameplayEventData;

UCLASS(Blueprintable, meta = (ShowWorldContextPin))
class SKILL_GAS_SUPPORT_API UTargetType : public UObject
{
	GENERATED_BODY()
public:
	UTargetType() {}

	UFUNCTION(BlueprintNativeEvent)
	void GetTargets(AActor* TargetingActor, FGameplayEventData EventData, TArray<FGameplayAbilityTargetDataHandle>& OutTargetData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const;
};

UCLASS(NotBlueprintable)
class SKILL_GAS_SUPPORT_API UTargetType_UseOwner : public UTargetType
{
	GENERATED_BODY()

public:
	UTargetType_UseOwner() {}
	
	virtual void GetTargets_Implementation(AActor* TargetingActor, FGameplayEventData EventData, TArray<FGameplayAbilityTargetDataHandle>& OutTargetData,
		TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};

UCLASS(NotBlueprintable)
class SKILL_GAS_SUPPORT_API UTargetType_UseEventData : public UTargetType
{
	GENERATED_BODY()

public:
	UTargetType_UseEventData() {}
	
	virtual void GetTargets_Implementation(AActor* TargetingActor, FGameplayEventData EventData, TArray<FGameplayAbilityTargetDataHandle>& OutTargetData,
		TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};
