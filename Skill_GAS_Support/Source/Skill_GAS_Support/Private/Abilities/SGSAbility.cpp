// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/SGSAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemLog.h"
#include "Abilities/Tasks/AbilityTask_WaitAbilityActivate.h"
#include "SkillAttributeSet.h"
#include "SkillSystemFunctionLibrary.h"
#include "SGSAbilitySystemComponent.h"
#include "Abilities/AdditionalAbilityCost.h"
#include "GameplayEffect/GameplayEffectContainerFunctionLibrary.h"
//#include "Component/PawnExtensionComponent.h"
#include "SkillStructType.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystem//SkillManager.h"
#include "SGSGameplayTags.h"

USGSAbility::USGSAbility()
{
}

/*UAnimConfig* USGSAbility::GetCharaterAnimConfig()
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (Avatar)
	{
		if(UPawnExtensionComponent* PEC = Avatar->GetComponentByClass<UPawnExtensionComponent>())
		{
			if(const UCharacterConfig* CharacterConfig = PEC->GetCharacterData<UCharacterConfig>())
			{
				return CharacterConfig->AnimConfig;
			}
		}
	}

	return nullptr;
}*/

void USGSAbility::SetChooseNum_Implementation(FScalableFloat InNum)
{
	//仅仅修改了服务器的CDO
	AbilityChooseNum = InNum;
	
}

bool USGSAbility::SetChooseNum_Validate(FScalableFloat InNum)
{
	return true;
}

const FGameplayTagContainer* USGSAbility::GetCooldownTags() const
{
	FGameplayTagContainer* MutableTags = const_cast<FGameplayTagContainer*>(&TempCooldownTags);
	MutableTags->Reset(); // MutableTags writes to the TempCooldownTags on the CDO so clear it in case the ability cooldown tags change (moved to a different slot)
	const FGameplayTagContainer* ParentTags = Super::GetCooldownTags();
	if (ParentTags)
	{
		MutableTags->AppendTags(*ParentTags);
	}
	MutableTags->AppendTags(CooldownTags);
	return MutableTags;
	
}

void USGSAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
		SpecHandle.Data.Get()->DynamicGrantedTags.AppendTags(CooldownTags);
		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
		float TempBattleSkillCDScaleFactor = 1.f;
		if(ASC)
		{
			if (const USkillAttributeSet* SkillAttributes = ASC->GetSet<USkillAttributeSet>())
			{
				float BattleSkillCDScaleFactor = SkillAttributes->GetBattleSkillCDScaleFactor();
				if (CooldownTags.HasTagExact(FGameplayTag::RequestGameplayTag(FName("Cooldown.Skill.BattleSkill"))))
				TempBattleSkillCDScaleFactor *= BattleSkillCDScaleFactor;
			}
		}

		float CooldownValue = -1;
		UGameInstance* GameInstance = GetWorld()->GetGameInstance();
		if(GameInstance)
		{
			USkillManager* SkillManager = GameInstance->GetSubsystem<USkillManager>();
			if(SkillManager)
			{
				CooldownValue = SkillManager->GetSkillConfigCooldownValueById(SkillId);
			}
		}
		
		CooldownValue = CooldownValue > 0 ? CooldownValue : CooldownDuration.GetValueAtLevel(GetAbilityLevel());
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName( "Cooldown" )), CooldownValue * TempBattleSkillCDScaleFactor);
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}

FGameplayAbilityTargetDataHandle USGSAbility::MakeGameplayAbilityTargetDataHandleFromActorArray(const TArray<AActor*> TargetActors)
{
	if (TargetActors.Num() > 0)
	{
		FGameplayAbilityTargetData_ActorArray* NewData = new FGameplayAbilityTargetData_ActorArray();
		NewData->TargetActorArray.Append(TargetActors);
		return FGameplayAbilityTargetDataHandle(NewData);
	}

	return FGameplayAbilityTargetDataHandle();
}

