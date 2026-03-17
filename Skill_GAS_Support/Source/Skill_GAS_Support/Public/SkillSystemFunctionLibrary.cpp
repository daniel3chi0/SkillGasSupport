#include "SkillSystemFunctionLibrary.h"

#include "AbilitySystemComponent.h"
#include "Werewolf_AbilitySystemComponent.h"
#include "Werewolf_MagicSystemComponent.h"
#include "Abilities/Werewolf_Ability.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "Manager/SkillManager.h"
#include "TargetActor/Werewolf_GATargetDataType.h"
//#include "Manager/SkillManager.cpp"

float USkillSystemFunctionLibrary::GetAngleForActorForwardVector(FVector RefPoint, FVector ContraPoint, AActor* RefActor)
{
	if(!RefPoint.Equals(ContraPoint))
	{
		FVector NormalDir = (ContraPoint - RefPoint).GetSafeNormal();
		FVector RefActorForwardVector =  RefActor->GetActorForwardVector();
		double AngleCos = NormalDir.Dot(RefActorForwardVector);
		double Angle = FMath::Acos(AngleCos);
		return Angle;
	}
	return 0;
}

void USkillSystemFunctionLibrary::GiveMagicAbilityById(UObject* WorldContextObject, UAbilitySystemComponent* ASC, int32 MagicAbilityId)
{
	if(AActor* Owner = ASC->GetOwner())
	{
		if(UWerewolf_MagicSystemComponent* MSC = Owner->GetComponentByClass<UWerewolf_MagicSystemComponent>())
		{
			FGameplayAbilitySpecHandle GASpecHandle = MSC->GetLearnedMagicGASpecHandleByMagicId(MagicAbilityId);
			if(GASpecHandle.IsValid()) return;
		}
	}
	
	if(UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject))
	{
		if (USkillManager* SkillManager = GameInstance->GetSubsystem<USkillManager>())
		{
			FSkillRowStruct* SkillRow = SkillManager->GetTableRowsMapRowById<FSkillRowStruct>("SkillTable", MagicAbilityId);
			if(SkillRow)
			{
				for (TSoftClassPtr<UGameplayAbility> AbilityClass : SkillRow->AbilityClasses)
				{
					if(!AbilityClass.IsNull())
					{
						FSoftObjectPath Path = AbilityClass.ToSoftObjectPath();
						UAssetManager::GetStreamableManager().RequestAsyncLoad(Path, FStreamableDelegate::CreateStatic(&USkillSystemFunctionLibrary::OnMagicAbilityLoaded, Path, SkillRow,ASC, WorldContextObject));
					}
				}
			}
			
			//FMagicRowStruct* MagicRowStruct = SkillManager->GetTableRowsMapRowById<FMagicRowStruct>("MagicInfoTable", MagicAbilityId);
		}
	}
}

void USkillSystemFunctionLibrary::OnMagicAbilityLoaded(FSoftObjectPath Path, FSkillRowStruct* MagicSkillRow, UAbilitySystemComponent* ASC, UObject* Context)
{
	if(UObject* RO = Path.ResolveObject())
	{
		UClass* MagicAbilityClass = Cast<UClass>(RO);
		
		if(ASC && ASC->IsOwnerActorAuthoritative())
		{
			if(UWerewolf_Ability* MagicCDO = Cast<UWerewolf_Ability>(MagicAbilityClass->GetDefaultObject()))
			{
				MagicCDO->SkillId = MagicSkillRow->SkillId;
				FGameplayAbilitySpec AbilitySpec(MagicCDO, 1);
				AbilitySpec.SourceObject = Context;
				const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);
				if(AActor* Owner = ASC->GetOwner())
				{
					UWerewolf_MagicSystemComponent* MSC = Owner->GetComponentByClass<UWerewolf_MagicSystemComponent>();
					if(MSC)
					{
						FLearnedMagicDataWrap LearnedMagicDataWrap;
						LearnedMagicDataWrap.MagicAbilityId = MagicSkillRow->SkillId;
						LearnedMagicDataWrap.GASpecHandle = AbilitySpecHandle;
						MSC->AddToLearnedMagicInfoArray(LearnedMagicDataWrap);
					}
				}
			}
			/*if(UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(Context))
			{
				if (USkillManager* SkillManager = GameInstance->GetSubsystem<USkillManager>())
				{
					SkillManager->AddToLearnedMagicArray(MagicSkillRow);
					if(SkillManager->OnMagicLearned.IsBound())
					{
						SkillManager->OnMagicLearned.Broadcast();
					}
				}
			}*/
		}
	}
}

FMagicRowStruct USkillSystemFunctionLibrary::GetMagicInfoBySkillId(UObject* WorldContextObject, int32 SkillId)
{
	if(UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject))
	{
		if (USkillManager* SkillManager = GameInstance->GetSubsystem<USkillManager>())
		{
			FMagicRowStruct MagicRow = *(SkillManager->GetTableRowsMapRowById<FMagicRowStruct>("MagicInfoTable", SkillId));
			return MagicRow;
		}
	}
	
	return FMagicRowStruct();
}

