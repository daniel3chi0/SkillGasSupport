// Fill out your copyright notice in the Description page of Project Settings.
#include "Subsystem/SkillManager.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"
#include "Abilities/GameplayAbility.h"
#include "ReadMagicAbilityArgument.h"
#include "SkillAttributeSet.h"
#include "SkillSystemFunctionLibrary.h"
#include "SGSAbilitySystemComponent.h"
#include "MagicSystemComponent.h"
#include "MagicSystemInterface.h"
#include "SGSGameplayTags.h"
#include "Abilities/SGSAbility.h"
#include "TargetActor/SGS_GATargetDataType.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
//#include "GameModifier/GameModificationManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
//#include "Struct/MsgStruct/Werewolf_HttpMsgStruct.h"
#include "SGSGameplayTags.h"

DEFINE_LOG_CATEGORY(LogSkillSystem)

void USkillManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	TMap<FName, FString> SkillTableMap
	{
		{"SkillTable", "/Game/Program/GAS/Data/DT_Skill.DT_Skill"},
		{"BattleSkillInfoTable", "DataTable'/Game/Program/GAS/Data/DT_BattleSkillInfo.DT_BattleSkillInfo'"},
		{"MagicInfoTable", "DataTable'/Game/Program/GAS/Data/DT_MagicInfo.DT_MagicInfo'"},
		{"MagicActionTable", "DataTable'/Game/Program/GAS/Data/DT_MagicAction.DT_MagicAction'"},
		{"InitBuffTable", "DataTable'/Game/Program/GAS/Data/DT_InitBuff.DT_InitBuff'"},
	};

	for (auto& Pair : SkillTableMap)
	{
		LoadDataTable(Pair.Key, Pair.Value);
	}

	Test();

	FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &USkillManager::CheatTest);
	UAbilitySystemGlobals::Get().InitGlobalData();
	
	//CheatTest();
}

void USkillManager::Deinitialize()
{
	Super::Deinitialize();
	if (GetWorld()->GetNetMode() >= NM_Client)
	{
		//if (GIsEditor && IsTemplate()) return;
		
		if (IConsoleObject* ConsoleObject = IConsoleManager::Get().FindConsoleObject(TEXT("GetAllMagicAndMagicStaff")))
		{
			IConsoleManager::Get().UnregisterConsoleObject(ConsoleObject);
			UE_LOG(LogTemp, Log, TEXT("GetAllMagicAndMagicStaff Cheat Command remove"));
			ConsoleObject = nullptr;
		}

		if (IConsoleObject* ConsoleObject = IConsoleManager::Get().FindConsoleObject(TEXT("AddAllPredictionGE")))
		{
			IConsoleManager::Get().UnregisterConsoleObject(ConsoleObject);
			UE_LOG(LogTemp, Log, TEXT("AddAllPredictionGE Cheat Command remove"));
			ConsoleObject = nullptr;
		}
	}
}

bool USkillManager::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

USkillManager::USkillManager()
{
	
}

UDataTable* USkillManager::GetTableByName(FName Name) const
{
	return SkillRelatedTables.FindRef(Name);
}

void USkillManager::Test()
{
	UDataTable* DT = GetTableByName("SkillTable");
	
	FSkillRowStruct* a = FindRowByCondition<FSkillRowStruct>(DT, [](FSkillRowStruct RD)
	{
		return RD.SkillId == 1;
	});

	UE_LOG(LogTemp, Log, TEXT("SkillDetail = %s"), *a->SkillDetail.ToString());

	int32 SkillId = 2; 
	FSkillRowStruct* b = GetTableRowsMapRowById<FSkillRowStruct>("SkillTable", 2);
	UE_LOG(LogTemp, Log, TEXT("Get SkillMap Detail = %s"), *b->SkillDetail.ToString());
}