FGameplayAbilityTargetDataHandle USGSAbility::MakeGameplayAbilityTargetDataHandleFromHitResults(const TArray<FHitResult> HitResults)
{
	FGameplayAbilityTargetDataHandle TargetData;

	for (const FHitResult& HitResult : HitResults)
	{
		FGameplayAbilityTargetData_SingleTargetHit* NewData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
		TargetData.Add(NewData);
	}

	return TargetData;
}

FGameplayAbilityTargetDataHandle USGSAbility::MakeTargetDataFromTargetType(TSubclassOf<UTargetType> TargetType, const FGameplayEventData& EventData)
{
	// 根据目标类型获取目标
	TArray<FHitResult> TargetHitResults;
	TArray<FGameplayAbilityTargetDataHandle> TargetDatas;
	TArray<AActor*> TargetActors;
	UGameplayEffectContainerFunctionLibrary::GetDataFromTargetType(TargetType, EventData, TargetDatas, TargetHitResults, TargetActors);

	FGameplayAbilityTargetDataHandle FinalTargetData;

	for (const FGameplayAbilityTargetDataHandle& TD : TargetDatas)
	{
		FinalTargetData.Append(TD);
	}

	for (const FHitResult& HitResult : TargetHitResults)
	{
		FGameplayAbilityTargetData_SingleTargetHit* NewData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
		FinalTargetData.Add(NewData);
	}

	if (TargetActors.Num() > 0)
	{
		FGameplayAbilityTargetData_ActorArray* NewData = new FGameplayAbilityTargetData_ActorArray();
		NewData->TargetActorArray.Append(TargetActors);
		FinalTargetData.Add(NewData);
	}

	return FinalTargetData;
}

FGameplayEffectContainerSpec USGSAbility::MakeEffectContainerSpecFromContainerMap(FGameplayTag ContainerTag, const FGameplayEventData& EventData,
                                                                        int32 OverrideGameplayLevel)
{
	FGameplayEffectContainer* FoundContainer = EffectContainerMap.Find(ContainerTag);

	if (FoundContainer)
	{
		return UGameplayEffectContainerFunctionLibrary::MakeEffectContainerSpec(*FoundContainer, EventData, OverrideGameplayLevel, this);
	}
	return FGameplayEffectContainerSpec();
}

TArray<FActiveGameplayEffectHandle> USGSAbility::ApplyEffectContainerSpecFromContainerMap(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	FGameplayEffectContainer* FoundContainer = EffectContainerMap.Find(ContainerTag);

	if (FoundContainer)
	{
		const FGameplayEffectContainerSpec Spec = UGameplayEffectContainerFunctionLibrary::MakeEffectContainerSpec(*FoundContainer, EventData, OverrideGameplayLevel, this);
		return UGameplayEffectContainerFunctionLibrary::ApplyEffectContainerSpec(this, Spec);
	}
	return TArray<FActiveGameplayEffectHandle>();
}

FGameplayTag USGSAbility::GetFirstAbilityTriggersEventTag()
{
	for (const FAbilityTriggerData& Data : AbilityTriggers)
	{
		if(Data.TriggerSource == EGameplayAbilityTriggerSource::Type::GameplayEvent)
		{
			return Data.TriggerTag;
		}
	}

	return FGameplayTag();
}

UGameplayEffect* USGSAbility::GetCostGameplayEffect() const
{
	//return Super::GetCostGameplayEffect();
	UGameplayEffect* CostGE = Super::GetCostGameplayEffect();
	if(CostGE)
	{
		if (CostGE->GetAssetTags().HasTag(FGameplayTag::RequestGameplayTag("EffectAssetTag.Cost.AttributeMana")))
		{
			if(CostGE->Modifiers.Num() >= 1)
			{
				FGameplayModifierInfo& ManaModifierInfo = CostGE->Modifiers[0];
				FMagicRowStruct MagicRow = USkillSystemFunctionLibrary::GetMagicInfoBySkillId(GetAvatarActorFromActorInfo(), SkillId);
				ManaModifierInfo.ModifierMagnitude = FScalableFloat(-MagicRow.CostManaValue);
			}
		}
		
		return CostGE;
	}

	return nullptr;
}

