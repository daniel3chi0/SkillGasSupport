#include "AbilitySystem/Werewolf_MagicSystemComponent.h"

#include "AbilitySystem/TargetActor/GATA_InPlace_GroundTrace.h"
#include "AbilitySystem/TargetActor/GATA_Radius.h"
#include "AbilitySystem/TargetActor/GATA_SphereTrace.h"
#include "Character/Werewolf_Hero.h"
#include "Game/Werewolf_HeroPlayerState.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Manager/SkillManager.h"
#include "Net/UnrealNetwork.h"


UWerewolf_MagicSystemComponent::UWerewolf_MagicSystemComponent()
{
	SetIsReplicatedByDefault(true);
}

void UWerewolf_MagicSystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UWerewolf_MagicSystemComponent, LearnedMagicInfoArray);
	DOREPLIFETIME(UWerewolf_MagicSystemComponent, EquipedMagicAbilityId);
	DOREPLIFETIME(UWerewolf_MagicSystemComponent, GroundTraceTargetActor);
}

bool UWerewolf_MagicSystemComponent::AddToLearnedMagicInfoArray(FLearnedMagicDataWrap& LearnedMagicData)
{
	if(!LearnedMagicInfoArray.Contains(LearnedMagicData))
	{
		LearnedMagicInfoArray.AddUnique(LearnedMagicData);
		if(AActor* OwnerActor = GetOwner())
		{
			if(OwnerActor->HasAuthority())
			{
				OnRep_UpdateLearnedMagicInfoArray();
			}
		}
		return true;
	}
	return false;
}

TArray<FLearnedMagicDataWrap>& UWerewolf_MagicSystemComponent::GetLearnedMagicInfoArray()
{
	return LearnedMagicInfoArray;
}

FGameplayAbilitySpecHandle UWerewolf_MagicSystemComponent::GetLearnedMagicGASpecHandleByMagicId(int32 MagicId)
{
	return LearnedMagicLinkMap.FindRef(MagicId);
}

void UWerewolf_MagicSystemComponent::FillMagicChantTime(float DeltaTime)
{
	AWerewolf_HeroPlayerState* HeroPlayerState = Cast<AWerewolf_HeroPlayerState>(GetOwner());
	AWerewolf_Hero* EnemyPawn = Cast<AWerewolf_Hero>(GetOwner());
	//&& HeroPlayerState->GetPawn()->IsLocallyControlled()
	if((HeroPlayerState && HeroPlayerState->GetPawn()->IsLocallyControlled()) || (EnemyPawn && EnemyPawn->HasAuthority()))
	{
		CurrentChantTime += DeltaTime;
		if(ReadMagicChantState == EReadMagicChantState::NotChanting)
		{
			ReadMagicChantState = EReadMagicChantState::Chanting;
			//bIsChanting = true;
			if(OnReadMagicStateChanged.IsBound())
			{
				//开始魔法吟唱
				OnReadMagicStateChanged.Broadcast(EReadMagicChantState::ChantingStart);
			}
		}

		if(ReadMagicChantState == EReadMagicChantState::CompleteChant) return;
		UGameInstance* GameInstance = GetWorld()->GetGameInstance();
		if(!GameInstance) return;
		USkillManager* SkillManager = GameInstance->GetSubsystem<USkillManager>();
		if(!SkillManager) return;
		
		FMagicRowStruct* MagicRow = SkillManager->GetTableRowsMapRowById<FMagicRowStruct>("MagicInfoTable", SkillManager->GetEquipedMagicAbilityId());
		if(MagicRow)
		{
			if(CurrentChantTime >= MagicRow->ChantTime)
			{
				if(OnStopMagicChantState.IsBound())
				{
					//完成魔法吟唱
					OnStopMagicChantState.Broadcast(EStopReadMagicState::CompleteChant);
					ReadMagicChantState = EReadMagicChantState::CompleteChant;
				}
			}
		}
	}
}

