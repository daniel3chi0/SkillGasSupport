#include "AbilitySystem/AsyncTasks/AsyncTask_SetDataLayerRuntimeStateCompleted.h"

#include "Kismet/KismetSystemLibrary.h"
#include "WorldPartition/WorldPartitionBlueprintLibrary.h"
#include "WorldPartition/WorldPartitionLevelStreamingPolicy.h"
#include "WorldPartition/WorldPartitionSubsystem.h"
#include "WorldPartition/DataLayer/DataLayerManager.h"

UAsyncTask_SetDataLayerRuntimeStateCompleted* UAsyncTask_SetDataLayerRuntimeStateCompleted::WaitSetDataLayerRuntimeStateCompleted(
	UObject* InWorldContextObject, const UDataLayerAsset* InDataLayerAsset, EDataLayerRuntimeState InState, bool bInIsRecursive, bool bInExactState)
{
	UAsyncTask_SetDataLayerRuntimeStateCompleted* TaskInstance = NewObject<UAsyncTask_SetDataLayerRuntimeStateCompleted>();
	TaskInstance->DataLayerAsset = const_cast<UDataLayerAsset*>(InDataLayerAsset);
	TaskInstance->TargetState = InState;
	TaskInstance->bIsRecursive = bInIsRecursive;
	TaskInstance->bExactState = bInExactState;
	TaskInstance->WorldContextObject = InWorldContextObject;
	TaskInstance->RegisterWithGameInstance(InWorldContextObject);
	return TaskInstance;
}

void UAsyncTask_SetDataLayerRuntimeStateCompleted::Activate()
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("Activate: World is null, cannot proceed"));
		EndTask();
		return;
	}
	
	if(UDataLayerManager* DataLayerManager = UDataLayerManager::GetDataLayerManager(World))
	{
		DataLayerManager->SetDataLayerRuntimeState(DataLayerAsset, TargetState, bIsRecursive);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Activate: DataLayerManager is null"));
	}

	if (RegisteredWithGameInstance.IsValid())
	{
		FTimerManager* TimerManager = &RegisteredWithGameInstance->GetTimerManager();
		if(TimerManager)
		{
			TimerManager->SetTimer(TimerHandle, this, &UAsyncTask_SetDataLayerRuntimeStateCompleted::CheckDataLayerStreamingCompleted,
			0.2f, true);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Activate: TimerManager is null"));
			EndTask();
		}
		/*AsyncTask(ENamedThreads::GameThread,[this]()
		{
			
		});*/
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Activate: GameInstance is invalid"));
		EndTask();
	}

	//WorldPartitionSubsystem = GetWorld()->GetSubsystem<UWorldPartitionSubsystem>();
}

void UAsyncTask_SetDataLayerRuntimeStateCompleted::EndTask()
{
	if (RegisteredWithGameInstance.IsValid())
	{
		FTimerManager* TimerManager = &RegisteredWithGameInstance->GetTimerManager();
		if (TimerManager && TimerHandle.IsValid())
		{
			TimerManager->ClearTimer(TimerHandle);
		}
	}
	
	SetReadyToDestroy();
	MarkAsGarbage();
}

void UAsyncTask_SetDataLayerRuntimeStateCompleted::CheckDataLayerStreamingCompleted()
{
	/*AsyncTask(ENamedThreads::GameThread,[this]()
	{
		
	});*/

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(this) || !World)
	{
		UE_LOG(LogTemp, Warning, TEXT("CheckDataLayerStreamingCompleted: Invalid object or World"));
		EndTask();
		return;
	}
		
	FName DataLayerName = FName(*UKismetSystemLibrary::GetDisplayName(DataLayerAsset));
	TArray<FWorldPartitionStreamingQuerySource> WorldPartitionStreamingQuerySources;
	//WorldPartitionStreamingQuerySources.AddUnique(WorldPartitionStreamingQuerySource);
		
	FWorldPartitionStreamingQuerySource& QuerySource = WorldPartitionStreamingQuerySources.Emplace_GetRef();
	QuerySource.Location = FVector::Zero();
	QuerySource.Radius = 0.f;
	TArray<FName> DataLayerNames;
	DataLayerNames.AddUnique(DataLayerName);
	QuerySource.DataLayers = DataLayerNames;
		
	if (UWorldPartitionSubsystem* WorldPartitionSubsystem = World->GetSubsystem<UWorldPartitionSubsystem>())
		//if(WorldPartitionSubsystem)
	{
		int32 Index = StaticEnum<EDataLayerRuntimeState>()->GetIndexByValue(static_cast<int64>(TargetState));
		UEnum* WorldPartitionRuntimeCellStateEnum = StaticEnum<EWorldPartitionRuntimeCellState>();
		EWorldPartitionRuntimeCellState WorldPartitionRuntimeCellState = static_cast<EWorldPartitionRuntimeCellState>(WorldPartitionRuntimeCellStateEnum->GetValueByIndex(Index));
		if(WorldPartitionSubsystem->IsStreamingCompleted(WorldPartitionRuntimeCellState, WorldPartitionStreamingQuerySources, bExactState))
		{
			if(OnSetDataLayerStateAndStreamingCompleted.IsBound())
			{
				OnSetDataLayerStateAndStreamingCompleted.Broadcast();	
			}

			UKismetSystemLibrary::K2_ClearAndInvalidateTimerHandle(this, TimerHandle);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CheckDataLayerStreamingCompleted: WorldPartitionSubsystem is null"));
		EndTask();
	}
}