bool USGSAbility::CanActivateAbilityReturnMsg(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags, FString& ReturnMsg) const
{
	//***** Copy Base *****
	//return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);

	// Don't set the actor info, CanActivate is called on the CDO

	// A valid AvatarActor is required. Simulated proxy check means only authority or autonomous proxies should be executing abilities.
	AActor* const AvatarActor = ActorInfo ? ActorInfo->AvatarActor.Get() : nullptr;
	if (AvatarActor == nullptr || !ShouldActivateAbility(AvatarActor->GetLocalRole()))
	{
		return false;
	}

	//make into a reference for simplicity
	static FGameplayTagContainer DummyContainer;
	DummyContainer.Reset();

	FGameplayTagContainer& OutTags = OptionalRelevantTags ? *OptionalRelevantTags : DummyContainer;

	// make sure the ability system component is valid, if not bail out.
	UAbilitySystemComponent* const AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
	if (!AbilitySystemComponent)
	{
		return false;
	}

	FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
	if (!Spec)
	{
		ABILITY_LOG(Warning, TEXT("CanActivateAbility %s failed, called with invalid Handle"), *GetName());
		return false;
	}

	if (AbilitySystemComponent->GetUserAbilityActivationInhibited())
	{
		/**
		 *	Input is inhibited (UI is pulled up, another ability may be blocking all other input, etc).
		 *	When we get into triggered abilities, we may need to better differentiate between CanActivate and CanUserActivate or something.
		 *	E.g., we would want LMB/RMB to be inhibited while the user is in the menu UI, but we wouldn't want to prevent a 'buff when I am low health'
		 *	ability to not trigger.
		 *	
		 *	Basically: CanActivateAbility is only used by user activated abilities now. If triggered abilities need to check costs/cooldowns, then we may
		 *	want to split this function up and change the calling API to distinguish between 'can I initiate an ability activation' and 'can this ability be activated'.
		 */ 

		/*if (FScopedCanActivateAbilityLogEnabler::IsLoggingEnabled())
		{
			UE_LOG(LogAbilitySystem, Verbose, TEXT("%s: %s could not be activated due to GetUserAbilityActivationInhibited"), *GetNameSafe(ActorInfo->OwnerActor.Get()), *GetNameSafe(Spec->Ability));
			UE_VLOG(ActorInfo->OwnerActor.Get(), VLogAbilitySystem, Verbose, TEXT("%s could not be activated due to GetUserAbilityActivationInhibited"), *GetNameSafe(Spec->Ability));
		}*/
		return false;
	}
	
	UAbilitySystemGlobals& AbilitySystemGlobals = UAbilitySystemGlobals::Get();

	if (!AbilitySystemGlobals.ShouldIgnoreCooldowns() && !CheckCooldown(Handle, ActorInfo, OptionalRelevantTags))
	{
		/*if (FScopedCanActivateAbilityLogEnabler::IsLoggingEnabled())
		{
			UE_LOG(LogAbilitySystem, Verbose, TEXT("%s: %s could not be activated due to Cooldown"), *GetNameSafe(ActorInfo->OwnerActor.Get()), *GetNameSafe(Spec->Ability));
			UE_VLOG(ActorInfo->OwnerActor.Get(), VLogAbilitySystem, Verbose, TEXT("%s could not be activated due to Cooldown"), *GetNameSafe(Spec->Ability));
		}*/
		ReturnMsg = FString("Cooldown Failed");
		return false;
	}

	if (!AbilitySystemGlobals.ShouldIgnoreCosts() && !CheckCost(Handle, ActorInfo, OptionalRelevantTags))
	{
		/*if (FScopedCanActivateAbilityLogEnabler::IsLoggingEnabled())
		{
			UE_LOG(LogAbilitySystem, Verbose, TEXT("%s: %s could not be activated due to Cost"), *GetNameSafe(ActorInfo->OwnerActor.Get()), *GetNameSafe(Spec->Ability));
			UE_VLOG(ActorInfo->OwnerActor.Get(), VLogAbilitySystem, Verbose, TEXT("%s could not be activated due to Cost"), *GetNameSafe(Spec->Ability));
		}*/
		ReturnMsg = FString("Cost Failed");
		return false;
	}

	if (!DoesAbilitySatisfyTagRequirementsWithMsg(*AbilitySystemComponent, SourceTags, TargetTags, OptionalRelevantTags, &ReturnMsg))
	{	// If the ability's tags are blocked, or if it has a "Blocking" tag or is missing a "Required" tag, then it can't activate.
		/*if (FScopedCanActivateAbilityLogEnabler::IsLoggingEnabled())
		{
			UE_LOG(LogAbilitySystem, Verbose, TEXT("%s: %s could not be activated due to Blocking Tags or Missing Required Tags"), *GetNameSafe(ActorInfo->OwnerActor.Get()), *GetNameSafe(Spec->Ability));
			UE_VLOG(ActorInfo->OwnerActor.Get(), VLogAbilitySystem, Verbose, TEXT("%s could not be activated due to Blocking Tags or Missing Required Tags"), *GetNameSafe(Spec->Ability));
		}*/
		return false;
	}

	// Check if this ability's input binding is currently blocked
	if (AbilitySystemComponent->IsAbilityInputBlocked(Spec->InputID))
	{
		/*if (FScopedCanActivateAbilityLogEnabler::IsLoggingEnabled())
		{
			UE_LOG(LogAbilitySystem, Verbose, TEXT("%s: %s could not be activated due to blocked input ID %d"), *GetNameSafe(ActorInfo->OwnerActor.Get()), *GetNameSafe(Spec->Ability), Spec->InputID);
			UE_VLOG(ActorInfo->OwnerActor.Get(), VLogAbilitySystem, Verbose, TEXT("%s could not be activated due to blocked input ID %d"), *GetNameSafe(Spec->Ability), Spec->InputID);
		}*/
		return false;
	}

	if (bHasBlueprintCanUse)
	{
		if (K2_CanActivateAbility(*ActorInfo, Handle, OutTags) == false)
		{
			/*if (FScopedCanActivateAbilityLogEnabler::IsLoggingEnabled())
			{
				UE_LOG(LogAbilitySystem, Verbose, TEXT("%s: CanActivateAbility on %s failed, Blueprint override returned false"), *GetNameSafe(ActorInfo->OwnerActor.Get()), *GetNameSafe(Spec->Ability));
				UE_VLOG(ActorInfo->OwnerActor.Get(), VLogAbilitySystem, Verbose, TEXT("CanActivateAbility on %s failed, Blueprint override returned false"), *GetNameSafe(Spec->Ability));
			}*/

			return false;
		}
	}

	ReturnMsg = FString("Can Activate");
	return true;
}

