// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/AbilityTask_WaitTargetDataReusableActor.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "AbilitySystem/TargetActor/GATA_GroundTrace.h"
#include "AbilitySystem/TargetActor/GATA_Trace.h"


UAbilityTask_WaitTargetDataReusableActor* UAbilityTask_WaitTargetDataReusableActor::WaitTargetDataWithReusableActor(UGameplayAbility* OwningAbility, FName TaskInstanceName,
                                                                                                  TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType, AGameplayAbilityTargetActor* InTargetActor, bool bCreateKeyIfNotValidForMorePredicting)
{
	UAbilityTask_WaitTargetDataReusableActor* Task = NewAbilityTask<UAbilityTask_WaitTargetDataReusableActor>(OwningAbility, TaskInstanceName);
	Task->TargetActor = InTargetActor;
	Task->ConfirmationType = ConfirmationType;
	Task->bCreateKeyIfNotValidForMorePredicting = bCreateKeyIfNotValidForMorePredicting;
	return Task;
}

void UAbilityTask_WaitTargetDataReusableActor::Activate()
{
	if(!IsValid(this))
	{
		return;
	}

	if(Ability && TargetActor)
	{
		InitializeTargetActor();
		RegisterTargetDataCallbacks();
		FinalizeTargetActor();
	}
	else
	{
		EndTask();
	}
}

void UAbilityTask_WaitTargetDataReusableActor::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ActivationTag)
{
	check(AbilitySystemComponent.IsValid());

	FGameplayAbilityTargetDataHandle MutableData = Data;
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());

	/**
	 *  Call into the TargetActor to sanitize/verify the data. If this returns false, we are rejecting
	 *	the replicated target data and will treat this as a cancel.
	 *
	 *	This can also be used for bandwidth optimizations. OnReplicatedTargetDataReceived could do an actual
	 *	trace/check/whatever server side and use that data. So rather than having the client send that data
	 *	explicitly, the client is basically just sending a 'confirm' and the server is now going to do the work
	 *	in OnReplicatedTargetDataReceived.
	 */
	if (TargetActor && !TargetActor->OnReplicatedTargetDataReceived(MutableData))
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			Cancelled.Broadcast(MutableData);
		}
	}
	else
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			ValidData.Broadcast(MutableData);
		}
	}

	if (ConfirmationType != EGameplayTargetingConfirmation::CustomMulti)
	{
		EndTask();
	}
}

void UAbilityTask_WaitTargetDataReusableActor::OnTargetDataReplicatedCancelledCallback()
{
	check(AbilitySystemComponent.IsValid());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		Cancelled.Broadcast(FGameplayAbilityTargetDataHandle());
	}
	EndTask();
}

void UAbilityTask_WaitTargetDataReusableActor::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& Data)
{
	check(AbilitySystemComponent.IsValid());
	if (!Ability)
	{
		return;
	}

	FScopedPredictionWindow	ScopedPrediction(AbilitySystemComponent.Get(),
		ShouldReplicateDataToServer() && (bCreateKeyIfNotValidForMorePredicting && !AbilitySystemComponent->ScopedPredictionKey.IsValidForMorePrediction()));

	const FGameplayAbilityActorInfo* Info = Ability->GetCurrentActorInfo();
	if (IsPredictingClient())
	{
		if (!TargetActor->ShouldProduceTargetDataOnServer)
		{
			FGameplayTag ApplicationTag; // Fixme: where would this be useful?
			AbilitySystemComponent->CallServerSetReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey(), Data, ApplicationTag, AbilitySystemComponent->ScopedPredictionKey);
		}
		else if (ConfirmationType == EGameplayTargetingConfirmation::UserConfirmed)
		{
			// We aren't going to send the target data, but we will send a generic confirmed message.
			AbilitySystemComponent->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::GenericConfirm, GetAbilitySpecHandle(), GetActivationPredictionKey(), AbilitySystemComponent->ScopedPredictionKey);
		}
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(Data);
	}

	if (ConfirmationType != EGameplayTargetingConfirmation::CustomMulti)
	{
		EndTask();
	}
}

void UAbilityTask_WaitTargetDataReusableActor::OnTargetDataCancelledCallback(const FGameplayAbilityTargetDataHandle& Data)
{
	check(AbilitySystemComponent.IsValid());

	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get(), IsPredictingClient());

	if (IsPredictingClient())
	{
		if (!TargetActor->ShouldProduceTargetDataOnServer)
		{
			AbilitySystemComponent->ServerSetReplicatedTargetDataCancelled(GetAbilitySpecHandle(), GetActivationPredictionKey(), AbilitySystemComponent->ScopedPredictionKey);
		}
		else
		{
			// We aren't going to send the target data, but we will send a generic confirmed message.
			AbilitySystemComponent->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::GenericCancel, GetAbilitySpecHandle(), GetActivationPredictionKey(), AbilitySystemComponent->ScopedPredictionKey);
		}
	}
	Cancelled.Broadcast(Data);
	EndTask();
}

