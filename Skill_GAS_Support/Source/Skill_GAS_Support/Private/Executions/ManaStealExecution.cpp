#include "AbilitySystem/Executions/ManaStealExecution.h"
#include "AbilitySystem/Werewolf_AttributeSet.h"

struct SManaStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Mana);

	SManaStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWerewolf_AttributeSet, Mana, Target, true);
	}
};

static const SManaStatics& ManaStatics()
{
	static SManaStatics ManaStatics;
	return ManaStatics;
}

UManaStealExecution::UManaStealExecution()
{
	RelevantAttributesToCapture.Add(SManaStatics().ManaDef);
}

void UManaStealExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float TargetMana = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(SManaStatics().ManaDef, EvaluateParameters, TargetMana);
	float ManaStealValue = FMath::Max<float>(Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Caller.ManaStealValue")), false, -1.0f), 0.0f);
	float FinalStealValue = -1 * (TargetMana > ManaStealValue ? ManaStealValue : TargetMana);
	
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(ManaStatics().ManaProperty, EGameplayModOp::Additive, FinalStealValue));

	FGameplayTagContainer SpecAssetTags;
	Spec.GetAllAssetTags(SpecAssetTags);
	if (SpecAssetTags.HasTag(FGameplayTag::RequestGameplayTag(FName("EffectAssetTag.CanManaSteal"))))
	{
		float ManaStoleValue = -FinalStealValue;

		UGameplayEffect* GEManaObtain = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("ManaObtain")));
		GEManaObtain->DurationPolicy = EGameplayEffectDurationType::Instant;

		int32 Idx = GEManaObtain->Modifiers.Num();
		GEManaObtain->Modifiers.SetNum(Idx + 1);
		FGameplayModifierInfo& Info = GEManaObtain->Modifiers[Idx];
		Info.ModifierMagnitude = FScalableFloat(ManaStoleValue);
		Info.ModifierOp = EGameplayModOp::Additive;
		Info.Attribute = UWerewolf_AttributeSet::GetManaAttribute();

		UAbilitySystemComponent* SASC = ExecutionParams.GetSourceAbilitySystemComponent();
		SASC->ApplyGameplayEffectToSelf(GEManaObtain, 1.0f, SASC->MakeEffectContext());
	}
}
