#pragma once
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "AbilitySystem/SkillStructType.h"
#include "Werewolf_GATargetDataType.generated.h"

UENUM(BlueprintType)
enum class ETargetDataType : uint8
{
	None,
	ActorArray,
	HitResult
};

USTRUCT(BlueprintType)
struct WESTERN_MAP_API FGameplayAbilityTargetData_ActorsOrHitResult : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	// FGameplayAbilityTargetData_ActorsOrHitResult(){}
	
	/** We could be selecting this group of actors from any type of location, so use a generic location type */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Targeting)
	FGameplayAbilityTargetingLocationInfo SourceLocation;

	/** Rather than targeting a single point, this type of targeting selects multiple actors. */
	UPROPERTY(EditAnywhere, Category = Targeting)
	TArray<TWeakObjectPtr<AActor> > TargetActorArray;

	UPROPERTY()
	FHitResult	HitResult;

	UPROPERTY(NotReplicated)
	bool bHitReplaced = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Targeting)
	ETargetDataType TargetDataType = ETargetDataType::None;

#pragma region MagicData
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Targeting)
	FMagicRowStruct MagicRow;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Targeting)
	FGameplayAbilitySpecHandle CurrentEquipedMagicSpecHandle;
	
#pragma endregion 魔法系统数据

	virtual TArray<TWeakObjectPtr<AActor> >	GetActors() const override
	{
		if(TargetDataType == ETargetDataType::ActorArray)
		{
			return TargetActorArray;
		}
		else if(TargetDataType == ETargetDataType::HitResult)
		{
			TArray<TWeakObjectPtr<AActor> >	Actors;
			if (HitResult.HasValidHitObjectHandle())
			{
				Actors.Push(HitResult.HitObjectHandle.FetchActor());
			}
			return Actors;
		}
		
		return Super::GetActors();
	}

	virtual bool SetActors(TArray<TWeakObjectPtr<AActor>> NewActorArray) override
	{
		TargetActorArray = NewActorArray;
		return true;
	}

	virtual bool HasHitResult() const override
	{
		return true;
	}
	
	virtual const FHitResult* GetHitResult() const override
	{
		return &HitResult;
	}

	// -------------------------------------

	virtual bool HasOrigin() const override
	{
		return true;
	}

	virtual FTransform GetOrigin() const override
	{
		if(TargetDataType == ETargetDataType::ActorArray)
		{
			FTransform ReturnTransform = SourceLocation.GetTargetingTransform();

			//Aim at first valid target, if we have one. Duplicating GetEndPoint() code here so we don't iterate through the target array twice.
			for (int32 i = 0; i < TargetActorArray.Num(); ++i)
			{
				if (TargetActorArray[i].IsValid())
				{
					FVector Direction = (TargetActorArray[i].Get()->GetActorLocation() - ReturnTransform.GetLocation()).GetSafeNormal();
					if (Direction.IsNormalized())
					{
						ReturnTransform.SetRotation(Direction.Rotation().Quaternion());
						break;
					}
				}
			}
			return ReturnTransform;
		}
		else if(TargetDataType == ETargetDataType::HitResult)
		{
			return FTransform((HitResult.TraceEnd - HitResult.TraceStart).Rotation(), HitResult.TraceStart);
		}

		return Super::GetOrigin();
	}

	// -------------------------------------

	virtual bool HasEndPoint() const override
	{
		if(TargetDataType == ETargetDataType::ActorArray)
		{
			//We have an endpoint if we have at least one valid actor in our target array
			for (int32 i = 0; i < TargetActorArray.Num(); ++i)
			{
				if (TargetActorArray[i].IsValid())
				{
					return true;
				}
			}
		
			return false;
		}
		else if(TargetDataType == ETargetDataType::HitResult)
		{
			return true;
		}
		
		return Super::HasEndPoint();
	}

	virtual FVector GetEndPoint() const override
	{
		if(TargetDataType == ETargetDataType::ActorArray)
		{
			for (int32 i = 0; i < TargetActorArray.Num(); ++i)
			{
				if (TargetActorArray[i].IsValid())
				{
					return TargetActorArray[i].Get()->GetActorLocation();
				}
			}
		}
		else if(TargetDataType == ETargetDataType::HitResult)
		{
			return HitResult.Location;
		}
		
		return Super::GetEndPoint();
	}

	virtual void ReplaceHitWith(AActor* NewHitActor, const FHitResult* NewHitResult) override
	{
		bHitReplaced = true;

		HitResult = FHitResult();
		if (NewHitResult != nullptr)
		{
			HitResult = *NewHitResult;
		}
	}

	// -------------------------------------

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGameplayAbilityTargetData_ActorsOrHitResult::StaticStruct();
	}

	virtual FString ToString() const override
	{
		return FString::Printf(TEXT("FGameplayAbilityTargetData_ActorsOrHitResult: SkillId=%d, Origin=%s"), 
			MagicRow.SkillId, *GetOrigin().GetLocation().ToString());
	}

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_ActorsOrHitResult> : public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_ActorsOrHitResult>
{
	enum
	{
		WithNetSerializer = true	// For now this is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	};
};