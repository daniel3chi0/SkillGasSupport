#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Abilities/Async/AbilityAsync.h"
#include "AsyncTask_GameplayTagAddedRemoved.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameplayTagAddedRemoved, FGameplayTag, Tag);

UCLASS(BlueprintType, meta = (ExposedAsyncProxy = AsyncTask))
class WESTERN_MAP_API UAsyncTask_GameplayTagAddedRemoved : public UAbilityAsync
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnGameplayTagAddedRemoved OnTagAdded;

	UPROPERTY(BlueprintAssignable)
	FOnGameplayTagAddedRemoved OnTagRemoved;

	//监听GameplayTag的添加和移除
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncTask_GameplayTagAddedRemoved* ListenForGameplayTagAddedOrRemoved(UAbilitySystemComponent* AbilitySystemComponent, FGameplayTagContainer Tags);
	
	UFUNCTION(BlueprintCallable, meta=(DeprecatedFunction, DeprecationMessage="Use EndAction"))
	void EndTask();

protected:
	virtual void EndAction() override;

	FGameplayTagContainer Tags;

	virtual void TagChanged(const FGameplayTag Tag, int32 NewCount);
};

