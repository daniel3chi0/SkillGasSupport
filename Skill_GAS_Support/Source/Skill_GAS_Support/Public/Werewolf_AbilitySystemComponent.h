// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilityTagRelationshipMapping.h"
#include "Werewolf_AbilitySystemComponent.generated.h"

class UWerewolf_GameTotalConfig;
struct FInputActionValue;
/**
 * 
 */
UCLASS()
class WESTERN_MAP_API UWerewolf_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	UWerewolf_AbilitySystemComponent();
	virtual void BeginPlay() override;
public:

	void AbilityInputTagPressed(const FGameplayTag& InputTag,const FInputActionValue& InputActionValue);
	void AbilityInputTagReleased(const FGameplayTag& InputTag,const FInputActionValue& InputActionValue);

	//处理能力输入
	void ProcessAbilityInput(float DeltaTime , bool bGamePaused);

	UFUNCTION(BlueprintCallable,Category = "Input")
	void ClearAbilityInput();

	//修改所有技能参数包括客户端和服务器
	UFUNCTION()
	void SetAllSkillParameters(FScalableFloat InPara);
	
	//修改Server技能参数的RPC
	UFUNCTION(Server,Reliable,WithValidation)
	void SetSkillParameters(FScalableFloat InPara);

	virtual void SetSkillParameters_Implementation(FScalableFloat InPara);

	virtual bool SetSkillParameters_Validate(FScalableFloat InPara);

	//根据ability Tag 返回TRM中的 require和blocked tag并增量加到传入的Tags中
	void GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const;
	
	void SetTagRelationshipMapping(UAbilityTagRelationshipMapping* NewMapping);

	void GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle, FGameplayAbilityActivationInfo ActivationInfo, FGameplayAbilityTargetDataHandle& OutTargetDataHandle);

	UFUNCTION(BlueprintCallable)
	void K2_CancelAbilities(FGameplayTagContainer TagContainer);

	UFUNCTION(BlueprintCallable)
	bool K2_IsAbilityActiveByTag(const FGameplayTag& Tag);

	UFUNCTION(BlueprintCallable)
	bool K2_CanActivateAbilityByTagsWithMsg(FGameplayTagContainer Tags, FString& ResultMsg);

protected:
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FScalableFloat SkillParameters = 0;

	
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

	// If set, this table is used to look up tag relationships for activate and cancel
	UPROPERTY()
	TObjectPtr<UAbilityTagRelationshipMapping> TagRelationshipMapping;

	UPROPERTY()
	UWerewolf_GameTotalConfig* GameTotalConfig;

	FString AssetPath = TEXT("/Game/Program/Config/GameConfig/GameTotalConfig.GameTotalConfig");
protected:
	void OnGameplayTagChanged(const FGameplayTag Tag,int32 NewCount);
	void AddViolationTagEvent();

	virtual UGameplayAbility* CreateNewInstanceOfAbility(FGameplayAbilitySpec& Spec, const UGameplayAbility* Ability) override;
protected:
	
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

	virtual void ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags) override;
};
