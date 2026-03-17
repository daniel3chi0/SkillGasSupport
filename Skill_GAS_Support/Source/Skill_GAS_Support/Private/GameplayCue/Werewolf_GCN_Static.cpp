// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GameplayCue/Werewolf_GCN_Static.h"

#include "AbilitySystem/Abilities/Werewolf_Passive_Hit.h"


FHitDynamicData UWerewolf_GCN_Static::GetHitEffectContext(const FGameplayCueParameters& Parameters)
{
	// 获取 EffectContext 的原始指针
	const FGameplayEffectContext* RawContext = Parameters.EffectContext.Get();

	// 确保 EffectContext 是 FHitEffectContext 类型
	if (const FHitEffectContext* HitEffectContext = static_cast<const FHitEffectContext*>(RawContext))
	{
		// 如果转换成功，则返回数据
		return HitEffectContext->GetDamageData();
	}

	// 如果转换失败，可以处理错误或返回一个默认值
	UE_LOG(LogTemp, Warning, TEXT("Failed to cast EffectContext to FHitEffectContext!"));
	return FHitDynamicData(); // 返回一个默认构造的 FHitDynamicData
}
