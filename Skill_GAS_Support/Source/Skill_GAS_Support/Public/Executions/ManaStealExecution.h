#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "ManaStealExecution.generated.h"

UCLASS()
class WESTERN_MAP_API UManaStealExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UManaStealExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
