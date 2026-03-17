#include "AbilitySystem/GameplayEffect/GameplayEffectContainerFunctionLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/Abilities/TargetType.h"
#include "Manager/SkillManager.h"

void UGameplayEffectContainerFunctionLibrary::GetDataFromTargetType(TSubclassOf<UTargetType> TargetType, const FGameplayEventData& EventData,
	TArray<FGameplayAbilityTargetDataHandle>& OutTargetData, TArray<FHitResult>& TargetHitResults, TArray<AActor*>& TargetActors)
{
	if(!EventData.Instigator)
	{
		UE_LOG(LogSkillSystem, Warning, TEXT("Can not found EventData.Instigator is: %s"), *EventData.EventTag.ToString());
	}
	
	UAbilitySystemComponent* SourceASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(EventData.Instigator);

	if (SourceASC && SourceASC->GetAvatarActor())
	{
		if (TargetType.Get())
		{
			const UTargetType* TargetTypeCDO = TargetType.GetDefaultObject();
			AActor* AvatarActor = SourceASC->GetAvatarActor();
			TargetTypeCDO->GetTargets(AvatarActor, EventData, OutTargetData, TargetHitResults, TargetActors);
			return;
		}
	}
	
	UE_LOG(LogSkillSystem, Warning, TEXT("Can not found ASC or ASC Avatar this ASC owner is: %s"), *EventData.Instigator.GetName());
}

FGameplayEffectContainerSpec UGameplayEffectContainerFunctionLibrary::MakeEffectContainerSpec(const FGameplayEffectContainer& Container, const FGameplayEventData& EventData,
                                                                                              int32 OverrideGameplayLevel, UGameplayAbility* SourceAbility)
{
	//直接从事件触发者获取
	FGameplayEffectContainerSpec ReturnSpec;
	UAbilitySystemComponent* SourceASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(EventData.Instigator);

	if (SourceASC && SourceASC->GetAvatarActor())
	{
		//原本配置了TargetActor就可以直接add进Container里面, 如果没有配置可能会在后续流程add进去
		if (Container.TargetType.Get())
		{
			TArray<FHitResult> HitResults;
			TArray<AActor*> TargetActors;
			TArray<FGameplayAbilityTargetDataHandle> TargetData;
			//const UTargetType* TargetTypeCDO = Container.TargetType.GetDefaultObject();
			//AActor* AvatarActor = SourceASC->GetAvatarActor();
			//TargetTypeCDO->GetTargets(AvatarActor, EventData, TargetData, HitResults, TargetActors);
			GetDataFromTargetType(Container.TargetType, EventData, TargetData, HitResults, TargetActors);
			ReturnSpec.AddTargets(TargetData, HitResults, TargetActors);
		}
		
		for (const FGEContainerGEWarp& EffectWarp : Container.TargetGameplayEffectClasses)
		{
			if (SourceAbility)
			{
				FGameplayEffectSpecHandle GESpecHandle = SourceAbility->MakeOutgoingGameplayEffectSpec(EffectWarp.TargetEffectClass, OverrideGameplayLevel);
				for (const TTuple<FGameplayTag, float>& Pair : EffectWarp.SetByCallerMagnitudeMap)
				{
					GESpecHandle.Data.Get()->SetSetByCallerMagnitude(Pair.Key, Pair.Value);	
				}
				
				ReturnSpec.TargetGameplayEffectSpecs.Add(GESpecHandle);
			}
			else
			{
				FGameplayEffectSpecHandle GESpecHandle = SourceASC->MakeOutgoingSpec(EffectWarp.TargetEffectClass, OverrideGameplayLevel, SourceASC->MakeEffectContext());
				for (const TTuple<FGameplayTag, float>& Pair : EffectWarp.SetByCallerMagnitudeMap)
				{
					GESpecHandle.Data.Get()->SetSetByCallerMagnitude(Pair.Key, Pair.Value);	
				}
				ReturnSpec.TargetGameplayEffectSpecs.Add(GESpecHandle);	
			}
		}
	}
	return ReturnSpec;
}

TArray<FActiveGameplayEffectHandle> UGameplayEffectContainerFunctionLibrary::ApplyEffectContainerSpec(UGameplayAbility* ExecutingAbility, const FGameplayEffectContainerSpec& ContainerSpec)
{
	TArray<FActiveGameplayEffectHandle> AllEffects;

	if (!IsValid(ExecutingAbility) || !ExecutingAbility->IsInstantiated())
	{
		UE_LOG(LogSkillSystem, Error, TEXT("Requires \"Executing ability\" to apply effect container spec."))
		return AllEffects;
	}

	const FGameplayAbilityActorInfo* ActorInfo = ExecutingAbility->GetCurrentActorInfo();
	const FGameplayAbilityActivationInfo& ActivationInfo = ExecutingAbility->GetCurrentActivationInfoRef();

	//给TargetData应用GE
	for (const FGameplayEffectSpecHandle& SpecHandle : ContainerSpec.TargetGameplayEffectSpecs)
	{
		TArray<FActiveGameplayEffectHandle> EffectHandles;

		if (SpecHandle.IsValid() && ExecutingAbility->HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
		{
			FScopedTargetListLock ActiveScopeLock(*ActorInfo->AbilitySystemComponent, *ExecutingAbility);

			for (TSharedPtr<FGameplayAbilityTargetData> Data : ContainerSpec.TargetData.Data)
			{
				if (Data.IsValid())
				{
					AllEffects.Append(Data->ApplyGameplayEffectSpec(*SpecHandle.Data.Get(), ActorInfo->AbilitySystemComponent->GetPredictionKeyForNewAction()));
				}
				else
				{
					UE_LOG(LogSkillSystem, Warning, TEXT("ApplyGameplayEffectSpecToTarget invalid target data passed in. Ability: %s"), *ExecutingAbility->GetPathName());
				}
			}
		}
	}

	return AllEffects;
}

TArray<FActiveGameplayEffectHandle> UGameplayEffectContainerFunctionLibrary::ApplyExternalEffectContainerSpec(const FGameplayEffectContainerSpec& ContainerSpec)
{
	TArray<FActiveGameplayEffectHandle> AllEffects;
	
	for (const FGameplayEffectSpecHandle& SpecHandle : ContainerSpec.TargetGameplayEffectSpecs)
	{
		if (SpecHandle.IsValid())
		{
			for (TSharedPtr<FGameplayAbilityTargetData> Data : ContainerSpec.TargetData.Data)
			{
				AllEffects.Append(Data->ApplyGameplayEffectSpec(*SpecHandle.Data.Get()));
			}
		}
	}
	return AllEffects;
}