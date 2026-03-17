// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Werewolf_AttributeSet.generated.h"


	#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class WESTERN_MAP_API UWerewolf_AttributeSet : public UAttributeSet
{
	GENERATED_BODY()


public:
	UWerewolf_AttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	//血量
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_Health , Category = "Vital Attributes")
	FGameplayAttributeData Health;
	//为此属性添加方便可调用的宏
	ATTRIBUTE_ACCESSORS(UWerewolf_AttributeSet,Health);
	

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_MaxHealth , Category = "Vital Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UWerewolf_AttributeSet,MaxHealth);
	
	
	//魔法
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_Mana , Category = "Vital Attributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UWerewolf_AttributeSet,Mana);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_MaxMana , Category = "Vital Attributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UWerewolf_AttributeSet,MaxMana);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_ManaRegenRate, Category = "Vital Attributes")
	FGameplayAttributeData ManaRegenRate;
	ATTRIBUTE_ACCESSORS(UWerewolf_AttributeSet, ManaRegenRate);

	//精神
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_Spirit , Category = "Vital Attributes")
	FGameplayAttributeData Spirit;
	ATTRIBUTE_ACCESSORS(UWerewolf_AttributeSet,Spirit);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_MaxSpirit , Category = "Vital Attributes")
	FGameplayAttributeData MaxSpirit;
	ATTRIBUTE_ACCESSORS(UWerewolf_AttributeSet,MaxSpirit);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SpiritRegenRate, Category = "Vital Attributes")
	FGameplayAttributeData SpiritRegenRate;
	ATTRIBUTE_ACCESSORS(UWerewolf_AttributeSet, SpiritRegenRate);

	//韧性
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_Toughness , Category = "Vital Attributes")
	FGameplayAttributeData Toughness;
	ATTRIBUTE_ACCESSORS(UWerewolf_AttributeSet,Toughness);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_MaxToughness , Category = "Vital Attributes")
	FGameplayAttributeData MaxToughness;
	ATTRIBUTE_ACCESSORS(UWerewolf_AttributeSet,MaxToughness);
	

	//护甲
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_Armor , Category = "Vital Attributes")
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UWerewolf_AttributeSet,Armor);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_MaxArmor , Category = "Vital Attributes")
	FGameplayAttributeData MaxArmor;
	ATTRIBUTE_ACCESSORS(UWerewolf_AttributeSet,MaxArmor);
	
	//虚血

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_DownedHealth , Category = "Vital Attributes")
	FGameplayAttributeData DownedHealth;
	//为此属性添加方便可调用的宏
	ATTRIBUTE_ACCESSORS(UWerewolf_AttributeSet,DownedHealth);
	

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_MaxDownedHealth , Category = "Vital Attributes")
	FGameplayAttributeData MaxDownedHealth;
	ATTRIBUTE_ACCESSORS(UWerewolf_AttributeSet,MaxDownedHealth);

	//移动速度
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_MoveSpeed, Category = "Vital Attributes")
	FGameplayAttributeData MoveSpeed;
	//为此属性添加方便可调用的宏
	ATTRIBUTE_ACCESSORS(UWerewolf_AttributeSet,MoveSpeed);
	
	//最大移动速度
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_MaxMoveSpeed , Category = "Vital Attributes")
	FGameplayAttributeData MaxMoveSpeed;
	ATTRIBUTE_ACCESSORS(UWerewolf_AttributeSet,MaxMoveSpeed);
	
	//最小移动速度
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MinMoveSpeed, Category = "Vital Attributes")
	FGameplayAttributeData MinMoveSpeed;
	ATTRIBUTE_ACCESSORS(UWerewolf_AttributeSet, MinMoveSpeed);

	//角色基础攻击力
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attack, Category = "Vital Attributes")
	FGameplayAttributeData Attack;
	ATTRIBUTE_ACCESSORS(UWerewolf_AttributeSet, Attack);


	//Physical 体力
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Physical, Category = "Vital Attributes")
	FGameplayAttributeData Physical;
	//为此属性添加方便可调用的宏
	ATTRIBUTE_ACCESSORS(UWerewolf_AttributeSet, Physical);


	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxPhysical, Category = "Vital Attributes")
	FGameplayAttributeData MaxPhysical;
	ATTRIBUTE_ACCESSORS(UWerewolf_AttributeSet, MaxPhysical);

	//减伤率
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_InjuryReduction, Category = "Vital Attributes")
	FGameplayAttributeData InjuryReduction;
	//为此属性添加方便可调用的宏
	ATTRIBUTE_ACCESSORS(UWerewolf_AttributeSet, InjuryReduction);


	UFUNCTION()
	void onRep_Health(const FGameplayAttributeData& OldHealth) const;

	UFUNCTION()
	void onRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;

	UFUNCTION()
	void onRep_Mana(const FGameplayAttributeData& OldMana) const;

	UFUNCTION()
	void onRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;

	UFUNCTION()
	void OnRep_ManaRegenRate(const FGameplayAttributeData& OldManaRegenRate) const;
	
	UFUNCTION()
	void onRep_Spirit(const FGameplayAttributeData& OldSpirit) const;

	UFUNCTION()
	void onRep_MaxSpirit(const FGameplayAttributeData& OldMaxSpirit) const;

	UFUNCTION()
	void OnRep_SpiritRegenRate(const FGameplayAttributeData& OldSpiritRegenRate) const;

	UFUNCTION()
	void onRep_Toughness(const FGameplayAttributeData& OldToughness) const;

	UFUNCTION()
	void onRep_MaxToughness(const FGameplayAttributeData& OldMaxToughness) const;

	UFUNCTION()
	void onRep_Armor(const FGameplayAttributeData& OldArmor) const;

	UFUNCTION()
	void onRep_MaxArmor(const FGameplayAttributeData& OldMaxArmor) const;

	UFUNCTION()
	void onRep_DownedHealth(const FGameplayAttributeData& OldDownedHealth) const;

	UFUNCTION()
	void onRep_MaxDownedHealth(const FGameplayAttributeData& OldMaxDownedHealth) const;

	UFUNCTION()
	void onRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed) const;

	UFUNCTION()
	void onRep_MaxMoveSpeed(const FGameplayAttributeData& OldMaxMoveSpeed) const;

	UFUNCTION()
	void onRep_MinMoveSpeed(const FGameplayAttributeData& OldMinMoveSpeed) const;
	
	UFUNCTION()
	void OnRep_Attack(const FGameplayAttributeData& OlAttack) const;


	UFUNCTION()
	void OnRep_Physical(const FGameplayAttributeData& OlPhysical) const;

	UFUNCTION()
	void OnRep_MaxPhysical(const FGameplayAttributeData& OlMaxPhysical) const;

	UFUNCTION()
	void OnRep_InjuryReduction(const FGameplayAttributeData& OlInjuryReduction) const;


	UFUNCTION(BlueprintCallable)
	void InitAttributeSet();

	UFUNCTION(BlueprintCallable)
	void RecoverAttributeSet();

	UFUNCTION(BlueprintCallable)
	void SetEnemyAttribute(float Heal,float Arm);
};

