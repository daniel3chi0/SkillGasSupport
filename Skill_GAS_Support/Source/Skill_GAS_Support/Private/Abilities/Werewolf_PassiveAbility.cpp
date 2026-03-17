// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/SGSPassiveAbility.h"
#include "AbilitySystemComponent.h"


void USGSPassiveAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
	if (bActivateAbilityOnGranted)
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}