void UAbilityTask_WaitTargetDataReusableActor::ExternalConfirm(bool bEndTask)
{
	check(AbilitySystemComponent.IsValid());
	if (TargetActor)
	{
		if (TargetActor->ShouldProduceTargetData())
		{
			TargetActor->ConfirmTargetingAndContinue();
		}
	}
	Super::ExternalConfirm(bEndTask);
}

void UAbilityTask_WaitTargetDataReusableActor::ExternalCancel()
{
	check(AbilitySystemComponent.IsValid());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		Cancelled.Broadcast(FGameplayAbilityTargetDataHandle());
	}
	Super::ExternalCancel();
}

void UAbilityTask_WaitTargetDataReusableActor::InitializeTargetActor() const
{
	check(TargetActor);
	check(Ability);

	TargetActor->PrimaryPC = Ability->GetCurrentActorInfo()->PlayerController.Get();

	// If we spawned the target actor, always register the callbacks for when the data is ready.
	TargetActor->TargetDataReadyDelegate.AddUObject(const_cast<UAbilityTask_WaitTargetDataReusableActor*>(this), &UAbilityTask_WaitTargetDataReusableActor::OnTargetDataReadyCallback);
	TargetActor->CanceledDelegate.AddUObject(const_cast<UAbilityTask_WaitTargetDataReusableActor*>(this), &UAbilityTask_WaitTargetDataReusableActor::OnTargetDataCancelledCallback);
}

void UAbilityTask_WaitTargetDataReusableActor::FinalizeTargetActor() const
{
	check(TargetActor);
	check(Ability);

	TargetActor->StartTargeting(Ability);

	if (TargetActor->ShouldProduceTargetData())
	{
		// If instant confirm, then stop targeting immediately.
		// Note this is kind of bad: we should be able to just call a static func on the CDO to do this. 
		// But then we wouldn't get to set ExposeOnSpawnParameters.
		if (ConfirmationType == EGameplayTargetingConfirmation::Instant)
		{
			TargetActor->ConfirmTargeting();
		}
		else if (ConfirmationType == EGameplayTargetingConfirmation::UserConfirmed)
		{
			// Bind to the Cancel/Confirm Delegates (called from local confirm or from repped confirm)
			TargetActor->BindToConfirmCancelInputs();
		}
	}
}

void UAbilityTask_WaitTargetDataReusableActor::RegisterTargetDataCallbacks()
{
	if (!(!IsValid(this)))
	{
		return;
	}

	check(Ability);

	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	const bool bShouldProduceTargetDataOnServer = TargetActor->ShouldProduceTargetDataOnServer;

	// If not locally controlled (server for remote client), see if TargetData was already sent
	// else register callback for when it does get here.
	if (!bIsLocallyControlled)
	{
		// Register with the TargetData callbacks if we are expecting client to send them
		if (!bShouldProduceTargetDataOnServer)
		{
			FGameplayAbilitySpecHandle	SpecHandle = GetAbilitySpecHandle();
			FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();

			//Since multifire is supported, we still need to hook up the callbacks
			AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UAbilityTask_WaitTargetDataReusableActor::OnTargetDataReplicatedCallback);
			AbilitySystemComponent->AbilityTargetDataCancelledDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UAbilityTask_WaitTargetDataReusableActor::OnTargetDataReplicatedCancelledCallback);

			AbilitySystemComponent->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);

			SetWaitingOnRemotePlayerData();
		}
	}
}

void UAbilityTask_WaitTargetDataReusableActor::OnDestroy(bool AbilityEnded)
{
	if (TargetActor)
	{
		AGATA_Trace* TraceTargetActor = Cast<AGATA_Trace>(TargetActor);
		AGATA_GroundTrace* GroundTraceActor = Cast<AGATA_GroundTrace>(TargetActor);
		if (TraceTargetActor)
		{
			TraceTargetActor->StopTargeting();
		}
		else if(GroundTraceActor)
		{
			GroundTraceActor->StopTargeting();
		}	
		else
		{
			// TargetActor doesn't have a StopTargeting function
			TargetActor->SetActorTickEnabled(false);

			// Clear added callbacks
			TargetActor->TargetDataReadyDelegate.RemoveAll(this);
			TargetActor->CanceledDelegate.RemoveAll(this);

			AbilitySystemComponent->GenericLocalConfirmCallbacks.RemoveDynamic(TargetActor, &AGameplayAbilityTargetActor::ConfirmTargeting);
			AbilitySystemComponent->GenericLocalCancelCallbacks.RemoveDynamic(TargetActor, &AGameplayAbilityTargetActor::CancelTargeting);
			TargetActor->GenericDelegateBoundASC = nullptr;
		}
	}

	Super::OnDestroy(AbilityEnded);
}

bool UAbilityTask_WaitTargetDataReusableActor::ShouldReplicateDataToServer() const
{
	if (!Ability || !TargetActor)
	{
		return false;
	}

	// Send TargetData to the server IFF we are the client and this isn't a GameplayTargetActor that can produce data on the server	
	const FGameplayAbilityActorInfo* Info = Ability->GetCurrentActorInfo();
	if (!Info->IsNetAuthority() && !TargetActor->ShouldProduceTargetDataOnServer)
	{
		return true;
	}

	return false;
}