void USkillManager::CheatTest(UWorld* World, const UWorld::InitializationValues IVS)
{
	/*if (GetWorld()->GetNetMode() >= NM_Client)
	{
		//if (GIsEditor && IsTemplate()) return;

		UE_LOG(LogTemp, Log, TEXT("Sys IsTemplate = %s"), *UKismetStringLibrary::Conv_BoolToString(IsTemplate()));
		UE_LOG(LogTemp, Log, TEXT("GEditor = %s"), *UKismetStringLibrary::Conv_BoolToString(GIsEditor));
		
		if (!IConsoleManager::Get().FindConsoleObject(TEXT("GetAllMagicAndMagicStaff")))
		{
			IConsoleManager::Get().RegisterConsoleCommand(
			TEXT("GetAllMagicAndMagicStaff"),
			TEXT("学习所有魔法和获取一个法杖"),
			FConsoleCommandDelegate::CreateLambda([this]()
			{
				UE_LOG(LogTemp, Log, TEXT("GetAllMagicAndMagicStaff Test"));
				APawn* Pawn = UGameplayStatics::GetPlayerPawn(this->GetWorld(),0);
				if (!Pawn) return;
				if (ACharacter_Hero* Hero = Cast<ACharacter_Hero>(Pawn))
				{
					Hero->ServerGiveAllMagicAbility();
					APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
					AGameModificationManager* GameModificationManager = UWerewolf_CommonBlueprintFunctionLibrary::GetGameModificationManager(PC);
					if (!GameModificationManager) return;
					UFunction* Func = GameModificationManager->GetClass()->FindFunctionByName("SetItem");
					if(Func)
					{
						struct FParams
						{
							FString ItemID;
							uint8 Number;
						};
						FParams Params { "WP_008", 1 };
						GameModificationManager->ProcessEvent(Func, &Params);
					}
				}
			}),
			ECVF_Default);
			UE_LOG(LogTemp, Log, TEXT("Get All Magic Cheat Command Add"));
		}

		if (!IConsoleManager::Get().FindConsoleObject(TEXT("AddAllPredictionGE")))
		{
			
			TestAbility = LoadObject<UClass>(nullptr, *FString("Blueprint'/Game/Program/GAS/SkillSystem/Magic/GA_AddAllPredictionGE.GA_AddAllPredictionGE_C'"));
			IConsoleManager::Get().RegisterConsoleCommand(
			TEXT("AddAllPredictionGE"),
			TEXT("加入所有预测的GE"),
			FConsoleCommandDelegate::CreateLambda([&, this]()
			{
				UE_LOG(LogTemp, Log, TEXT("AddAllPredictionGE Test"));
				ENetMode NetMode = this->GetWorld()->GetNetMode();
				UE_LOG(LogTemp, Log, TEXT("NetMode = %d"), NetMode);
				APawn* Pawn = UGameplayStatics::GetPlayerPawn(this->GetWorld(),0);
				if (!Pawn) return;
				if (ACharacter* Hero = Cast<ACharacter>(Pawn))
				{
					USGSAbilitySystemComponent* WASC = Hero->FindComponentByClass<USGSAbilitySystemComponent>();
					if (WASC && Hero->IsLocallyControlled())
					{
						//FGameplayAbilitySpec GASpec(TestAbility);
						//WASC->GiveAbilityAndActivateOnce()
						//在蓝图的werewolf_hero beginplay后赋予
						WASC->TryActivateAbilityByClass(TestAbility);
					}
				}
				//USkillSystemFunctionLibrary::GiveMagicAbilityById(GetWorld(),);
			}),
			ECVF_Default);
			UE_LOG(LogTemp, Log, TEXT("Add All Prediction GE Cheat Command Add"));
		}

		UE_LOG(LogTemp, Log, TEXT("Cheat Command Build"));
	}*/
}

void USkillManager::LoadDataTable(FName TableName, const FString& Path)
{
	TObjectPtr<UDataTable> DT = LoadObject<UDataTable>(nullptr, *Path);
	if(!DT)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s Can not find in path: %s"), *TableName.ToString(), *Path);
		return;
	}
	
	SkillRelatedTables.Emplace(TableName, DT);
	LoadDataTableRowsMap(TableName, DT);
}

void USkillManager::LoadDataTableRowsMap(FName TableName, UDataTable* DataTable)
{
	TMap<int32, uint8*> RowsMap;
	if(DataTable->GetRowStruct()->IsChildOf(FSkillRowStruct::StaticStruct()))
	{
		for (auto& Pair : DataTable->GetRowMap())
		{
			FSkillRowStruct* RowData = reinterpret_cast<FSkillRowStruct*>(Pair.Value);	
			RowsMap.Emplace(RowData->SkillId, Pair.Value);
		}
	}
	else if(DataTable->GetRowStruct()->IsChildOf(FBattleSkillRowStruct::StaticStruct()))
	{
		for (auto& Pair : DataTable->GetRowMap())
		{
			FBattleSkillRowStruct* RowData = reinterpret_cast<FBattleSkillRowStruct*>(Pair.Value);	
			RowsMap.Emplace(RowData->SkillId, Pair.Value);
		}
	}
	else if(DataTable->GetRowStruct()->IsChildOf(FMagicRowStruct::StaticStruct()))
	{
		for (auto& Pair : DataTable->GetRowMap())
		{
			FMagicRowStruct* RowData = reinterpret_cast<FMagicRowStruct*>(Pair.Value);	
			RowsMap.Emplace(RowData->SkillId, Pair.Value);
		}
	}
	else if(DataTable->GetRowStruct()->IsChildOf(FInitialBuffRowStruct::StaticStruct()))
	{
		for (auto& Pair : DataTable->GetRowMap())
		{
			FInitialBuffRowStruct* RowData = reinterpret_cast<FInitialBuffRowStruct*>(Pair.Value);	
			RowsMap.Emplace((int32)RowData->Career, Pair.Value);
		}
	}

	SkillRelatedTablesRowsMap.Emplace(TableName, RowsMap);
}

/*template <typename T>
T* USkillManager::GetTableRowsMapRowById(FName TableName, int32 Id)
{
	TMap<int32, uint8*> TableRowsMap = SkillRelatedTablesRowsMap.FindRef(TableName);
	return reinterpret_cast<T*>(TableRowsMap.FindRef(Id));
}
*/

template FMagicRowStruct* USkillManager::GetTableRowsMapRowById<FMagicRowStruct>(FName TableName, int32 Id);

template<typename T>
T* USkillManager::FindRowByCondition(UDataTable* DataTable, auto Condition)
{
	for (auto& Pair : DataTable->GetRowMap())
	{
		T* RowData = reinterpret_cast<T*>(Pair.Value);
		if (RowData && Condition(*RowData))
		{
			return RowData;
		}
	}
	return nullptr;
}

template <typename T>
TArray<T*> USkillManager::FindRowsByCondition(UDataTable* DataTable, auto Condition)
{
	TArray<T*> RowsData;
	for (auto& Pair : DataTable->GetRowMap())
	{
		T* RowData = reinterpret_cast<T*>(Pair.Value);
		if (RowData && Condition(*RowData))
		{
			RowsData.Emplace(RowData);
		}
	}
	
	return RowsData;
}

