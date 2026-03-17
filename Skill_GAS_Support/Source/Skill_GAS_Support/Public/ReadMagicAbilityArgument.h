// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillStructType.h"
#include "ReadMagicAbilityArgument.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class WESTERN_MAP_API UReadMagicAbilityArgument : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadOnly)
	FMagicRowStruct MagicRow;

	UPROPERTY(BlueprintReadOnly)
	FGameplayAbilitySpecHandle CurrentEquipedMagicSpecHandle;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	virtual bool IsSupportedForNetworking() const override
	{
		return true;
	}
};

template<>
struct TStructOpsTypeTraits<UReadMagicAbilityArgument> : public TStructOpsTypeTraitsBase2<UReadMagicAbilityArgument>
{
	enum
	{
		WithNetSerializer = true,
	};
};
