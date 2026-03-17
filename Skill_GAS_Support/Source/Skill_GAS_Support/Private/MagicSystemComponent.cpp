#include "MagicSystemComponent.h"

#include "SkillSystemFunctionLibrary.h"
#include "TargetActor/GATA_InPlace_GroundTrace.h"
#include "TargetActor/GATA_Radius.h"
#include "TargetActor/GATA_SphereTrace.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystem/SkillManager.h"
#include "Net/UnrealNetwork.h"


UMagicSystemComponent::UMagicSystemComponent()
{
	SetIsReplicatedByDefault(true);
}

void UMagicSystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UMagicSystemComponent, LearnedMagicInfoArray);
	DOREPLIFETIME(UMagicSystemComponent, EquipedMagicAbilityId);
	DOREPLIFETIME(UMagicSystemComponent, EquipedDarkMagicAbilityId);
	DOREPLIFETIME(UMagicSystemComponent, GroundTraceTargetActor);
}

bool UMagicSystemComponent::AddToLearnedMagicInfoArray(FLearnedMagicDataWrap& LearnedMagicData)
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

TArray<FLearnedMagicDataWrap>& UMagicSystemComponent::GetLearnedMagicInfoArray()
{
	return LearnedMagicInfoArray;
}

FGameplayAbilitySpecHandle UMagicSystemComponent::GetLearnedMagicGASpecHandleByMagicId(int32 MagicId)
{
	return LearnedMagicLinkMap.FindRef(MagicId);
}

void UMagicSystemComponent::FillMagicChantTime(float DeltaTime)
{
	APlayerState* PlayerState = Cast<APlayerState>(GetOwner());
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	//&& HeroPlayerState->GetPawn()->IsLocallyControlled()
	if((PlayerState && PlayerState->GetPawn()->IsLocallyControlled()) || (Character && Character->HasAuthority()))
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

void UMagicSystemComponent::CancelMagicChant(bool bForced)
{
	APlayerState* PlayerState = Cast<APlayerState>(GetOwner());
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	// && HeroPlayerState->GetPawn()->IsLocallyControlled()
	if((PlayerState && PlayerState->GetPawn() && PlayerState->GetPawn()->IsLocallyControlled()) || (Character && Character->HasAuthority()))
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

void UMagicSystemComponent::OnRep_UpdateLearnedMagicInfoArray()
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

void UMagicSystemComponent::OnRep_GroundTraceTargetActor()
{
	UE_LOG(LogTemp, Log, TEXT("OnRep_GroundTraceTargetActor: %s"), *GetNameSafe(GroundTraceTargetActor));
}

AGATA_SphereTrace* UMagicSystemComponent::GetSphereTraceTargetActor()
{
	if (SphereTraceTargetActor)
	{
		return SphereTraceTargetActor;
	}

	SphereTraceTargetActor = GetWorld()->SpawnActor<AGATA_SphereTrace>();
	SphereTraceTargetActor->SetOwner(GetOwner());
	return SphereTraceTargetActor;
}

AGameplayAbilityTargetActor_GroundTrace* UMagicSystemComponent::GetGroundTraceTargetActorByClass(
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

AGATA_InPlace_GroundTrace* UMagicSystemComponent::GetInPlaceGroundTraceTargetActorByClass(TSubclassOf<AGATA_InPlace_GroundTrace> TargetActorClass)
{
	if (InPlaceGroundTraceTargetActor)
	{
		return InPlaceGroundTraceTargetActor;
	}

	InPlaceGroundTraceTargetActor = GetWorld()->SpawnActor<AGATA_InPlace_GroundTrace>(TargetActorClass);
	InPlaceGroundTraceTargetActor->SetOwner(GetOwner());
	return InPlaceGroundTraceTargetActor;
}

AGATA_Radius* UMagicSystemComponent::GetRadiusTargetActor()
{
	if (RadiusTargetActor)
	{
		return RadiusTargetActor;
	}

	RadiusTargetActor = GetWorld()->SpawnActor<AGATA_Radius>();
	RadiusTargetActor->SetOwner(GetOwner());
	return RadiusTargetActor;
}

void UMagicSystemComponent::GetAllLearnedMagicAbilitiesIds(TArray<int32>& ResultArray)
{
	LearnedMagicLinkMap.GenerateKeyArray(ResultArray);
}

FGameplayAbilitySpecHandle UMagicSystemComponent::GetCurrentEquipedMagicAbilitySpecHandle(EMagicType MagicType)
{
	int32 ReturnMagicId = MagicType == EMagicType::Normal ? EquipedMagicAbilityId : EquipedDarkMagicAbilityId;
	return LearnedMagicLinkMap.FindRef(ReturnMagicId);
}

void UMagicSystemComponent::ServerSetEquipedMagicAbilityId_Implementation(int32 InEquipedMagicAbilityId)
{
	FMagicRowStruct MagicRowInfo = USkillSystemFunctionLibrary::GetMagicInfoBySkillId(this, InEquipedMagicAbilityId);
	if (MagicRowInfo.IsValid())
	{
		if (MagicRowInfo.MagicType == EMagicType::Normal)
		{
			EquipedMagicAbilityId = InEquipedMagicAbilityId;		
		}
		else if (MagicRowInfo.MagicType == EMagicType::Dark)
		{
			EquipedDarkMagicAbilityId = InEquipedMagicAbilityId;
		}
	}
}