FSkillRowStruct USkillManager::GetInitedSkillInfoByUIType(ESkillUIStateType UISkillType)
 {
	FSkillRowStruct* Info = InitedBattleSkillInfo.FindRef(UISkillType);
	if(Info)
	{
		return *Info;
	}
	else
	{
		return FSkillRowStruct();
	}
}

void USkillManager::SetCurrentActivatedSkillTags(const FGameplayTagContainer& Tags)
{
	CurrentActivatedSkillTags.Reset();
	CurrentActivatedSkillTags.AppendTags(Tags);
}

void USkillManager::SkillInternalInputCancel(UAbilitySystemComponent* ASC)
{
	if(ASC)
	{
		ASC->CancelAbilities(&CurrentActivatedSkillTags);
	}
}

float USkillManager::GetSkillConfigCooldownValueById(int32 SkillId)
{
	FSkillRowStruct* SRow = GetTableRowsMapRowById<FSkillRowStruct>("SkillTable", SkillId);
	if(SRow)
	{
		return SRow->CoolDownTime;
	}
	
	return -1.f;
}

UMagicSystemComponent* USkillManager::GetLocalPlayerMagicSystemComponent()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0.f);
	if(PC)
	{
		APlayerState* PS = PC->GetPlayerState<APlayerState>();
		if(PS)
		{
			UMagicSystemComponent* MSC = PS->GetComponentByClass<UMagicSystemComponent>();
			return MSC;
		}
	}
	return nullptr;
}

UMagicSystemComponent* USkillManager::GetPlayerMagicSystemComponent(AActor* Actor)
{
	const IMagicSystemInterface* MSI = Cast<IMagicSystemInterface>(Actor);
	if(MSI)
	{
		return MSI->GetMagicSystemComponent();
	}
	return nullptr;
}

bool USkillManager::EquipMagicAbilityByMagicId(int32 MagicAbilityId)
{
	UMagicSystemComponent* MSC = GetLocalPlayerMagicSystemComponent();
	if(MSC)
	{
		CurrentEquipedMagicAbilitySpecHandle = MSC->GetLearnedMagicGASpecHandleByMagicId(MagicAbilityId);
		EquipedMagicAbilityId = MagicAbilityId;
		//黑白魔法通用
		if(OnMagicEquiped.IsBound())
		{
			OnMagicEquiped.Broadcast(MagicAbilityId);
		}

		/*APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		if(PC)
		{
			UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PC->GetPawn());
			bool bHasMagicRead = USkillSystemFunctionLibrary::HasGivedAbilityByTag(ASC,FGameplayTag::RequestGameplayTag("Event.Trigger.Magic.MagicReading"));
			if(!bHasMagicRead)
			{
				//ASC->GiveAbility()
			}
		}*/
		MSC->ServerSetEquipedMagicAbilityId(MagicAbilityId);
		return CurrentEquipedMagicAbilitySpecHandle.IsValid();
	}

	return false;
}

bool USkillManager::EquipDarkMagicAbilityByDarkMagicId(int32 DarkMagicAbilityId)
{
	UMagicSystemComponent* MSC = GetLocalPlayerMagicSystemComponent();
	if(MSC)
	{
		CurrentEquipedDarkMagicAbilitySpecHandle = MSC->GetLearnedMagicGASpecHandleByMagicId(DarkMagicAbilityId);
		EquipedDarkMagicAbilityId = DarkMagicAbilityId;
		//黑白魔法通用
		if(OnMagicEquiped.IsBound())
		{
			OnMagicEquiped.Broadcast(DarkMagicAbilityId);
		}
		
		MSC->ServerSetEquipedMagicAbilityId(DarkMagicAbilityId);
		return CurrentEquipedDarkMagicAbilitySpecHandle.IsValid();
	}

	return false;
}

bool USkillManager::TryUseCurrentEquipedMagicAbility(UAbilitySystemComponent* ASC)
{
	if(ASC && CurrentEquipedMagicAbilitySpecHandle.IsValid() && EquipedMagicAbilityId != 0)
	{
		FMagicRowStruct* MagicRow = GetTableRowsMapRowById<FMagicRowStruct>("MagicInfoTable", EquipedMagicAbilityId);
		if(!MagicRow) return false;
		
		AActor* Avatar = ASC->GetAvatarActor();
		FGameplayEventData EventData;
		/*UReadMagicAbilityArgument* ReadMagicAbilityArgument = NewObject<UReadMagicAbilityArgument>();
		ReadMagicAbilityArgument->MagicRow = *MagicRow;
		ReadMagicAbilityArgument->CurrentEquipedMagicSpecHandle = CurrentEquipedMagicAbilitySpecHandle;
		EventData.OptionalObject = ReadMagicAbilityArgument;*/
		FGameplayAbilityTargetData_ActorsOrHitResult* TargetData_ActorsOrHitResult = new FGameplayAbilityTargetData_ActorsOrHitResult();
		TargetData_ActorsOrHitResult->MagicRow = *MagicRow;
		TargetData_ActorsOrHitResult->CurrentEquipedMagicSpecHandle = CurrentEquipedMagicAbilitySpecHandle;
		FGameplayAbilityTargetDataHandle TargetDataHandle(TargetData_ActorsOrHitResult);
		EventData.TargetData = TargetDataHandle;
		return USkillSystemFunctionLibrary::SendGameplayEventToActorByReturn(Avatar, FGameplayTag::RequestGameplayTag("Event.Trigger.Magic.MagicReading"), EventData);
		//MagicRow->MagicReleaseType
		//return ASC->TryActivateAbility(CurrentEquipedMagicAbilitySpecHandle);
	}
	return false;
}

