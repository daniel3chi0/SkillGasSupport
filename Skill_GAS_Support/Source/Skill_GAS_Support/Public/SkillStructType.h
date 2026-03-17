// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "SkillStructType.generated.h"

class UGameplayEffect;
class UGameplayAbility;

UENUM(BlueprintType)
enum class ESkillObtainType : uint8
{
	None			UMETA(DisplayName = "None"),
	StaticSkill		UMETA(DisplayName = "初始拥有（配置项）"),
	DynamicSkill	UMETA(DisplayName = "游戏中学习（动态项）"),
};

UENUM(BlueprintType)
enum class ESkillGameType : uint8
{
	None			UMETA(DisplayName = "None"),
	BattleSkill		UMETA(DisplayName = "战技"),
	Magic			UMETA(DisplayName = "魔法"),
};

UENUM(BlueprintType)
enum class ESkillEffectType : uint8
{
	None				UMETA(DisplayName = "None"),
	RangeEffect			UMETA(DisplayName = "范围效果AOE"),
	Placement			UMETA(DisplayName = "放置类"),
	BlockRay			UMETA(DisplayName = "受阻挡的射线"),
	//RangeEffectOrSelf	UMETA(DisplayName = "范围效果或自己类"),
	Reconnoitre			UMETA(DisplayName = "侦察类型"),
	SelfEffect			UMETA(DisplayName = "自身效果"),
};

UENUM(BlueprintType)
enum class EPlayerCareer : uint8
{
	None			UMETA(DisplayName = "None"),
	Samurai			UMETA(DisplayName = "武士"),
	Artist			UMETA(DisplayName = "墨者"),
	Priest			UMETA(DisplayName = "牧师"),
	Paladin			UMETA(DisplayName = "圣骑士"),
	Archer			UMETA(DisplayName = "射手"),
	Magician		UMETA(DisplayName = "魔法师"),
	Prophet			UMETA(DisplayName = "预言家"),
	Windtalkers		UMETA(DisplayName = "风语者"),
	Assassin		UMETA(DisplayName = "刺客"),
	Prayer			UMETA(DisplayName = "祈愿者"),
	General			UMETA(DisplayName = "将军"),
	NineTailedFox	UMETA(DisplayName = "九尾狐"),
	Alcoholic		UMETA(DisplayName = "酒鬼"),
};

UENUM(BlueprintType)
enum class ETargetType : uint8
{
	None				UMETA(DisplayName = "None"),
	Self				UMETA(DisplayName = "自身"),
	MultiOther			UMETA(DisplayName = "多个其他玩家"),
	SingleOther			UMETA(DisPlayName = "单个其他玩家"),
	MultiOtherOrSelf	UMETA(DisPlayName = "多个其他玩家或自身"),
	SingleOtherOrSelf	UMETA(DisplayName = "单个其他玩家或自身"),
	RayPoint			UMETA(DisplayName = "射线击中的点位"),
};

UENUM(BlueprintType)
enum class EUseLimit : uint8
{
	None					UMETA(DisplayName = "None"),
	HadWeapon				UMETA(DisplayName = "武器"),
	HadAttribute			UMETA(DisplayName = "属性"),
	HadWeaponAndAttribute	UMETA(DisplayName = "有武器和属性"),
};

UENUM(BlueprintType)
enum class EVillainExtraBattleSkillSlotIndex : uint8
{
	None		UMETA(DisplayName = "非狼人额外战技"),
	SlotOne		UMETA(DisplayName = "狼人额外战技槽位1"),
	SlotTwo		UMETA(DisplayName = "狼人额外战技槽位2"),
};

UENUM(BlueprintType)
enum class EOwnerFaction : uint8
{
	Neutral UMETA(DisplayName = "Neutral Guy"), //中立身份
	Good UMETA(DisplayName = "Good Guy"),    // 好人身份
	Bad UMETA(DisplayName = "Werewolf Guy")      // 狼人身份

};