bool USGSAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                  FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags) || !ActorInfo)
	{
		return false;
	}
	
	for (const TObjectPtr<UAdditionalAbilityCost>& AdditionalCost : AdditionalCosts)
	{
		if (AdditionalCost != nullptr)
		{
			if (!AdditionalCost->CheckCost(this, Handle, ActorInfo, /*inout*/ OptionalRelevantTags))
			{
				return false;
			}
		}
	}

	return true;
}

void USGSAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo) const
{
	//Super::ApplyCost(Handle, ActorInfo, ActivationInfo);
	UGameplayEffect* CostGE = GetCostGameplayEffect();
	if(CostGE)
	{
		float OutMagnitude = 0.f;
		if(CostGE->Modifiers[0].ModifierMagnitude.GetStaticMagnitudeIfPossible(0, OutMagnitude) && OutMagnitude != 0)
		{
			ApplyGameplayEffectToOwner(Handle, ActorInfo, ActivationInfo, CostGE, GetAbilityLevel(Handle, ActorInfo));
		}
	}
	

	check(ActorInfo);

	// 判断TargetData是否有HitResult
	auto DetermineIfAbilityHitTarget = [&]()
	{
		if (ActorInfo->IsNetAuthority())
		{
			if (USGSAbilitySystemComponent* ASC = Cast<USGSAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get()))
			{
				FGameplayAbilityTargetDataHandle TargetData;
				ASC->GetAbilityTargetData(Handle, ActivationInfo, TargetData);
				for (int32 TargetDataIdx = 0; TargetDataIdx < TargetData.Data.Num(); ++TargetDataIdx)
				{
					if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetData, TargetDataIdx))
					{
						return true;
					}
				}
			}
		}

		return false;
	};

	//应用附加消耗
	bool bAbilityHitTarget = false;
	bool bHasDeterminedIfAbilityHitTarget = false;
	for (const TObjectPtr<UAdditionalAbilityCost>& AdditionalCost : AdditionalCosts)
	{
		if (AdditionalCost != nullptr)
		{
			if (AdditionalCost->ShouldOnlyApplyCostOnHit())
			{
				if (!bHasDeterminedIfAbilityHitTarget)
				{
					bAbilityHitTarget = DetermineIfAbilityHitTarget();
					bHasDeterminedIfAbilityHitTarget = true;
				}

				if (!bAbilityHitTarget)
				{
					continue;
				}
			}

			AdditionalCost->ApplyCost(this, Handle, ActorInfo, ActivationInfo);
		}
	}
}