bool USkillManager::TryUseCurrentEquipedDarkMagicAbility(UAbilitySystemComponent* ASC)
{
	if(ASC)
	{
		if(CurrentEquipedDarkMagicAbilitySpecHandle.IsValid() && EquipedDarkMagicAbilityId != 0)
		{
			FGameplayTag EquipedDarkMagicTriggerTags = GetAbilityTriggerTagFromSpecHandle(ASC, CurrentEquipedDarkMagicAbilitySpecHandle);
			//ASC->TryActivateAbilitiesByTag(EquipedDarkMagicTriggerTags);
			FGameplayEventData EventData;
			return USkillSystemFunctionLibrary::SendGameplayEventToActorByReturn(ASC->GetAvatarActor(), EquipedDarkMagicTriggerTags, EventData);
		}
	}
	return false;
}

USGSAbility* USkillManager::GetCurrentEquipedMagicClass(UAbilitySystemComponent* ASC)
{
	if(!ASC) return nullptr;
	
	if(CurrentEquipedMagicAbilitySpecHandle.IsValid())
	{
		FGameplayAbilitySpec* GASpec = ASC->FindAbilitySpecFromHandle(CurrentEquipedMagicAbilitySpecHandle);
		if(GASpec)
		{
			return Cast<USGSAbility>(GASpec->Ability);
			/*if(WerewolfAbility)
			{
				const FGameplayTagContainer* TagContainer = WerewolfAbility->GetCooldownTags();
			}*/
		}
	}
	else
	{
		if(AActor* Owner = ASC->GetOwnerActor())
		{
			UMagicSystemComponent* MSC = GetPlayerMagicSystemComponent(Owner);
			if(!MSC) return nullptr;
			
			FGameplayAbilitySpecHandle MagicSpecHandle = MSC->GetCurrentEquipedMagicAbilitySpecHandle(EMagicType::Normal);
			if(MagicSpecHandle.IsValid())
			{
				FGameplayAbilitySpec* MagicAbilitySpec = ASC->FindAbilitySpecFromHandle(MagicSpecHandle);
				if(MagicAbilitySpec)
				{
					return Cast<USGSAbility>(MagicAbilitySpec->Ability);
				}
			}
		}
	}

	return nullptr;
}

USGSAbility* USkillManager::GetCurrentEquipedDarkMagicClass(UAbilitySystemComponent* ASC)
{
	if(!ASC) return nullptr;
	
	if(CurrentEquipedDarkMagicAbilitySpecHandle.IsValid())
	{
		FGameplayAbilitySpec* GASpec = ASC->FindAbilitySpecFromHandle(CurrentEquipedDarkMagicAbilitySpecHandle);
		if(GASpec)
		{
			return Cast<USGSAbility>(GASpec->Ability);
		}
	}
	else
	{
		if(AActor* Owner = ASC->GetOwnerActor())
		{
			UMagicSystemComponent* MSC = GetPlayerMagicSystemComponent(Owner);
			if(MSC)
			{
				FGameplayAbilitySpecHandle MagicSpecHandle = MSC->GetCurrentEquipedMagicAbilitySpecHandle(EMagicType::Dark);
				if(MagicSpecHandle.IsValid())
				{
					FGameplayAbilitySpec* MagicAbilitySpec = ASC->FindAbilitySpecFromHandle(MagicSpecHandle);
					if(MagicAbilitySpec)
					{
						return Cast<USGSAbility>(MagicAbilitySpec->Ability);
					}
				}
			}
		}
	}

	return nullptr;
}

FGameplayTagContainer USkillManager::GetCurrentEquipedMagicCooldownTags(UAbilitySystemComponent* ASC)
{
	USGSAbility* Ability = GetCurrentEquipedMagicClass(ASC);
	if(Ability)
	{
		return *(Ability->GetCooldownTags());
	}

	return FGameplayTagContainer();
}

FGameplayTagContainer USkillManager::GetCurrentEquipedDarkMagicCooldownTags(UAbilitySystemComponent* ASC)
{
	USGSAbility* Ability = GetCurrentEquipedDarkMagicClass(ASC);
	if(Ability)
	{
		return *(Ability->GetCooldownTags());
	}

	return FGameplayTagContainer();
}

FGameplayTagContainer USkillManager::GetCurrentEquipedMagicAbilityTags(UAbilitySystemComponent* ASC)
{
	USGSAbility* Ability = GetCurrentEquipedMagicClass(ASC);
	if(Ability)
	{
		return Ability->AbilityTags;
	}

	return FGameplayTagContainer();
}

FGameplayTagContainer USkillManager::GetCurrentEquipedDarkMagicAbilityTags(UAbilitySystemComponent* ASC)
{
	USGSAbility* Ability = GetCurrentEquipedDarkMagicClass(ASC);
	if(Ability)
	{
		return Ability->AbilityTags;
	}

	return FGameplayTagContainer();
}

FGameplayTag USkillManager::GetCurrentEquipedMagicAbilityTriggerTag(UAbilitySystemComponent* ASC)
{
	USGSAbility* Ability = GetCurrentEquipedMagicClass(ASC);
	if(Ability)
	{
		return Ability->GetFirstAbilityTriggersEventTag();
	}

	return FGameplayTag();
}

FGameplayTag USkillManager::GetCurrentEquipedDarkMagicAbilityTriggerTag(UAbilitySystemComponent* ASC)
{
	USGSAbility* Ability = GetCurrentEquipedDarkMagicClass(ASC);
	if(Ability)
	{
		return Ability->GetFirstAbilityTriggersEventTag();
	}

	return FGameplayTag();
}

