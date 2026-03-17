// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_ListenKeyAndWaitInputEvent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FListenKeyInputEventDelegate);
/**
 * 
 */
UCLASS()
class SKILL_GAS_SUPPORT_API UAbilityTask_ListenKeyAndWaitInputEvent : public UAbilityTask
{
	GENERATED_BODY()

public:
	
	UAbilityTask_ListenKeyAndWaitInputEvent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_ListenKeyAndWaitInputEvent* ListenKeyAndWaitInputEvent(UGameplayAbility* OwningAbility, FKey ListenKeys, EInputEvent InputEvent);

protected:
	
	virtual void Activate() override;

	virtual void OnDestroy(bool bInOwnerFinished) override;

private:

	FKey ListenKey;

	EInputEvent InputEvent;

	FInputKeyBinding InputKeyBinding;

	int32 CacheIndex;

	static int32 RemoveIndex;

public:

	UPROPERTY(BlueprintAssignable)
	FListenKeyInputEventDelegate OnKeyInputEventCallBack;

private:

	void OnICBindKeyInputEventCallBack();
};