FSkillRowStruct USkillSystemFunctionLibrary::GetSkillInfoBySkillId(UObject* WorldContextObject, int32 SkillId)
{
	if(UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject))
	{
		if (USkillManager* SkillManager = GameInstance->GetSubsystem<USkillManager>())
		{
			FSkillRowStruct* SkillRow = SkillManager->GetTableRowsMapRowById<FSkillRowStruct>("SkillTable", SkillId);
			if(SkillRow)
			{
				return *SkillRow;
			}
		}
	}
	
	return FSkillRowStruct();
}

bool USkillSystemFunctionLibrary::IsAbilityActiveByTag(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTag& Tag)
{
	if(!AbilitySystemComponent) return false;

	UWerewolf_AbilitySystemComponent* ASC = Cast<UWerewolf_AbilitySystemComponent>(AbilitySystemComponent);
	if(ASC)
	{
		return ASC->K2_IsAbilityActiveByTag(Tag);
	}

	return false;
}

UWerewolf_MagicSystemComponent* USkillSystemFunctionLibrary::GetLocalPlayerMagicSystemComponent(UObject* WorldContextObject)
{
	if(UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject))
	{
		if (USkillManager* SkillManager = GameInstance->GetSubsystem<USkillManager>())
		{
			SkillManager->GetLocalPlayerMagicSystemComponent();
		}
	}

	return nullptr;
}

UWerewolf_MagicSystemComponent* USkillSystemFunctionLibrary::GetPlayerMagicSystemComponent(UObject* WorldContextObject, AActor* CheckActor)
{
	if(UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject))
	{
		if (USkillManager* SkillManager = GameInstance->GetSubsystem<USkillManager>())
		{
			return SkillManager->GetPlayerMagicSystemComponent(CheckActor);
		}
	}

	return nullptr;
}

bool USkillSystemFunctionLibrary::EquipMagicAbilityByMagicId(UObject* WorldContextObject, int32 MagicId)
{
	if(UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject))
	{
		if (USkillManager* SkillManager = GameInstance->GetSubsystem<USkillManager>())
		{
			return SkillManager->EquipMagicAbilityByMagicId(MagicId);
		}
	}

	return false;
}

bool USkillSystemFunctionLibrary::TryUseCurrentEquipedMagicAbility(UObject* WorldContextObject, UAbilitySystemComponent* ASC)
{
	if(UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject))
	{
		if (USkillManager* SkillManager = GameInstance->GetSubsystem<USkillManager>())
		{
			return SkillManager->TryUseCurrentEquipedMagicAbility(ASC);
		}
	}

	return false;
}

UWerewolf_Ability* USkillSystemFunctionLibrary::GetCurrentEquipedMagicClass(UObject* WorldContextObject, UAbilitySystemComponent* ASC)
{
	if(UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject))
	{
		if (USkillManager* SkillManager = GameInstance->GetSubsystem<USkillManager>())
		{
			return SkillManager->GetCurrentEquipedMagicClass(ASC);
		}
	}

	return nullptr;
}

FGameplayTagContainer USkillSystemFunctionLibrary::GetCurrentEquipedMagicCooldownTags(UObject* WorldContextObject, UAbilitySystemComponent* ASC)
{
	if(UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject))
	{
		if (USkillManager* SkillManager = GameInstance->GetSubsystem<USkillManager>())
		{
			return SkillManager->GetCurrentEquipedMagicCooldownTags(ASC);
		}
	}

	return FGameplayTagContainer();
}

UWerewolf_Ability* USkillSystemFunctionLibrary::GetPlayerLearnedMagicClassById(UObject* WorldContextObject, int32 MagicAbilityId)
{
	if(UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject))
	{
		if (USkillManager* SkillManager = GameInstance->GetSubsystem<USkillManager>())
		{
			return SkillManager->GetPlayerLearnedMagicClassById(MagicAbilityId);
		}
	}

	return nullptr;
}

FGameplayTagContainer USkillSystemFunctionLibrary::GetPlayerLearnedMagicCooldownTagsById(UObject* WorldContextObject, int32 MagicAbilityId)
{
	if(UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject))
	{
		if (USkillManager* SkillManager = GameInstance->GetSubsystem<USkillManager>())
		{
			return SkillManager->GetPlayerLearnedMagicCooldownTagsById(MagicAbilityId);
		}
	}

	return FGameplayTagContainer();
}

bool USkillSystemFunctionLibrary::HasGivedAbilityByTag(UAbilitySystemComponent* ASC, FGameplayTag Tag)
{
	if(ASC)
	{
		TArray<FGameplayAbilitySpecHandle> GASpecHandles;
		FGameplayTagQuery TagQuery = FGameplayTagQuery::MakeQuery_MatchTag(Tag);
		ASC->FindAllAbilitiesMatchingQuery(GASpecHandles, TagQuery);
		return GASpecHandles.Num() > 0;
	}

	return false;
}

