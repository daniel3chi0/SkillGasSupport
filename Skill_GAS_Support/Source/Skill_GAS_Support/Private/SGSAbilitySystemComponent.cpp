// Fill out your copyright notice in the Description page of Project Settings.
#include "SGSAbilitySystemComponent.h"
#include "InputActionValue.h"
#include "Abilities/SGSAbility.h"
#include "SkillSystemFunctionLibrary.h"
#include "Engine/AssetManager.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystem//SkillManager.h"


USGSAbilitySystemComponent::USGSAbilitySystemComponent()
{
	// 加载资产，路径是硬编码的
	// static ConstructorHelpers::FObjectFinder<UWerewolf_GameTotalConfig> GameTotalConfigAsset(TEXT("/Game/Program/Config/GameConfig/GameTotalConfig.GameTotalConfig"));
	// GameTotalConfig = GameTotalConfigAsset.Object;
}

void USGSAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();
	//AddViolationTagEvent();
}

void USGSAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag,const FInputActionValue& InputActionValue)
{
	if(InputTag.IsValid())
	{
		for(const FGameplayAbilitySpec& AbilitySpec: ActivatableAbilities.Items)
		{
			if(AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				if(USGSAbility* UWerewolfAbility = Cast<USGSAbility>(AbilitySpec.Ability))
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

void USGSAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag,const FInputActionValue& InputActionValue)
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

void USGSAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
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
			if(USGSAbility* Werewolf_Ability = Cast<USGSAbility>(AbilitySpec->Ability))
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

void USGSAbilitySystemComponent::ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

void USGSAbilitySystemComponent::SetAllSkillParameters(FScalableFloat InPara)
{
	SkillParameters = InPara;
	SetSkillParameters(InPara);
}

void USGSAbilitySystemComponent::SetSkillParameters_Implementation(FScalableFloat InPara)
{
	SkillParameters = InPara;
}

bool USGSAbilitySystemComponent::SetSkillParameters_Validate(FScalableFloat InPara)
{
	return  true;
}

void USGSAbilitySystemComponent::OnGameplayTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	AActor* TempOwner = GetOwner();
	if (!TempOwner)
	{
		return;
	}
	APlayerState* TempPS = Cast<APlayerState>(TempOwner);
	if (!TempPS)
	{
		return;
	}
}

void USGSAbilitySystemComponent::AddViolationTagEvent()
{
	AActor* TempOwner = GetOwner();
	if (!TempOwner)
	{
		return;
	}
}

UGameplayAbility* USGSAbilitySystemComponent::CreateNewInstanceOfAbility(FGameplayAbilitySpec& Spec, const UGameplayAbility* Ability)
{
	check(Ability);
	check(Ability->HasAllFlags(RF_ClassDefaultObject));

	AActor* Owner = GetOwner();
	check(Owner);

	UGameplayAbility * AbilityInstance = NewObject<UGameplayAbility>(Owner, Ability->GetClass());
	
	USGSAbility* Werewolf_Ability = const_cast<USGSAbility*>(Cast<USGSAbility>(Ability));
	USGSAbility* Werewolf_Ability_Instance = Cast<USGSAbility>(AbilityInstance);
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

void USGSAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);
	
	if (Spec.IsActive())
	{
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void USGSAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	if (Spec.IsActive())
	{
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void USGSAbilitySystemComponent::GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags,
	FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const
{
	if (TagRelationshipMapping)
	{
		TagRelationshipMapping->GetRequiredAndBlockedActivationTags(AbilityTags, &OutActivationRequired, &OutActivationBlocked);
	}
}

void USGSAbilitySystemComponent::SetTagRelationshipMapping(UAbilityTagRelationshipMapping* NewMapping)
{
	TagRelationshipMapping = NewMapping;
}

void USGSAbilitySystemComponent::GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle, FGameplayAbilityActivationInfo ActivationInfo, FGameplayAbilityTargetDataHandle& OutTargetDataHandle)
{
	TSharedPtr<FAbilityReplicatedDataCache> ReplicatedData = AbilityTargetDataMap.Find(FGameplayAbilitySpecHandleAndPredictionKey(AbilityHandle, ActivationInfo.GetActivationPredictionKey()));
	if (ReplicatedData.IsValid())
	{
		OutTargetDataHandle = ReplicatedData->TargetData;
	}
}

void USGSAbilitySystemComponent::K2_CancelAbilities(FGameplayTagContainer TagContainer)
{
	const FGameplayTagContainer* InnerTagContainer = &TagContainer;
	Super::CancelAbilities(InnerTagContainer);
}

bool USGSAbilitySystemComponent::K2_IsAbilityActiveByTag(const FGameplayTag& Tag)
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

bool USGSAbilitySystemComponent::K2_CanActivateAbilityByTagsWithMsg(FGameplayTagContainer Tags, FString& ResultMsg)
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

			if(USGSAbility* Werewolf_Ability = Cast<USGSAbility>(Spec.GetPrimaryInstance()))
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

void USGSAbilitySystemComponent::ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags,
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