void UWerewolf_MagicSystemComponent::CancelMagicChant(bool bForced)
{
	AWerewolf_HeroPlayerState* HeroPlayerState = Cast<AWerewolf_HeroPlayerState>(GetOwner());
	AWerewolf_Hero* EnemyPawn = Cast<AWerewolf_Hero>(GetOwner());
	// && HeroPlayerState->GetPawn()->IsLocallyControlled()
	if((HeroPlayerState && HeroPlayerState->GetPawn() && HeroPlayerState->GetPawn()->IsLocallyControlled()) || (EnemyPawn && EnemyPawn->HasAuthority()))
	{
		if(ReadMagicChantState == EReadMagicChantState::Chanting)
		{
			UGameInstance* GameInstance = GetWorld()->GetGameInstance();
			if(!GameInstance) return;
			USkillManager* SkillManager = GameInstance->GetSubsystem<USkillManager>();
			if(!SkillManager) return;
		
			FMagicRowStruct* MagicRow = SkillManager->GetTableRowsMapRowById<FMagicRowStruct>("MagicInfoTable", SkillManager->GetEquipedMagicAbilityId());
			if(MagicRow)
			{
				if(CurrentChantTime < MagicRow->ChantTime)
				{
					if(OnStopMagicChantState.IsBound())
					{
						//打断吟唱
						OnStopMagicChantState.Broadcast(EStopReadMagicState::InterruptChant);
					}
				}
			}
		}
		else if(ReadMagicChantState == EReadMagicChantState::CompleteChant)
		{
			if(bForced)
			{
				if(OnStopMagicChantState.IsBound())
				{
					//强制打断吟唱
					OnStopMagicChantState.Broadcast(EStopReadMagicState::InterruptChant);
				}
			}
			else
			{
				if(OnStopMagicChantState.IsBound())
				{
					//释放吟唱技能
					OnStopMagicChantState.Broadcast(EStopReadMagicState::ReleaseChantSkill);
				}	
			}
		}
		
		CurrentChantTime = 0;
		ReadMagicChantState = EReadMagicChantState::NotChanting;
		if(OnReadMagicStateChanged.IsBound())
		{
			OnReadMagicStateChanged.Broadcast(EReadMagicChantState::ChantingEnd);
		}
	}
}

void UWerewolf_MagicSystemComponent::OnRep_UpdateLearnedMagicInfoArray()
{
	LearnedMagicLinkMap.Empty();
	for (FLearnedMagicDataWrap& MagicDataWrap : LearnedMagicInfoArray)
	{
		LearnedMagicLinkMap.Emplace(MagicDataWrap.MagicAbilityId, MagicDataWrap.GASpecHandle);
	}

	if(OnLearnedMagic.IsBound())
	{
		OnLearnedMagic.Broadcast();
	}
	/*UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	if(GameInstance)
	{
		USkillManager* SkillManager = GameInstance->GetSubsystem<USkillManager>();
		if(SkillManager)
		{
			if(SkillManager->OnMagicLearned.IsBound())
			{
				SkillManager->OnMagicLearned.Broadcast();
			}
		}
	}*/
}

void UWerewolf_MagicSystemComponent::OnRep_GroundTraceTargetActor()
{
	UE_LOG(LogTemp, Log, TEXT("OnRep_GroundTraceTargetActor: %s"), *GetNameSafe(GroundTraceTargetActor));
}

AGATA_SphereTrace* UWerewolf_MagicSystemComponent::GetSphereTraceTargetActor()
{
	if (SphereTraceTargetActor)
	{
		return SphereTraceTargetActor;
	}

	SphereTraceTargetActor = GetWorld()->SpawnActor<AGATA_SphereTrace>();
	SphereTraceTargetActor->SetOwner(GetOwner());
	return SphereTraceTargetActor;
}

AGameplayAbilityTargetActor_GroundTrace* UWerewolf_MagicSystemComponent::GetGroundTraceTargetActorByClass(
	TSubclassOf<AGameplayAbilityTargetActor_GroundTrace> TargetActorClass)
{
	if (GroundTraceTargetActor)
	{
		return GroundTraceTargetActor;
	}

	GroundTraceTargetActor = GetWorld()->SpawnActor<AGameplayAbilityTargetActor_GroundTrace>(TargetActorClass);
	GroundTraceTargetActor->SetOwner(GetOwner());
	return GroundTraceTargetActor;
}

AGATA_InPlace_GroundTrace* UWerewolf_MagicSystemComponent::GetInPlaceGroundTraceTargetActorByClass(TSubclassOf<AGATA_InPlace_GroundTrace> TargetActorClass)
{
	if (InPlaceGroundTraceTargetActor)
	{
		return InPlaceGroundTraceTargetActor;
	}

	InPlaceGroundTraceTargetActor = GetWorld()->SpawnActor<AGATA_InPlace_GroundTrace>(TargetActorClass);
	InPlaceGroundTraceTargetActor->SetOwner(GetOwner());
	return InPlaceGroundTraceTargetActor;
}

AGATA_Radius* UWerewolf_MagicSystemComponent::GetRadiusTargetActor()
{
	if (RadiusTargetActor)
	{
		return RadiusTargetActor;
	}

	RadiusTargetActor = GetWorld()->SpawnActor<AGATA_Radius>();
	RadiusTargetActor->SetOwner(GetOwner());
	return RadiusTargetActor;
}

void UWerewolf_MagicSystemComponent::GetAllLearnedMagicAbilitiesIds(TArray<int32>& ResultArray)
{
	LearnedMagicLinkMap.GenerateKeyArray(ResultArray);
}

FGameplayAbilitySpecHandle UWerewolf_MagicSystemComponent::GetCurrentEquipedMagicAbilitySpecHandle()
{
	return LearnedMagicLinkMap.FindRef(EquipedMagicAbilityId);
}

void UWerewolf_MagicSystemComponent::ServerSetEquipedMagicAbilityId_Implementation(int32 InEquipedMagicAbilityId)
{
	EquipedMagicAbilityId = InEquipedMagicAbilityId;
}
