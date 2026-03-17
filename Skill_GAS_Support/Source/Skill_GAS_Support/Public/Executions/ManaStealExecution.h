#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "ManaStealExecution.generated.h"

UCLASS()
class SKILL_GAS_SUPPORT_API UManaStealExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UManaStealExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