FGameplayTag USkillManager::GetAbilityTriggerTagFromSpecHandle(UAbilitySystemComponent* ASC, FGameplayAbilitySpecHandle SpecHandle)
{
	if(ASC)
	{
		bool bIsInstance;
		const UGameplayAbility* Ability = UAbilitySystemBlueprintLibrary::GetGameplayAbilityFromSpecHandle(ASC, SpecHandle, bIsInstance);
		if(USGSAbility* Werewolf_Ability = const_cast<USGSAbility*>(Cast<USGSAbility>(Ability)))
		{
			return Werewolf_Ability->GetFirstAbilityTriggersEventTag();
		}
	}

	return FGameplayTag();
}

USGSAbility* USkillManager::GetPlayerLearnedMagicClassById(int32 MagicAbilityId)
{
	UMagicSystemComponent* MSC = GetLocalPlayerMagicSystemComponent();
	if(MSC)
	{
		FGameplayAbilitySpecHandle GASpecHandle = MSC->GetLearnedMagicGASpecHandleByMagicId(MagicAbilityId);
		if(!GASpecHandle.IsValid()) return nullptr;

		if(APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(),0))
		{
			if(APlayerState* PS = PC->GetPlayerState<APlayerState>())
			{
				if(UAbilitySystemComponent* ASC = PS->FindComponentByClass<UAbilitySystemComponent>())
				{
					FGameplayAbilitySpec* GASpec = ASC->FindAbilitySpecFromHandle(GASpecHandle);
					if(GASpec)
					{
						return Cast<USGSAbility>(GASpec->Ability);
					}
				}
			}
		}
	}
	return nullptr;
}

FGameplayTagContainer USkillManager::GetPlayerLearnedMagicCooldownTagsById(int32 MagicAbilityId)
{
	USGSAbility* MagicClass = GetPlayerLearnedMagicClassById(MagicAbilityId);
	if(MagicClass)
	{
		return *(MagicClass->GetCooldownTags());
	}

	return FGameplayTagContainer();
}

int32 USkillManager::GetEquipedMagicAbilityId()
{
	return EquipedMagicAbilityId;
}

void USkillManager::ClientAddCue(AActor* TargetActor, FGameplayTag CueTag, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters)
{
	/*if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
	{
		ASC->AddLooseGameplayTag(CueTag);
	}*/
	
	if (UGameplayCueManager* GCM = UAbilitySystemGlobals::Get().GetGameplayCueManager())
	{
		GCM->HandleGameplayCue(TargetActor, CueTag, EventType, Parameters);
		//GCM->HandleGameplayCue(TargetActor, CueTag, EGameplayCueEvent::WhileActive, Parameters);
	}
}

void USkillManager::ClientRemoveCue(AActor* TargetActor, FGameplayTag CueTag, const FGameplayCueParameters& Parameters)
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
	{
		ASC->RemoveLooseGameplayTag(CueTag);
	}

	if (UGameplayCueManager* GCM = UAbilitySystemGlobals::Get().GetGameplayCueManager())
	{
		GCM->HandleGameplayCue(TargetActor, CueTag, EGameplayCueEvent::Removed, Parameters);
	}
}

void USkillManager::InitHeroSkillEntry(ACharacter_Hero* Hero)
{
	if(Hero)
	{
		/*UE_LOG(LogTemp, Log, TEXT("affter SetIdentity : Skill init ------ %s ----------- net mode = %d -------------"
							"-heroname = %s --------- IsLocal = %s"), *UEnum::GetValueAsString(Hero->GetPawnExtCompoent()->GetCharacterIdentity().GetTrueFaction()),
			GetWorld()->GetNetMode(), *Hero->GetName(), (Hero->IsLocallyControlled() ? TEXT("true") : TEXT("false")));

		if(Hero->IsLocallyControlled())
		{
			InitedBattleSkillInfo.Empty();
		}*/
		//普通战技（现在每个职业表中一个， 后期改成进局前选择两个）
		//后期需要调整逻辑
		InitHeroBattleSkill(Hero);
		//狼人额外战技（固定两个，变身和透视）
		InitHeroFactionBattleSkill(Hero);
		//狼人额外魔法（狼人角色在对局前可选择, 获得方式为静态（不在游戏内获得），是否类型为魔法）
		//后期需要调整逻辑
		InitHeroFactionMagic(Hero);
		//初始化魔法行为
		InitHeroMagicAction(Hero);
		//初始化职业自带buff
		InitHeroInitBuff(Hero);
		
		/*if(Hero->GetLocalRole() == ROLE_Authority)
		{
			InitSkillAttributeSet(Hero->GetAbilitySystemComponent());
		}*/
	}
}

void USkillManager::InitSkillAttributeSet(UAbilitySystemComponent* ASC)
{
	USkillAttributeSet* SkillAttributes = NewObject<USkillAttributeSet>(ASC->GetOwner());
	ASC->AddAttributeSetSubobject(SkillAttributes);
}