bool USGSAbility::DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent,
                                                          const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
                                                          FGameplayTagContainer* OptionalRelevantTags) const
{

	bool bBlocked = false;
	bool bMissing = false;

	UAbilitySystemGlobals& AbilitySystemGlobals = UAbilitySystemGlobals::Get();
	const FGameplayTag& BlockedTag = AbilitySystemGlobals.ActivateFailTagsBlockedTag;
	const FGameplayTag& MissingTag = AbilitySystemGlobals.ActivateFailTagsMissingTag;
	
	if (AbilitySystemComponent.AreAbilityTagsBlocked(AbilityTags))
	{
		bBlocked = true;
	}

	const USGSAbilitySystemComponent* ASC = Cast<USGSAbilitySystemComponent>(&AbilitySystemComponent);
	static FGameplayTagContainer AllRequiredTags;
	static FGameplayTagContainer AllBlockedTags;

	AllRequiredTags = ActivationRequiredTags;
	AllBlockedTags = ActivationBlockedTags;

	// copy原来ga中自带的内容让后在TRM中找GA对应的Tag，加在后面Tag唯一
	if (ASC)
	{
		ASC->GetAdditionalActivationTagRequirements(AbilityTags, AllRequiredTags, AllBlockedTags);
	}
	
	if (AllBlockedTags.Num() || AllRequiredTags.Num())
	{
		static FGameplayTagContainer AbilitySystemComponentTags;
		
		AbilitySystemComponentTags.Reset();
		AbilitySystemComponent.GetOwnedGameplayTags(AbilitySystemComponentTags);

		if (AbilitySystemComponentTags.HasAny(AllBlockedTags))
		{
			
			if (OptionalRelevantTags && AbilitySystemComponentTags.HasTag(FGameplayTag::RequestGameplayTag(FName("ActionState.Dead"))))
			{
				OptionalRelevantTags->AddTag(FSGSGameplayTags::Ability_ActivateFail_IsDead);
			}

			bBlocked = true;
		}

		if (!AbilitySystemComponentTags.HasAll(AllRequiredTags))
		{
			bMissing = true;
		}
	}

	if (SourceTags != nullptr)
	{
		if (SourceBlockedTags.Num() || SourceRequiredTags.Num())
		{
			if (SourceTags->HasAny(SourceBlockedTags))
			{
				bBlocked = true;
			}

			if (!SourceTags->HasAll(SourceRequiredTags))
			{
				bMissing = true;
			}
		}
	}

	if (TargetTags != nullptr)
	{
		if (TargetBlockedTags.Num() || TargetRequiredTags.Num())
		{
			if (TargetTags->HasAny(TargetBlockedTags))
			{
				bBlocked = true;
			}

			if (!TargetTags->HasAll(TargetRequiredTags))
			{
				bMissing = true;
			}
		}
	}

	if (bBlocked)
	{
		if (OptionalRelevantTags && BlockedTag.IsValid())
		{
			OptionalRelevantTags->AddTag(BlockedTag);
		}
		return false;
	}
	if (bMissing)
	{
		if (OptionalRelevantTags && MissingTag.IsValid())
		{
			OptionalRelevantTags->AddTag(MissingTag);
		}
		return false;
	}

	return true;
}

