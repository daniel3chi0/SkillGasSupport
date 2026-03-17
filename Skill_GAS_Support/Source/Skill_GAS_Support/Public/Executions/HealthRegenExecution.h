// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "HealthRegenExecution.generated.h"

UENUM(BlueprintType)
enum class ECalcuType : uint8
{
	ECT_Reduce,
	ECT_Increase,
};
/**
 * 
 */
UCLASS()
class WESTERN_MAP_API UHealthRegenExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UHealthRegenExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

public:

	UPROPERTY(EditAnywhere, Category = Extend)
	float HealthCoefficient;

	UPROPERTY(EditAnywhere, Category = Extend)
	ECalcuType CalcuType;
};