void USkillManager::InitHeroBattleSkill(ACharacter_Hero* Hero)
{
	//普通战技用职业查询
	//------------Legacy------------
	/*if (!bUseOutRoomSkill)
	{
		LegacyInitHeroBattleSkill(Hero);
	}
	else
	{
		int32 OutRoomSkillId = GetOutRoomBattleSkillId(Hero);
		if (OutRoomSkillId == 0)
		{
			LegacyInitHeroBattleSkill(Hero);
			return;
		}
		FSkillRowStruct* SkillRow = GetTableRowsMapRowById<FSkillRowStruct>("SkillTable", OutRoomSkillId);
		if(!SkillRow) return;
	
		for (int i = 0; i < SkillRow->AbilityClasses.Num(); i++)
		{
			if (!SkillRow->AbilityClasses[i].IsNull())
			{
				if(Hero->IsLocallyControlled())
					InitedBattleSkillInfo.Emplace(ESkillUIStateType::BattleSkill, SkillRow);
			
				FSoftObjectPath Path = SkillRow->AbilityClasses[i].ToSoftObjectPath();
				UAbilitySystemComponent* ASC = Hero->GetWerewolfAbilitySystemComponent();
				ESkillGameType SkillGameType = ESkillGameType::BattleSkill;
				bool bIsRootSkill = i == 0;
				UAssetManager::GetStreamableManager().RequestAsyncLoad(Path,
					FStreamableDelegate::CreateUObject(this, &USkillManager::OnLoadAbilityClassFinished, Path, ASC, SkillRow, SkillGameType, bIsRootSkill));
			}
		}
	}*/
}

void USkillManager::LegacyInitHeroBattleSkill(ACharacter_Hero* Hero)
{
	/*EPlayerCareer HeroCareer = Hero->PlayerCareer;
	UDataTable* BattleSkillDT = GetTableByName("BattleSkillInfoTable");
	if(!BattleSkillDT) return;
	
	FBattleSkillRowStruct* BSRow = FindRowByCondition<FBattleSkillRowStruct>(BattleSkillDT, [&HeroCareer](FBattleSkillRowStruct RD)
	{
		return RD.Career == HeroCareer;
	});

	UDataTable* SkillDT = GetTableByName("SkillTable");
	if(!SkillDT) return;
	FSkillRowStruct* SRow = FindRowByCondition<FSkillRowStruct>(SkillDT, [BSRow](FSkillRowStruct RD)
	{
		return RD.SkillId == BSRow->SkillId;
	});

	//if(SRow && !SRow->AbilityClass.IsNull()) // TO OP
	if(!SRow) return;
	
	for (int i = 0; i < SRow->AbilityClasses.Num(); i++)
	{
		if (!SRow->AbilityClasses[i].IsNull())
		{
			if(Hero->IsLocallyControlled())
				InitedBattleSkillInfo.Emplace(ESkillUIStateType::BattleSkill, SRow);
			
			FSoftObjectPath Path = SRow->AbilityClasses[i].ToSoftObjectPath();
			UAbilitySystemComponent* ASC = Hero->GetWerewolfAbilitySystemComponent();
			ESkillGameType SkillGameType = ESkillGameType::BattleSkill;
			bool bIsRootSkill = i == 0;
			UAssetManager::GetStreamableManager().RequestAsyncLoad(Path, FStreamableDelegate::CreateUObject(this, &USkillManager::OnLoadAbilityClassFinished, Path, ASC, SRow, SkillGameType, bIsRootSkill));
		}
	}*/
}

int32 USkillManager::GetOutRoomBattleSkillId(ACharacter_Hero* Hero) const
{
	/*AWerewolf_GameStateBase* GS = Cast<AWerewolf_GameStateBase>(GetWorld()->GetGameState());
	if(!GS) return 0;
	ACharacter_HeroPlayerState* PS = Cast<ACharacter_HeroPlayerState>(Hero->GetPlayerState());
	if(!PS) return 0;
	for (const FCharacterData& CharacterData : GS->OutGameCharactersData)
	{
		if (CharacterData.SteamId == PS->GetSteamID())
		{
			return FCString::Atoi(*CharacterData.CharacterSelectedAbilityId);
		}
	}*/
	
	return 0;
}

void USkillManager::InitHeroFactionBattleSkill(ACharacter_Hero* Hero)
{
	/*EOwnerFaction HeroFaction = Hero->GetIdentity().GetTrueFaction();
	if(HeroFaction != EOwnerFaction::Bad) return;
	
	UDataTable* SkillDT = GetTableByName("SkillTable");
	if(!SkillDT) return;
	
	TArray<FSkillRowStruct*> SRows = FindRowsByCondition<FSkillRowStruct>(SkillDT, [](FSkillRowStruct RD)
	{
		return RD.SkillGameType == ESkillGameType::BattleSkill && RD.SkillObtainType == ESkillObtainType::StaticSkill && RD.ExclusiveFaction == EOwnerFaction::BadGuy;
	});

	//狼人专属战技用身份查
	//有多个
	for (auto SRow : SRows)
	{
		//if(!SRow->AbilityClass.IsNull())//TO OP
		for (int i = 0; i < SRow->AbilityClasses.Num(); i++)
		{
			if(!SRow->AbilityClasses[i].IsNull())
			{
				if(Hero->IsLocallyControlled())
				{
					FBattleSkillRowStruct* BSRow = GetTableRowsMapRowById<FBattleSkillRowStruct>("BattleSkillInfoTable", SRow->SkillId);
					if(BSRow && BSRow->ExtraBattleSkillSlotIndex == EVillainExtraBattleSkillSlotIndex::SlotOne && i == 0)
					{
						InitedBattleSkillInfo.Emplace(ESkillUIStateType::ExtraBattleSkill1, SRow);
					}
					else if(BSRow && BSRow->ExtraBattleSkillSlotIndex == EVillainExtraBattleSkillSlotIndex::SlotTwo && i == 0)
					{
						InitedBattleSkillInfo.Emplace(ESkillUIStateType::ExtraBattleSkill2, SRow);
					}
				}
			
				FSoftObjectPath Path = SRow->AbilityClasses[i].ToSoftObjectPath();
				UAbilitySystemComponent* ASC = Hero->GetWerewolfAbilitySystemComponent();
				ESkillGameType SkillGameType = ESkillGameType::BattleSkill;
				bool bIsRootSkill = i == 0;
				UAssetManager::GetStreamableManager().RequestAsyncLoad(Path, FStreamableDelegate::CreateUObject(this, &USkillManager::OnLoadAbilityClassFinished, Path, ASC, SRow, SkillGameType, bIsRootSkill));
			}
		}
	}*/
}

