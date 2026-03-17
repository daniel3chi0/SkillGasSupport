// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/WerewolfAbility_LeftAttack.h"

#include "AbilitySystemComponent.h"


void UWerewolfAbility_LeftAttack::InputPressed(const FGameplayAbilitySpecHandle Handle,
                                               const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	if(CheckCooldown(Handle,ActorInfo))
	{
		
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Handle,true);
		
	}


}




void UWerewolfAbility_LeftAttack::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
		SpecHandle.Data.Get()->DynamicGrantedTags.AppendTags(CooldownTags);
		if(AttackOrder == true)
		{
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName( "Cooldown" )), CooldownDuration_01.GetValueAtLevel(GetAbilityLevel()));
		}
		else
		{
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName( "Cooldown" )), CooldownDuration_02.GetValueAtLevel(GetAbilityLevel()));
		}
	
		
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}
