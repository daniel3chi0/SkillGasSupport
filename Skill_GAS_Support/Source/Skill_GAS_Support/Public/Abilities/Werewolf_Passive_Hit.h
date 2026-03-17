// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Werewolf_PassiveAbility.h"
#include "Werewolf_Passive_Hit.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FHitDynamicData
{
	GENERATED_BODY()

	FHitDynamicData()
		: ImpactStrength(0.f)
		, Damage(0.f)
		,TrueDamage(0.f)
		, HitLocation(FVector::ZeroVector)
		, HitActor(nullptr)
		, Attacker(nullptr)
		, Victim(nullptr)
	{}

	FHitDynamicData(float InImpactStrength, float InDamage,float InTrueDamage,FVector InHitLocation, AActor* InHitActor,AActor* InAttacker,AActor* InVictim)
		: ImpactStrength(InImpactStrength)
		, Damage(InDamage)
		,TrueDamage(InTrueDamage)
		, HitLocation(InHitLocation)
		, HitActor(InHitActor)
		, Attacker(InAttacker)
		, Victim(InVictim)
	{}

	// 正常伤害
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitData")
	float ImpactStrength;

	// 伤害
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitData")
	float Damage;
	//真实伤害
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitData")
	float TrueDamage;
	// 伤害发生地点
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitData")
	FVector HitLocation;

	// 打击物,实施打击的道具等
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitData")
	AActor* HitActor;

	//攻击者
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitData")
	AActor* Attacker;

	//受击者
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitData")
	AActor* Victim;

	// 序列化函数
	friend FArchive& operator<<(FArchive& Ar, FHitDynamicData& Data);
	
	// // 序列化函数
	// bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
	// {
	// 	// 使用标准的序列化方法来处理简单的类型
	// 	Ar << ImpactStrength;
	// 	Ar << Damage;
	// 	Ar << HitLocation;
	// 	Ar << HitActor;
	// 	Ar << Attacker;
	// 	Ar << Victim;
 //        
	// 	bOutSuccess = true;
	// 	return true;
	// }
	//
	// // 网络反序列化
	// void Deserialize(FArchive& Ar)
	// {
	// 	Ar << ImpactStrength;
	// 	Ar << Damage;
	// 	Ar << HitLocation;
	// 	Ar << HitActor;
	// 	Ar << Attacker;
	// 	Ar << Victim;
	// }
	//
	// // 反序列化之后可以打印出数据帮助调试
	// FString ToString() const
	// {
	// 	return FString::Printf(TEXT("NormalDamage: %.2f, TrueDamage: %.2f, Location: %s"),
	// 		ImpactStrength, Damage, *HitLocation.ToString());
	// }
};

USTRUCT(BlueprintType)
struct FGameplayAbilityTargetData_Hit : public FGameplayAbilityTargetData
{
	GENERATED_BODY()

public:
	FGameplayAbilityTargetData_Hit()
	{ }

	// 带参构造函数，接收一个 FHitData
	FGameplayAbilityTargetData_Hit(const FHitDynamicData& InHitData)
		: HitData(InHitData)
	{ }

	// 存储 Hit 信息
	UPROPERTY()
	FHitDynamicData HitData;

	// 网络序列化函数
	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) 
	{
		Ar << HitData.ImpactStrength;
		Ar << HitData.Damage;
		Ar << HitData.TrueDamage;
		Ar << HitData.HitLocation;
		Ar << HitData.HitActor;
		Ar << HitData.Attacker;
		Ar << HitData.Victim;

		bOutSuccess = true;
		return true;
	}

	// 返回当前结构体对应的 ScriptStruct
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGameplayAbilityTargetData_Hit::StaticStruct();
	}

	// 转换为字符串，便于调试
	virtual FString ToString() const override
	{
		return FString::Printf(TEXT("FGameplayAbilityTargetData_Hit: NormalDamage=%.2f, TrueDamage=%.2f, Location=%s"),
			HitData.ImpactStrength,
			HitData.Damage,
			*HitData.HitLocation.ToString());
	}
};

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_Hit> : public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_Hit>
{
	enum
	{
		WithNetSerializer = true  // 启用网络序列化
	};
};

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	Normal UMETA(DisplayName = "Normal"),
	Invincible UMETA(DisplayName = "Invincible"),
	Tyrants UMETA(DisplayName = "Tyrants"),
};

// 自定义的 GameplayEffectContext
USTRUCT(BlueprintType)
struct FHitEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:
	FHitEffectContext() 
		: HitDynamicData()  // 默认构造函数
	{}

	// 返回当前结构体对应的 ScriptStruct
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGameplayAbilityTargetData_Hit::StaticStruct();
	}

	// 网络序列化函数
	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) 
	{
		Ar << HitDynamicData.ImpactStrength;
		Ar << HitDynamicData.Damage;
		Ar << HitDynamicData.TrueDamage;
		Ar << HitDynamicData.HitLocation;
		Ar << HitDynamicData.HitActor;
		Ar << HitDynamicData.Attacker;
		Ar << HitDynamicData.Victim;

		bOutSuccess = true;
		return true;
	}
	
	
	// 设置伤害数据
	void SetHitDynamicData(const FHitDynamicData& InHitDynamicData)
	{
		HitDynamicData = InHitDynamicData;
	}

	// 获取伤害数据
	const FHitDynamicData& GetDamageData() const
	{
		return HitDynamicData;
	}

	

private:
	// 伤害数据结构体
	UPROPERTY()
	FHitDynamicData HitDynamicData;

};
template<>
struct TStructOpsTypeTraits<FHitEffectContext> : public TStructOpsTypeTraitsBase2<FHitEffectContext>
{
	enum
	{
		WithNetSerializer = true  // 启用网络序列化
	};
};
UCLASS()
class WESTERN_MAP_API UWerewolf_Passive_Hit : public UWerewolf_PassiveAbility
{
	GENERATED_BODY()
	
	
	
protected:
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="GE")
	TSubclassOf<UGameplayEffect>  AchievedEffect;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Anim")
	TArray<TObjectPtr<UAnimMontage>> HitAnimMontages;

	//�ܵ�������Actorʩ��һ��GEЧ��
	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* OwnActor, AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass,FHitDynamicData InHitData,const FGameplayTag GameplayCueTag);

	
	// 根据角色的 Gameplay Tags 来确定角色状态
	UFUNCTION(BlueprintCallable, Category = "Character State")
	ECharacterState GetCharacterBuff() const;

	//验证是否执行此GA
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable, Category = "Verify")
	bool VerifyExecute(AActor* Attacker);

	FGameplayCueParameters GameplayCueParameters; 
};
