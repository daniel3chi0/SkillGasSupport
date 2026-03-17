// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "SkillAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class WESTERN_MAP_API USkillAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	USkillAttributeSet();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	//战技CD缩放系数
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BattleSkillCDScaleFactor, Category = "Vital Attributes")
	FGameplayAttributeData BattleSkillCDScaleFactor;
	ATTRIBUTE_ACCESSORS(USkillAttributeSet, BattleSkillCDScaleFactor);

	UFUNCTION()
	void OnRep_BattleSkillCDScaleFactor(const FGameplayAttributeData& OldBattleSkillCDScaleFactor) const;
};