bool USGSAbility::DoesAbilitySatisfyTagRequirementsWithMsg(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags, FString* ReturnMsg) const
{
	bool bBlocked = false;
	bool bMissing = false;

	UAbilitySystemGlobals& AbilitySystemGlobals = UAbilitySystemGlobals::Get();
	const FGameplayTag& BlockedTag = AbilitySystemGlobals.ActivateFailTagsBlockedTag;
	const FGameplayTag& MissingTag = AbilitySystemGlobals.ActivateFailTagsMissingTag;
	
	if (AbilitySystemComponent.AreAbilityTagsBlocked(AbilityTags))
	{
		bBlocked = true;
	}

	const USGSAbilitySystemComponent* ASC = Cast<USGSAbilitySystemComponent>(&AbilitySystemComponent);
	static FGameplayTagContainer AllRequiredTags;
	static FGameplayTagContainer AllBlockedTags;

	AllRequiredTags = ActivationRequiredTags;
	AllBlockedTags = ActivationBlockedTags;

	// copy原来ga中自带的内容让后在TRM中找GA对应的Tag，加在后面Tag唯一
	if (ASC)
	{
		ASC->GetAdditionalActivationTagRequirements(AbilityTags, AllRequiredTags, AllBlockedTags);
	}
	
	if (AllBlockedTags.Num() || AllRequiredTags.Num())
	{
		static FGameplayTagContainer AbilitySystemComponentTags;
		
		AbilitySystemComponentTags.Reset();
		AbilitySystemComponent.GetOwnedGameplayTags(AbilitySystemComponentTags);

		if (AbilitySystemComponentTags.HasAny(AllBlockedTags))
		{
			
			if (OptionalRelevantTags && AbilitySystemComponentTags.HasTag(FGameplayTag::RequestGameplayTag(FName("ActionState.Dead"))))
			{
				OptionalRelevantTags->AddTag(FSGSGameplayTags::Ability_ActivateFail_IsDead);
			}

			bBlocked = true;
		}

		if (!AbilitySystemComponentTags.HasAll(AllRequiredTags))
		{
			bMissing = true;
		}
	}

	if (SourceTags != nullptr)
	{
		if (SourceBlockedTags.Num() || SourceRequiredTags.Num())
		{
			if (SourceTags->HasAny(SourceBlockedTags))
			{
				bBlocked = true;
			}

			if (!SourceTags->HasAll(SourceRequiredTags))
			{
				bMissing = true;
			}
		}
	}

	if (TargetTags != nullptr)
	{
		if (TargetBlockedTags.Num() || TargetRequiredTags.Num())
		{
			if (TargetTags->HasAny(TargetBlockedTags))
			{
				bBlocked = true;
			}

			if (!TargetTags->HasAll(TargetRequiredTags))
			{
				bMissing = true;
			}
		}
	}

	if (bBlocked)
	{
		if (OptionalRelevantTags && BlockedTag.IsValid())
		{
			OptionalRelevantTags->AddTag(BlockedTag);
		}
		*ReturnMsg = FString("Blocked Failed");
		return false;
	}
	if (bMissing)
	{
		if (OptionalRelevantTags && MissingTag.IsValid())
		{
			OptionalRelevantTags->AddTag(MissingTag);
		}
		*ReturnMsg = FString("Required Failed");
		return false;
	}

	*ReturnMsg = FString("Ability pass required and block");
	return true;
}

void USGSAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if(bIsCancelableSkill)
	{
		UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetAvatarActorFromActorInfo());
		if(GameInstance)
		{
			USkillManager* SkillManager = GameInstance->GetSubsystem<USkillManager>();
			if(SkillManager)
			{
				SkillManager->SetCurrentActivatedSkillTags(AbilityTags);
			}
		}
	}
}
