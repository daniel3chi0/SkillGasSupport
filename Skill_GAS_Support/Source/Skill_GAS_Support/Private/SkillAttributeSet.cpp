// Fill out your copyright notice in the Description page of Project Settings.
#include "SkillAttributeSet.h"
#include "Net/UnrealNetwork.h"

USkillAttributeSet::USkillAttributeSet()
{
	InitBattleSkillCDScaleFactor(1.f);
}

void USkillAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(USkillAttributeSet, BattleSkillCDScaleFactor, COND_None, REPNOTIFY_Always);
}

void USkillAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void USkillAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

void USkillAttributeSet::OnRep_BattleSkillCDScaleFactor(const FGameplayAttributeData& OldBattleSkillCDScaleFactor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USkillAttributeSet, BattleSkillCDScaleFactor, OldBattleSkillCDScaleFactor);
}
