// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityTypes.h"
#include "Abilities/GameplayAbility.h"
#include "Werewolf_Ability.generated.h"

class UWerewolf_AnimConfig;
struct FGameplayEffectContainer;
class UAdditionalAbilityCost;
/**
 * 
 */
UCLASS()
class WESTERN_MAP_API UWerewolf_Ability : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayEffects")
	TMap<FGameplayTag, FGameplayEffectContainer> EffectContainerMap;

	//not bIsCancelable
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	bool bIsCancelableSkill = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	int32 SkillId = 0;
	
protected:
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FScalableFloat AbilityChooseNum = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Cooldown")
	FScalableFloat CooldownDuration;

	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category = "Cooldown")
	FGameplayTagContainer CooldownTags;

	UPROPERTY(Transient)
	FGameplayTagContainer TempCooldownTags;

	//扩展gas原生的ge消耗
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Costs")
	TArray<TObjectPtr<UAdditionalAbilityCost>> AdditionalCosts;
	
public:
	UWerewolf_Ability();
	
	UFUNCTION(Server,Reliable,WithValidation)
	void SetChooseNum(FScalableFloat InNum);

	UFUNCTION(BlueprintPure)
	UWerewolf_AnimConfig* GetCharaterAnimConfig();

	virtual void SetChooseNum_Implementation(FScalableFloat InNum);

	virtual bool SetChooseNum_Validate(FScalableFloat InNum);

	virtual const FGameplayTagContainer* GetCooldownTags() const override;
	
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	virtual FGameplayAbilityTargetDataHandle MakeGameplayAbilityTargetDataHandleFromActorArray(const TArray<AActor*> TargetActors);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	virtual FGameplayAbilityTargetDataHandle MakeGameplayAbilityTargetDataHandleFromHitResults(const TArray<FHitResult> HitResults);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData", meta = (AutoCreateRefTerm = "EventData"))
	virtual FGameplayAbilityTargetDataHandle MakeTargetDataFromTargetType(TSubclassOf<UTargetType> TargetType, const FGameplayEventData& EventData);

	// 根据Tag找到ge container然后生成spec
	UFUNCTION(BlueprintCallable, Category = "Ability", meta = (AutoCreateRefTerm = "EventData"))
	virtual FGameplayEffectContainerSpec MakeEffectContainerSpecFromContainerMap(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);

	// 查找 创建 应用GE Container Spec
	UFUNCTION(BlueprintCallable, Category = "Ability", meta = (AutoCreateRefTerm = "EventData"))
	virtual TArray<FActiveGameplayEffectHandle> ApplyEffectContainerSpecFromContainerMap(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel);

	FGameplayTag GetFirstAbilityTriggersEventTag();

	virtual UGameplayEffect* GetCostGameplayEffect() const override;

	virtual bool CanActivateAbilityReturnMsg(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
		const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags, FString& ReturnMsg) const;
	
protected:

	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	
	virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual bool DoesAbilitySatisfyTagRequirementsWithMsg(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr, FString* ReturnMsg = nullptr) const;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
