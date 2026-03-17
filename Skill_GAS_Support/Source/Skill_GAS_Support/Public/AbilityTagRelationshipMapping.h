#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AbilityTagRelationshipMapping.generated.h"


USTRUCT()
struct FAbilityTagRelationship
{
	GENERATED_BODY()
	
	//Relationship的Tag，一个能力可能会有多个这个Tag
	UPROPERTY(EditAnywhere, Category = Ability, meta = (Categories = "Gameplay.Action"))
	FGameplayTag AbilityTag;
	
	//其他拥有这些tags的能力会被block(AbilityTag)
	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer AbilityTagsToBlock;
	
	//其他拥有这些tags的能力会被取消(AbilityTag)
	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer AbilityTagsToCancel;
	
	//本能力激活所需要的tags，会被隐式加入GA中
	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer ActivationRequiredTags;
	
	//本能力会被阻挡激活如果拥有这些tags，会被隐式加入GA中
	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer ActivationBlockedTags;
};

UCLASS()
class UAbilityTagRelationshipMapping : public UDataAsset
{
	GENERATED_BODY()
	
private:
	
	UPROPERTY(EditAnywhere, Category = Ability, meta=(TitleProperty="AbilityTag"))
	TArray<FAbilityTagRelationship> AbilityTagRelationships;

public:
	//填充能力的排斥tag
	void GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutTagsToBlock, FGameplayTagContainer* OutTagsToCancel) const;

	//填充能力激活所需条件tag
	void GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutActivationRequired, FGameplayTagContainer* OutActivationBlocked) const;

	//拥有这些Tags的能力被传入的ActionTag取消时会返回true
	bool IsAbilityCancelledByTag(const FGameplayTagContainer& AbilityTags, const FGameplayTag& ActionTag) const;
};
