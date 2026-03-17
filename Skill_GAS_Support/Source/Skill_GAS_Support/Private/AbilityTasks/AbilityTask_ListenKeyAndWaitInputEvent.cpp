// Fill out your copyright notice in the Description page of Project Settings.
#include "AbilityTasks/AbilityTask_ListenKeyAndWaitInputEvent.h"

int32 UAbilityTask_ListenKeyAndWaitInputEvent::RemoveIndex = 0;

UAbilityTask_ListenKeyAndWaitInputEvent::UAbilityTask_ListenKeyAndWaitInputEvent(const FObjectInitializer& ObjectInitializer)
{
	ListenKey = EKeys::AnyKey;
	InputEvent = EInputEvent::IE_Pressed;
}

UAbilityTask_ListenKeyAndWaitInputEvent* UAbilityTask_ListenKeyAndWaitInputEvent::ListenKeyAndWaitInputEvent(UGameplayAbility* OwningAbility, FKey ListenKey,
	EInputEvent InputEvent)
{
	UAbilityTask_ListenKeyAndWaitInputEvent* Task = NewAbilityTask<UAbilityTask_ListenKeyAndWaitInputEvent>(OwningAbility);
	Task->ListenKey = ListenKey;
	Task->InputEvent = InputEvent;
	return Task;
}

void UAbilityTask_ListenKeyAndWaitInputEvent::Activate()
{
	if(!Ability) return;

	AActor* Avatar = Ability->GetAvatarActorFromActorInfo();
	if(Avatar)
	{
		if(Avatar->InputComponent)
		{
			InputKeyBinding = Avatar->InputComponent->BindKey(ListenKey, InputEvent, this, &UAbilityTask_ListenKeyAndWaitInputEvent::OnICBindKeyInputEventCallBack);
			CacheIndex = Avatar->InputComponent->KeyBindings.Num() - 1;
		}
	}
}

void UAbilityTask_ListenKeyAndWaitInputEvent::OnDestroy(bool bInOwnerFinished)
{
	if(Ability)
	{
		AActor* Avatar = Ability->GetAvatarActorFromActorInfo();
		if(Avatar)
		{
			if(Avatar->InputComponent)
			{
				if(RemoveIndex > 0 && RemoveIndex < CacheIndex)
					CacheIndex = RemoveIndex;
				Avatar->InputComponent->KeyBindings.RemoveAt(CacheIndex, EAllowShrinking::No);
				RemoveIndex = CacheIndex;
			}
		}
	}
	
	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_ListenKeyAndWaitInputEvent::OnICBindKeyInputEventCallBack()
{
	if(ShouldBroadcastAbilityTaskDelegates())
	{
		if(OnKeyInputEventCallBack.IsBound())
		{
			OnKeyInputEventCallBack.Broadcast();
		}
	}

	EndTask();
}