USTRUCT(BlueprintType)
struct FSkillRowStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	int32 SkillId = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	TArray<TSoftClassPtr<UGameplayAbility>> AbilityClasses;
	//TSoftClassPtr<UGameplayAbility> AbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	FText SkillName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	ESkillObtainType SkillObtainType = ESkillObtainType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	ESkillGameType SkillGameType = ESkillGameType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	EOwnerFaction ExclusiveFaction = EOwnerFaction::Neutral;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	ESkillEffectType SkillEffectType = ESkillEffectType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	FText SkillDetail;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	ETargetType TargetType = ETargetType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill" , meta=(Units = "s"))
	float CoolDownTime = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	EUseLimit UseLimit = EUseLimit::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	int32 MaxStackCount = 1;

	/*FSkillRowStruct():
	SkillId(0),AbilityClass(nullptr),SkillName(FText()),SkillObtainType(ESkillObtainType::None),SkillEffectType(ESkillEffectType::None),
	SkillDetail(FText()),TargetType(ETargetType::None),CoolDownTime(0),UseLimit(EUseLimit::None),MaxStackCount(1)
	{
	}*/

	// 重载 == 操作符
	bool operator==(const FSkillRowStruct& Other) const
	{
		return SkillId == Other.SkillId;
	}
	// 声明 GetTypeHash 为 friend 函数
	friend uint32 GetTypeHash(const FSkillRowStruct& CustomStruct)
	{
		return GetTypeHash(CustomStruct.SkillId);
	}
	
	bool IsValid() const
	{
		return SkillId > 0;
	}
};

USTRUCT(BlueprintType)
struct FLearnedMagicDataWrap
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MagicAbilityId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAbilitySpecHandle GASpecHandle;

	bool operator==(const FLearnedMagicDataWrap& Other) const
	{
		return (MagicAbilityId == Other.MagicAbilityId) && (GASpecHandle == Other.GASpecHandle);
	}
};

USTRUCT(BlueprintType)
struct FBattleSkillRowStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BattleSkill")
	int32 SkillId = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BattleSkill")
	EPlayerCareer Career = EPlayerCareer::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BattleSkill")
	EVillainExtraBattleSkillSlotIndex ExtraBattleSkillSlotIndex = EVillainExtraBattleSkillSlotIndex::None;

	bool IsValid() const
	{
		return SkillId > 0;
	}
};

UENUM(BlueprintType)
enum class EMagicType: uint8
{
	Normal		UMETA(DisplayName = "普通魔法"),
	Dark		UMETA(DisplayName = "黑魔法"),
};

UENUM(BlueprintType)
enum class EMagicReleaseType: uint8
{
	HoldHigh		UMETA(DisplayName = "高举类"),
	Straight		UMETA(DisplayName = "直线类"),
	NormalLocked	UMETA(DisplayName = "普通瞄准类"),
	GroundPoint		UMETA(DisplayName = "选点类"),
	PerspectiveLocked	UMETA(DisplayName = "透视瞄准类"),
	Special			UMETA(DisplayName = "特殊类"),
};

USTRUCT(BlueprintType)
struct FMagicRowStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Magic")
	int32 SkillId = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Magic")
	EMagicType MagicType = EMagicType::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Magic")
	EMagicReleaseType MagicReleaseType = EMagicReleaseType::HoldHigh;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Magic")
	float ChantTime = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Magic")
	float CostManaValue = 0;

	bool IsValid() const
	{
		return SkillId > 0;
	}
	
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << SkillId;
		Ar << MagicType;
		Ar << MagicReleaseType;
		Ar << ChantTime;
		bOutSuccess = true;
		return true;
	}
};

template<>
struct TStructOpsTypeTraits<FMagicRowStruct> : public TStructOpsTypeTraitsBase2<FMagicRowStruct>
{
	enum
	{
		WithNetSerializer = true,
	};
};

USTRUCT(BlueprintType)
struct FMagicActionRowStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	TArray<TSoftClassPtr<UGameplayAbility>> AbilityClasses;
	//TSoftClassPtr<UGameplayAbility> AbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	FText MagicActionName;
};

USTRUCT(BlueprintType)
struct FInitialBuffRowStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	EPlayerCareer Career;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	TArray<TSoftClassPtr<UGameplayEffect>> InitBuffs;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	FText DetailInfo;
};