FGameplayTagContainer USkillSystemFunctionLibrary::GetCurrentEquipedMagicAbilityTags(UObject* WorldContextObject, UAbilitySystemComponent* ASC)
{
	if(UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject))
	{
		if (USkillManager* SkillManager = GameInstance->GetSubsystem<USkillManager>())
		{
			return SkillManager->GetCurrentEquipedMagicAbilityTags(ASC);
		}
	}

	return FGameplayTagContainer();
}

FGameplayTag USkillSystemFunctionLibrary::GetCurrentEquipedMagicAbilityTriggerTag(UObject* WorldContextObject, UAbilitySystemComponent* ASC)
{
	if(UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject))
	{
		if (USkillManager* SkillManager = GameInstance->GetSubsystem<USkillManager>())
		{
			return SkillManager->GetCurrentEquipedMagicAbilityTriggerTag(ASC);
		}
	}

	return FGameplayTag();
}

FGameplayTag USkillSystemFunctionLibrary::GetAbilityTriggerTagFromSpecHandle(UObject* WorldContextObject, UAbilitySystemComponent* ASC, FGameplayAbilitySpecHandle SpecHandle)
{
	if(UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject))
	{
		if (USkillManager* SkillManager = GameInstance->GetSubsystem<USkillManager>())
		{
			return SkillManager->GetAbilityTriggerTagFromSpecHandle(ASC, SpecHandle);
		}
	}

	return FGameplayTag();
}

void USkillSystemFunctionLibrary::GetPlayerAllLearnedMagicAbilitiesIds(UObject* WorldContextObject, AActor* CheckActor, TArray<int32>& ResultArray)
{
	if(UWerewolf_MagicSystemComponent* MSC = GetPlayerMagicSystemComponent(WorldContextObject, CheckActor))
	{
		MSC->GetAllLearnedMagicAbilitiesIds(ResultArray);
	}
}

bool USkillSystemFunctionLibrary::MagicRowIsValid(const FMagicRowStruct& MagicRow)
{
	return MagicRow.IsValid();
}

bool USkillSystemFunctionLibrary::SkillRowIsValid(const FSkillRowStruct& SkillRow)
{
	return SkillRow.IsValid();
}

bool USkillSystemFunctionLibrary::SkillIdIsValid(const int32 SkillId)
{
	return SkillId > 0;
}

FMagicRowStruct USkillSystemFunctionLibrary::GetMagicRowFromTargetData(const FGameplayAbilityTargetDataHandle& TargetData, int32 Index)
{
	if (TargetData.Data.IsValidIndex(Index))
	{
		FGameplayAbilityTargetData* Data = TargetData.Data[Index].Get();
		if (Data)
		{
			if(Data->GetScriptStruct() == FGameplayAbilityTargetData_ActorsOrHitResult::StaticStruct())
			{
				FGameplayAbilityTargetData_ActorsOrHitResult* TargetData_AOH = static_cast<FGameplayAbilityTargetData_ActorsOrHitResult*>(Data);
				return TargetData_AOH->MagicRow;	
			}
		}
	}

	return FMagicRowStruct();
}

FGameplayAbilitySpecHandle USkillSystemFunctionLibrary::GetCurrentEquipedMagicSpecHandleFromTargetData(const FGameplayAbilityTargetDataHandle& TargetData, int32 Index)
{
	if (TargetData.Data.IsValidIndex(Index))
	{
		FGameplayAbilityTargetData* Data = TargetData.Data[Index].Get();
		if (Data)
		{
			if(Data->GetScriptStruct() == FGameplayAbilityTargetData_ActorsOrHitResult::StaticStruct())
			{
				FGameplayAbilityTargetData_ActorsOrHitResult* TargetData_AOH = static_cast<FGameplayAbilityTargetData_ActorsOrHitResult*>(Data);
				return TargetData_AOH->CurrentEquipedMagicSpecHandle;	
			}
		}
	}

	return FGameplayAbilitySpecHandle();
}

void USkillSystemFunctionLibrary::GetCDTimeRemainingAndDurationByTag(UAbilitySystemComponent* ASC, FGameplayTagContainer InCooldownTags, float& TimeRemaining, float& CooldownDuration)
{
	TimeRemaining = 0.f;
	CooldownDuration = 0.f;
	if (IsValid(ASC) && InCooldownTags.Num() > 0)
	{
		FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(InCooldownTags);
		TArray< TPair<float, float> > DurationAndTimeRemaining = ASC->GetActiveEffectsTimeRemainingAndDuration(Query);
		if (DurationAndTimeRemaining.Num() > 0)
		{
			int32 BestIdx = 0;
			float LongestTime = DurationAndTimeRemaining[0].Key;
			for (int32 Idx = 1; Idx < DurationAndTimeRemaining.Num(); ++Idx)
			{
				if (DurationAndTimeRemaining[Idx].Key > LongestTime)
				{
					LongestTime = DurationAndTimeRemaining[Idx].Key;
					BestIdx = Idx;
				}
			}

			TimeRemaining = DurationAndTimeRemaining[BestIdx].Key;
			CooldownDuration = DurationAndTimeRemaining[BestIdx].Value;
		}
	}
}

