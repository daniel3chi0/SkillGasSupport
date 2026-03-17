// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Werewolf_Passive_Hit.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"
#include "Component/Werewolf_CombatComponent.h"


void UWerewolf_Passive_Hit::ApplyEffectToTarget(AActor* OwnActor, AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass,FHitDynamicData InHitData,const FGameplayTag GameplayCueTag)
{
	//获取技能系统
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC == nullptr) return;
	
	check(GameplayEffectClass);
	
	FHitEffectContext* EffectContext = new FHitEffectContext();
	EffectContext->SetHitDynamicData(InHitData);
	
	//	TargetASC->MakeEffectContext();
	//添加游戏效果源对象
	EffectContext->AddSourceObject(OwnActor);
	EffectContext->AddInstigator(OwnActor,OwnActor);
	//FGameplayEffectContext* GameplayEffectContext = StaticCast<FGameplayEffectContext*>(EffectContext);
	FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1.f, FGameplayEffectContextHandle(EffectContext));
	
	// 检查 EffectSpecHandle 是否有效
	if (!EffectSpecHandle.IsValid() || !EffectSpecHandle.Data.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create a valid EffectSpecHandle!"));
		delete EffectContext;// 确保释放内存
		return;
	}
	//
	//GameplayEffectContextHandle =FGameplayEffectContextHandle(EffectContext);
		
	//应用游戏效果到TargetASC自身,因此可以在蓝图中选择执行效果的TargetActor
	TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	/*
	 *以下是GC的使用 ,因为参数问题暂时废弃
	 * 
	 */
	//TargetASC->ExecuteGameplayCue(GameplayCueTag,GameplayEffectContextHandle);
	 // FHitEffectContext* EffectContext2 = new FHitEffectContext();
	 // EffectContext2->SetHitDynamicData(InHitData);
	 //EffectContext2->AddSourceObject(OwnActor);
    // EffectContext2->AddInstigator(OwnActor,OwnActor);
 
	 // if (!TargetActor)
	 // {
	 // 	return;
	 // }
	 // GameplayCueParameters = FGameplayCueParameters();
	 // GameplayCueParameters.EffectContext = FGameplayEffectContextHandle(EffectContext2);
	 //
	 // 	// The actor has an ability system so the event will fire on authority only and will be replicated.
	 // if (TargetActor->GetLocalRole() == ROLE_Authority)
	 // {
	 // 	TargetASC->ExecuteGameplayCue(GameplayCueTag, GameplayCueParameters);
	 // }
	 // else
	 // {
	 // 	// The actor does not have an ability system so the event will only be fired locally.
	 // 	UGameplayCueManager::ExecuteGameplayCue_NonReplicated(TargetActor, GameplayCueTag, GameplayCueParameters);
	 // }
	
}

ECharacterState UWerewolf_Passive_Hit::GetCharacterBuff() const
{
	if(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo())->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Buff.Invincible"))))
	{
		return ECharacterState::Invincible;
	}

	if(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo())->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Buff.Tyrants"))))
	{
		return ECharacterState::Tyrants;
	}

	return ECharacterState::Normal;

}

bool UWerewolf_Passive_Hit::VerifyExecute_Implementation(AActor* Attacker)
{
	//后续会根据是否有别的条件进行修改
	if(UWerewolf_CombatComponent* AttackerCombatComponent = UWerewolf_CombatComponent::GetActorCombatComponent(Attacker))
	{
		return 	AttackerCombatComponent->bInAlreadyHitActors(GetAvatarActorFromActorInfo());
	}
	
	return false;
}

FArchive& operator<<(FArchive& Ar, FHitDynamicData& Data)
{
		// 使用标准的序列化方法来处理简单的类型
	 	Ar << Data.ImpactStrength;
	 	Ar << Data.Damage;
	 	Ar << Data.HitLocation;
	 	Ar << Data.HitActor;
	 	Ar << Data.Attacker;
	 	Ar << Data.Victim;
		return Ar;
}