void USkillManager::InitHeroFactionMagic(ACharacter_Hero* Hero)
{
	//------------Legacy------------
	/*if (!bUseOutRoomSkill)
	{
		LegacyInitHeroFactionMagic(Hero);
	}
	else
	{
		EOwnerFaction HeroFaction = Hero->GetIdentity().GetTrueFaction();
		if(HeroFaction != EOwnerFaction::Bad) return;
		
		int32 OutRoomFactionMagicId = GetOutRoomFactionMagicId(Hero);
		if (OutRoomFactionMagicId == 0)
		{
			LegacyInitHeroFactionMagic(Hero);
			return;
		}
		FSkillRowStruct* SkillRow = GetTableRowsMapRowById<FSkillRowStruct>("SkillTable", OutRoomFactionMagicId);
		if(!SkillRow) return;
	
		//if(!SRow->AbilityClass.IsNull())//To op
		for (int i = 0; i < SkillRow->AbilityClasses.Num(); i++)
		{
			if(!SkillRow->AbilityClasses[i].IsNull())
			{
				FSoftObjectPath Path = SkillRow->AbilityClasses[i].ToSoftObjectPath();
				UAbilitySystemComponent* ASC = Hero->GetWerewolfAbilitySystemComponent();
				ESkillGameType SkillGameType = ESkillGameType::Magic;
				bool bIsRootSkill = i == 0;
				UAssetManager::GetStreamableManager().RequestAsyncLoad(Path,
					FStreamableDelegate::CreateUObject(this, &USkillManager::OnLoadAbilityClassFinished, Path, ASC, SkillRow, SkillGameType, bIsRootSkill));	
			}
		}
	}*/
}

void USkillManager::LegacyInitHeroFactionMagic(ACharacter_Hero* Hero)
{
	/*EOwnerFaction HeroFaction = Hero->GetIdentity().GetTrueFaction();
	if(HeroFaction != EOwnerFaction::Bad) return;
	
	UDataTable* SkillDT = GetTableByName("SkillTable");
	if(!SkillDT) return;
	
	TArray<FSkillRowStruct*> SRows = FindRowsByCondition<FSkillRowStruct>(SkillDT, [](FSkillRowStruct RD)
	{
		return RD.SkillGameType == ESkillGameType::Magic && RD.SkillObtainType == ESkillObtainType::StaticSkill && RD.ExclusiveFaction == EOwnerFaction::BadGuy;
	});

	//狼人专属魔法和专属战技差不多分类下debug用
	//有多个
	for (auto SRow : SRows)
	{
		//if(!SRow->AbilityClass.IsNull())//To op
		for (int i = 0; i < SRow->AbilityClasses.Num(); i++)
		{
			if(!SRow->AbilityClasses[i].IsNull())
			{
				FSoftObjectPath Path = SRow->AbilityClasses[i].ToSoftObjectPath();
				UAbilitySystemComponent* ASC = Hero->GetWerewolfAbilitySystemComponent();
				ESkillGameType SkillGameType = ESkillGameType::Magic;
				bool bIsRootSkill = i == 0;
				UAssetManager::GetStreamableManager().RequestAsyncLoad(Path, FStreamableDelegate::CreateUObject(this, &USkillManager::OnLoadAbilityClassFinished, Path, ASC, SRow, SkillGameType, bIsRootSkill));	
			}
		}
	}*/
}

int32 USkillManager::GetOutRoomFactionMagicId(ACharacter_Hero* Hero) const
{
	/*AWerewolf_GameStateBase* GS = Cast<AWerewolf_GameStateBase>(GetWorld()->GetGameState());
	if(!GS) return 0;
	ACharacter_HeroPlayerState* PS = Cast<ACharacter_HeroPlayerState>(Hero->GetPlayerState());
	if(!PS) return 0;
	for (const FUsrData& UserData : GS->OutGameUsrData)
	{
		if (UserData.SteamId == PS->GetSteamID())
		{
			return FCString::Atoi(*UserData.PlayerAbilityId);
		}
	}*/
	
	return 0;
}

void USkillManager::InitHeroMagicAction(ACharacter_Hero* Hero)
{
	/*UDataTable* MagicActionTable = GetTableByName("MagicActionTable");
	if(!MagicActionTable) return;
	
	for (auto Pair : MagicActionTable->GetRowMap())
	{
		FMagicActionRowStruct* MagicActionRow = reinterpret_cast<FMagicActionRowStruct*>(Pair.Value);
		if(!MagicActionRow) return;
		
		for (int i = 0; i < MagicActionRow->AbilityClasses.Num(); i++)
		{
			if(!MagicActionRow->AbilityClasses[i].IsNull())
			{
				FSoftObjectPath Path = MagicActionRow->AbilityClasses[i].ToSoftObjectPath();
				UAbilitySystemComponent* ASC = Hero->GetWerewolfAbilitySystemComponent();
				UAssetManager::GetStreamableManager().RequestAsyncLoad(Path, FStreamableDelegate::CreateUObject(this, &USkillManager::LoadAndGiveMagicAction, Path, ASC));
			}
		}
	}*/
}

