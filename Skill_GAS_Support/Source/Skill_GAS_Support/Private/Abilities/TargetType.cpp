#include "AbilitySystem/Abilities/TargetType.h"
#include "Abilities/GameplayAbilityTypes.h"

void UTargetType::GetTargets_Implementation(AActor* TargetingActor, FGameplayEventData EventData, TArray<FGameplayAbilityTargetDataHandle>& OutTargetData, TArray<FHitResult>& OutHitResults,
                                            TArray<AActor*>& OutActors) const
{
	return;
}

void UTargetType_UseOwner::GetTargets_Implementation(AActor* TargetingActor, FGameplayEventData EventData, TArray<FGameplayAbilityTargetDataHandle>& OutTargetData, TArray<FHitResult>& OutHitResults,
	TArray<AActor*>& OutActors) const
{
	OutActors.Add(TargetingActor);
}

void UTargetType_UseEventData::GetTargets_Implementation(AActor* TargetingActor, FGameplayEventData EventData, TArray<FGameplayAbilityTargetDataHandle>& OutTargetData, TArray<FHitResult>& OutHitResults,
	TArray<AActor*>& OutActors) const
{
	const FHitResult* FoundHitResult = EventData.ContextHandle.GetHitResult();
	if (FoundHitResult)
	{
		OutHitResults.Add(*FoundHitResult);
	}
	else if (EventData.Target)
	{
		OutActors.Add(const_cast<AActor*>(EventData.Target.Get()));
	}
}
