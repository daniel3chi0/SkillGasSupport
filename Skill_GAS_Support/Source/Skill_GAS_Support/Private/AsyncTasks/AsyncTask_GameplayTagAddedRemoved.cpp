#include "AsyncTasks/AsyncTask_GameplayTagAddedRemoved.h"

UAsyncTask_GameplayTagAddedRemoved* UAsyncTask_GameplayTagAddedRemoved::ListenForGameplayTagAddedOrRemoved(UAbilitySystemComponent* AbilitySystemComponent, FGameplayTagContainer InTags)
{
	UAsyncTask_GameplayTagAddedRemoved* TaskInstance = NewObject<UAsyncTask_GameplayTagAddedRemoved>();
	TaskInstance->SetAbilitySystemComponent(AbilitySystemComponent);
	TaskInstance->Tags = InTags;

	if (!IsValid(AbilitySystemComponent) || InTags.Num() < 1)
	{
		TaskInstance->EndTask();
		return nullptr;
	}

	TArray<FGameplayTag> TagArray;
	InTags.GetGameplayTagArray(TagArray);

	for (FGameplayTag Tag : TagArray)
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved).AddUObject(TaskInstance, &UAsyncTask_GameplayTagAddedRemoved::TagChanged);
	}

	return TaskInstance;
}

void UAsyncTask_GameplayTagAddedRemoved::EndTask()
{
	EndAction();
}

void UAsyncTask_GameplayTagAddedRemoved::EndAction()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		TArray<FGameplayTag> TagArray;
		Tags.GetGameplayTagArray(TagArray);

		for (FGameplayTag Tag : TagArray)
		{
			ASC->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
		}
	}

	Super::EndAction();
}

void UAsyncTask_GameplayTagAddedRemoved::TagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0)
	{
		OnTagAdded.Broadcast(Tag);
	}
	else
	{
		OnTagRemoved.Broadcast(Tag);
	}
}