void USkillManager::InitHeroInitBuff(ACharacter_Hero* Hero)
{
	/*FInitialBuffRowStruct* InitBuffRow = GetTableRowsMapRowById<FInitialBuffRowStruct>("InitBuffTable", (int32)Hero->PlayerCareer);
	if(!InitBuffRow) return;
	
	for (TSoftClassPtr<UGameplayEffect>& Buff : InitBuffRow->InitBuffs)
	{
		UAbilitySystemComponent* ASC = Hero->GetAbilitySystemComponent();
		if(ASC)
		{
			FSoftObjectPath Path = Buff.ToSoftObjectPath();
			UAssetManager::GetStreamableManager().RequestAsyncLoad(Path, FStreamableDelegate::CreateUObject(this, &USkillManager::LoadInitBuff, Path, ASC));
		}
	}*/
}

void USkillManager::OnLoadAbilityClassFinished(FSoftObjectPath Path, UAbilitySystemComponent* ASC, FSkillRowStruct* SRow, const ESkillGameType SkillGameType, bool bIsRootSkill)
{
	if(UObject* RO = Path.ResolveObject())
	{
		UClass* AbilityClass = Cast<UClass>(RO);
		if(AbilityClass)
		{
			LoadedAbilitiesMap.Emplace(*SRow, AbilityClass);
		}

		if(ASC && ASC->IsOwnerActorAuthoritative())
		{
			GiveSkillToHero(ASC, AbilityClass, SRow, SkillGameType, bIsRootSkill);
		}

		if(OnSkillInited.IsBound())
		{
			OnSkillInited.Broadcast();
		}
	}
}

void USkillManager::GiveSkillToHero(UAbilitySystemComponent* ASC, UClass* SkillClass, FSkillRowStruct* SRow, const ESkillGameType& SkillGameType, bool bIsRootSkill)
{
	USGSAbility* SkillCDO = Cast<USGSAbility>(SkillClass->GetDefaultObject());
	if(SkillCDO && SRow)
	{
		SkillCDO->SkillId = SRow->SkillId;
	}
	else
	{
		return;
	}
	
	FGameplayAbilitySpec AbilitySpec(SkillCDO, 1);
	AbilitySpec.SourceObject = this;

	if(bIsRootSkill && SkillGameType == ESkillGameType::BattleSkill && SRow)
	{
		FBattleSkillRowStruct* BSRow = nullptr;
		UDataTable* BattleSkillDT = GetTableByName("BattleSkillInfoTable");
		if(BattleSkillDT)
		{
			BSRow = FindRowByCondition<FBattleSkillRowStruct>(BattleSkillDT, [&SRow](FBattleSkillRowStruct RD)
		   {
			   return RD.SkillId == SRow->SkillId;
		   });
		}

		//初始黑魔法不走这里的逻辑
		if(SRow->SkillGameType == ESkillGameType::BattleSkill)
		{
			if(SRow->ExclusiveFaction == EOwnerFaction::Bad)
			{
				if(BSRow && BSRow->ExtraBattleSkillSlotIndex == EVillainExtraBattleSkillSlotIndex::SlotOne)
				{
					AbilitySpec.DynamicAbilityTags.AddTag(FSGSGameplayTags::InputTag_SkillSystem_VillainExtraBattleSkill1);	
				}
				else if(BSRow && BSRow->ExtraBattleSkillSlotIndex == EVillainExtraBattleSkillSlotIndex::SlotTwo)
				{
					AbilitySpec.DynamicAbilityTags.AddTag(FSGSGameplayTags::InputTag_SkillSystem_VillainExtraBattleSkill2);
				}
			}
			else
			{
				AbilitySpec.DynamicAbilityTags.AddTag(FSGSGameplayTags::InputTag_SkillSystem_BattleSkill);
			}
		}
		
		//魔法类技能不需要加输入tag
		const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);
	}
	else if (bIsRootSkill && SkillGameType == ESkillGameType::Magic && SRow)
	{
		const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);
		UMagicSystemComponent* MSC = ASC->GetOwner()->GetComponentByClass<UMagicSystemComponent>();
		if(MSC)
		{
			FLearnedMagicDataWrap LearnedMagicDataWrap;
			LearnedMagicDataWrap.MagicAbilityId = SRow->SkillId;
			LearnedMagicDataWrap.GASpecHandle = AbilitySpecHandle;
			MSC->AddToLearnedMagicInfoArray(LearnedMagicDataWrap);
		}
	}
}

void USkillManager::LoadAndGiveMagicAction(FSoftObjectPath Path, UAbilitySystemComponent* ASC)
{
	if(UObject* RO = Path.ResolveObject())
	{
		UClass* AbilityClass = Cast<UClass>(RO);
		if(AbilityClass)
		{
			if(ASC->IsOwnerActorAuthoritative())
			{
				USGSAbility* SkillCDO = Cast<USGSAbility>(AbilityClass->GetDefaultObject());
				if(!SkillCDO) return;
				FGameplayAbilitySpec AbilitySpec(SkillCDO, 1);
				AbilitySpec.SourceObject = this;
				const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);
			}
		}
	}
}

void USkillManager::LoadInitBuff(FSoftObjectPath Path, UAbilitySystemComponent* ASC)
{
	if(UObject* RO = Path.ResolveObject())
	{
		UClass* EffectClass = Cast<UClass>(RO);
		if(EffectClass)
		{
			if(ASC && ASC->IsOwnerActorAuthoritative())
			{
				FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
				ContextHandle.AddSourceObject(this);
				FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(EffectClass, 1.0f, ContextHandle);
				ASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
			}
		}
	}
}
