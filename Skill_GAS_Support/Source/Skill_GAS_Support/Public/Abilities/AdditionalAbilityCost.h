#pragma once
#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "AdditionalAbilityCost.generated.h"

struct FGameplayAbilityActorInfo;
class UGameplayAbility;

UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, Abstract)
class UAdditionalAbilityCost : public UObject
{
	GENERATED_BODY()
	
public:
	UAdditionalAbilityCost(){}

	virtual bool CheckCost(const UGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
	{
		return BP_CheckCost(Ability, Handle, *ActorInfo, *OptionalRelevantTags);
	}

	virtual void ApplyCost(const UGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
	{
		BP_ApplyCost(Ability, Handle, *ActorInfo, ActivationInfo);
	}

	//只有TargetData中有HitResult的时候消耗才能被应用
	bool ShouldOnlyApplyCostOnHit() const { return bOnlyApplyCostOnHit; }

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Costs", meta=(DisplayName="Check Cost"))
	bool BP_CheckCost(const UGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo, const FGameplayTagContainer& OptionalRelevantTags) const;

	bool BP_CheckCost_Implementation(const UGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo, const FGameplayTagContainer& OptionalRelevantTags) const
	{
		return true;
	}

	UFUNCTION(BlueprintNativeEvent, Category = "Costs", meta=(DisplayName="Apply Cost"))
	void BP_ApplyCost(const UGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo, const FGameplayAbilityActivationInfo& ActivationInfo);

	void BP_ApplyCost_Implementation(const UGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo, const FGameplayAbilityActivationInfo& ActivationInfo)
	{
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Costs)
	bool bOnlyApplyCostOnHit = false;
};
