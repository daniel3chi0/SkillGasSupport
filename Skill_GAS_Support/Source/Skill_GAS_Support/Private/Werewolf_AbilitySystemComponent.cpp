// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Werewolf_AbilitySystemComponent.h"

#include "InputActionValue.h"
#include "AbilitySystem/SkillSystemFunctionLibrary.h"
#include "AbilitySystem/Abilities/Werewolf_Ability.h"
#include "BlueprintFunctionLibrary/Werewolf_CommonBlueprintFunctionLibrary.h"
#include "Config/Werewolf_GameTotalConfig.h"
#include "Engine/AssetManager.h"
#include "Game/Werewolf_HeroPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Manager/SkillManager.h"


UWerewolf_AbilitySystemComponent::UWerewolf_AbilitySystemComponent()
{
	// 加载资产，路径是硬编码的
	// static ConstructorHelpers::FObjectFinder<UWerewolf_GameTotalConfig> GameTotalConfigAsset(TEXT("/Game/Program/Config/GameConfig/GameTotalConfig.GameTotalConfig"));
	// GameTotalConfig = GameTotalConfigAsset.Object;
}

void UWerewolf_AbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();
	AddViolationTagEvent();
	

	
}

void UWerewolf_AbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag,const FInputActionValue& InputActionValue)
{
	if(InputTag.IsValid())
	{
		for(const FGameplayAbilitySpec& AbilitySpec: ActivatableAbilities.Items)
		{
			if(AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				if(UWerewolf_Ability* UWerewolfAbility = Cast<UWerewolf_Ability>(AbilitySpec.Ability))
				{
					UWerewolfAbility->SetChooseNum(InputActionValue.GetMagnitude());
					SetAllSkillParameters(InputActionValue.GetMagnitude());
					
				}
				InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
				
			}
		}
	}
}

