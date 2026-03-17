#pragma once
#include "Kismet/BlueprintAsyncActionBase.h"
#include "WorldPartition/DataLayer/DataLayerInstance.h"
#include "AsyncTask_SetDataLayerRuntimeStateCompleted.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSetDataLayerStateAndStreamingCompleted);

UCLASS(BlueprintType, meta = (ExposedAsyncProxy = AsyncTask))
class SKILL_GAS_SUPPORT_API UAsyncTask_SetDataLayerRuntimeStateCompleted : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "InWorldContextObject"))
	static UAsyncTask_SetDataLayerRuntimeStateCompleted* WaitSetDataLayerRuntimeStateCompleted(UObject* InWorldContextObject, const UDataLayerAsset* InDataLayerAsset,
		EDataLayerRuntimeState InState, bool bInIsRecursive, bool bInExactState);

	virtual void Activate() override;
	
	UFUNCTION(BlueprintCallable)
	void EndTask();

	void CheckDataLayerStreamingCompleted();
	
protected:

	UPROPERTY()
	UDataLayerAsset* DataLayerAsset;

	EDataLayerRuntimeState TargetState;

	bool bIsRecursive;

	FTimerHandle TimerHandle;

	bool bExactState;

	UPROPERTY()
	UObject* WorldContextObject;

	//UWorldPartitionSubsystem* WorldPartitionSubsystem;

	UPROPERTY(BlueprintAssignable)
	FOnSetDataLayerStateAndStreamingCompleted OnSetDataLayerStateAndStreamingCompleted;
};
