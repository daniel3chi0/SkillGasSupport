// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Executions/HealthRegenExecution.h"

#include "AbilitySystem/Werewolf_AttributeSet.h"

struct SHealthStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxHealth);

	SHealthStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWerewolf_AttributeSet, Health, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWerewolf_AttributeSet, MaxHealth, Source, false);
	}
};

UHealthRegenExecution::UHealthRegenExecution()
{
	RelevantAttributesToCapture.Add(SHealthStatics().HealthDef);
	RelevantAttributesToCapture.Add(SHealthStatics().MaxHealthDef);
	
}

void UHealthRegenExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                   FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float Health = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(SHealthStatics().HealthDef, EvaluateParameters, Health);
	float UnmitigatedHealth = Health;

	float MaxHealth = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(SHealthStatics().MaxHealthDef, EvaluateParameters, MaxHealth);
	float UnmitigatedMaxHealth = MaxHealth;

	UAbilitySystemComponent* SASC = ExecutionParams.GetSourceAbilitySystemComponent();
	UWerewolf_AttributeSet* AttributeSet = const_cast<UWerewolf_AttributeSet*>(Cast<UWerewolf_AttributeSet>(SASC->GetAttributeSet(UWerewolf_AttributeSet::StaticClass())));
	UWerewolf_AttributeSet* AttributeSetCDO = AttributeSet->GetClass()->GetDefaultObject<UWerewolf_AttributeSet>();
	float DefaultMaxHealth = AttributeSetCDO->GetMaxHealth();
	
	if(CalcuType == ECalcuType::ECT_Reduce)
	{
		UnmitigatedMaxHealth = MaxHealth - HealthCoefficient * DefaultMaxHealth;
	}
	else if(CalcuType == ECalcuType::ECT_Increase)
	{
		UnmitigatedMaxHealth = MaxHealth + HealthCoefficient * DefaultMaxHealth;
	}
	
	if(Health > UnmitigatedMaxHealth)
	{
		UnmitigatedHealth = UnmitigatedMaxHealth;
	}

	if(Health != UnmitigatedHealth)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(SHealthStatics().HealthProperty, EGameplayModOp::Override, UnmitigatedHealth));
	}
	
	if(MaxHealth != UnmitigatedMaxHealth)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(SHealthStatics().MaxHealthProperty, EGameplayModOp::Override, UnmitigatedMaxHealth));
	}
}
