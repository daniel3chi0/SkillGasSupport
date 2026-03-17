// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Werewolf_PassiveAbility.h"

#include "AbilitySystemComponent.h"


void UWerewolf_PassiveAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
	if (bActivateAbilityOnGranted)
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}