void UWerewolf_AbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag,const FInputActionValue& InputActionValue)
{
	if(InputTag.IsValid())
	{
		for(const FGameplayAbilitySpec& AbilitySpec: ActivatableAbilities.Items)
		{
			if(AbilitySpec.Ability&&(AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.Remove(AbilitySpec.Handle);
			}
		}
	}
}

void UWerewolf_AbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	
	
	//每次清除要激活的能力数组
	static  TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	//处理持续按压
	for(const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if(const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if(AbilitySpec->Ability && !AbilitySpec->IsActive())
			{
				//后期还需修改
				AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
			}
		}
	}
	//处理trigger在这一帧
	for(const FGameplayAbilitySpecHandle& SpecHandle:InputPressedSpecHandles)
	{
		if(FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if(AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;

				if(AbilitySpec->IsActive())
				{
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}

	for(const FGameplayAbilitySpecHandle& AbilitySpecHandle:AbilitiesToActivate)
	{
		bool bCanActivate = true;
		if(FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(AbilitySpecHandle))
		{
			if(UWerewolf_Ability* Werewolf_Ability = Cast<UWerewolf_Ability>(AbilitySpec->Ability))
			{
				FString ResultMsg = FString("Skill Activate Default Messages");
				if(UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld()))
				{
					if(USkillManager* SkillManager = GameInstance->GetSubsystem<USkillManager>())
					{
						if(SkillManager->OnReturnSkillActivateMsg.IsBound() && USkillSystemFunctionLibrary::SkillIdIsValid(Werewolf_Ability->SkillId))
						{
							bCanActivate = K2_CanActivateAbilityByTagsWithMsg(Werewolf_Ability->AbilityTags, ResultMsg);
							SkillManager->OnReturnSkillActivateMsg.Broadcast(bCanActivate, Werewolf_Ability->SkillId, ResultMsg);
						}
					}
				}
			}
		}

		if(bCanActivate)
		{
			TryActivateAbility(AbilitySpecHandle);
		}
	}

	//处理松开Release
	for(const FGameplayAbilitySpecHandle& SpecHandle:InputReleasedSpecHandles)
	{
		if(FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if(AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;

				if(AbilitySpec->IsActive())
				{
					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}

	//清空待处理数组
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void UWerewolf_AbilitySystemComponent::ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

void UWerewolf_AbilitySystemComponent::SetAllSkillParameters(FScalableFloat InPara)
{
	SkillParameters = InPara;
	SetSkillParameters(InPara);
}

void UWerewolf_AbilitySystemComponent::SetSkillParameters_Implementation(FScalableFloat InPara)
{
	SkillParameters = InPara;
}

bool UWerewolf_AbilitySystemComponent::SetSkillParameters_Validate(FScalableFloat InPara)
{
	return  true;
}

void UWerewolf_AbilitySystemComponent::OnGameplayTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	AActor* TempOwner = GetOwner();
	if (!TempOwner)
	{
		return;
	}
	AWerewolf_HeroPlayerState* TempPS = Cast<AWerewolf_HeroPlayerState>(TempOwner);
	if (!TempPS)
	{
		return;
	}
	UWerewolf_CharacterManager* CharacterManager = TempPS->GetGameInstance()->GetSubsystem<UWerewolf_CharacterManager>();
	if (!CharacterManager)
	{
		return;
	}
	if (TempOwner->HasAuthority())
	{
		if (NewCount==0)
		{
			CharacterManager->SetWerewolfWantedFromID(TempPS->GetPlayerInfo().PlayerID,EWerewolfWanted::NotWanted);
		}
		if (NewCount==1)
		{
			CharacterManager->SetWerewolfWantedFromID(TempPS->GetPlayerInfo().PlayerID,EWerewolfWanted::Wanted);
		}
	}
	
}

void UWerewolf_AbilitySystemComponent::AddViolationTagEvent()
{
	AActor* TempOwner = GetOwner();
	if (!TempOwner)
	{
		return;
	}
	if (AWerewolf_HeroPlayerState* TempPS = Cast<AWerewolf_HeroPlayerState>(TempOwner))
	{
		if (TempOwner->HasAuthority())
		{
			// 使用 StaticLoadObject 同步加载资产
			GameTotalConfig = Cast<UWerewolf_GameTotalConfig>(StaticLoadObject(UWerewolf_GameTotalConfig::StaticClass(), nullptr, *AssetPath));
			for (FGameplayTag& TempTag:GameTotalConfig->ViolationTag)
			{
				//监听GameplayTag
				RegisterGameplayTagEvent(TempTag,EGameplayTagEventType::NewOrRemoved).AddUObject(this,&UWerewolf_AbilitySystemComponent::OnGameplayTagChanged);

			}
		}
	}

	
}

UGameplayAbility* UWerewolf_AbilitySystemComponent::CreateNewInstanceOfAbility(FGameplayAbilitySpec& Spec, const UGameplayAbility* Ability)
{
	check(Ability);
	check(Ability->HasAllFlags(RF_ClassDefaultObject));

	AActor* Owner = GetOwner();
	check(Owner);

	UGameplayAbility * AbilityInstance = NewObject<UGameplayAbility>(Owner, Ability->GetClass());
	
	UWerewolf_Ability* Werewolf_Ability = const_cast<UWerewolf_Ability*>(Cast<UWerewolf_Ability>(Ability));
	UWerewolf_Ability* Werewolf_Ability_Instance = Cast<UWerewolf_Ability>(AbilityInstance);
	if(!Werewolf_Ability || !Werewolf_Ability_Instance)
	{
		return Super::CreateNewInstanceOfAbility(Spec, Ability);
	}
	Werewolf_Ability_Instance->SkillId = Werewolf_Ability->SkillId;
	check(Werewolf_Ability_Instance);

	// Add it to one of our instance lists so that it doesn't GC.
	if (Werewolf_Ability_Instance->GetReplicationPolicy() != EGameplayAbilityReplicationPolicy::ReplicateNo)
	{
		Spec.ReplicatedInstances.Add(Werewolf_Ability_Instance);
		AddReplicatedInstancedAbility(Werewolf_Ability_Instance);
	}
	else
	{
		Spec.NonReplicatedInstances.Add(Werewolf_Ability_Instance);
	}
	
	return Werewolf_Ability_Instance;
}

void UWerewolf_AbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);
	
	if (Spec.IsActive())
	{
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void UWerewolf_AbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	if (Spec.IsActive())
	{
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void UWerewolf_AbilitySystemComponent::GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags,
	FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const
{
	if (TagRelationshipMapping)
	{
		TagRelationshipMapping->GetRequiredAndBlockedActivationTags(AbilityTags, &OutActivationRequired, &OutActivationBlocked);
	}
}

void UWerewolf_AbilitySystemComponent::SetTagRelationshipMapping(UAbilityTagRelationshipMapping* NewMapping)
{
	TagRelationshipMapping = NewMapping;
}

void UWerewolf_AbilitySystemComponent::GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle, FGameplayAbilityActivationInfo ActivationInfo, FGameplayAbilityTargetDataHandle& OutTargetDataHandle)
{
	TSharedPtr<FAbilityReplicatedDataCache> ReplicatedData = AbilityTargetDataMap.Find(FGameplayAbilitySpecHandleAndPredictionKey(AbilityHandle, ActivationInfo.GetActivationPredictionKey()));
	if (ReplicatedData.IsValid())
	{
		OutTargetDataHandle = ReplicatedData->TargetData;
	}
}

void UWerewolf_AbilitySystemComponent::K2_CancelAbilities(FGameplayTagContainer TagContainer)
{
	const FGameplayTagContainer* InnerTagContainer = &TagContainer;
	Super::CancelAbilities(InnerTagContainer);
}

bool UWerewolf_AbilitySystemComponent::K2_IsAbilityActiveByTag(const FGameplayTag& Tag)
{
	ABILITYLIST_SCOPE_LOCK();
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (!Spec.IsActive() || Spec.Ability == nullptr)
		{
			continue;
		}

		if(Spec.Ability->AbilityTags.HasTagExact(Tag))
		{
			return true;
		}
	}

	return false;
}

bool UWerewolf_AbilitySystemComponent::K2_CanActivateAbilityByTagsWithMsg(FGameplayTagContainer Tags, FString& ResultMsg)
{
	ResultMsg = FString("Skill Activate Default Messages");
	const FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	ABILITYLIST_SCOPE_LOCK();
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.Ability == nullptr)
		{
			continue;
		}

		/*if(Spec.Ability->AbilityTags.HasTagExact(FGameplayTag::RequestGameplayTag("AbilityTag.Magic.Silence&Slow")))
		{
			UE_LOG(LogTemp, Log, TEXT("11111"));
		}*/
		
		if(Spec.Ability->AbilityTags.HasAllExact(Tags))
		{
			if(!Spec.GetPrimaryInstance()) continue;

			if(UWerewolf_Ability* Werewolf_Ability = Cast<UWerewolf_Ability>(Spec.GetPrimaryInstance()))
			{
				return Werewolf_Ability->CanActivateAbilityReturnMsg(Spec.Handle, ActorInfo, nullptr, nullptr,
					nullptr, ResultMsg);
			}
			
			if(Spec.GetPrimaryInstance()->CanActivateAbility(Spec.Handle, ActorInfo))
			{
				return true;
			}
		}
	}

	return false;
}

void UWerewolf_AbilitySystemComponent::ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags,
                                                                      UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags,
                                                                      bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags)
{
	FGameplayTagContainer ModifiedBlockTags = BlockTags;
	FGameplayTagContainer ModifiedCancelTags = CancelTags;

	if (TagRelationshipMapping)
	{
		TagRelationshipMapping->GetAbilityTagsToBlockAndCancel(AbilityTags, &ModifiedBlockTags, &ModifiedCancelTags);
	}
	
	Super::ApplyAbilityBlockAndCancelTags(AbilityTags, RequestingAbility, bEnableBlockTags, ModifiedBlockTags,
	                                      bExecuteCancelTags, ModifiedCancelTags);
}
