// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "SkillStructType.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SkillManager.generated.h"

class USGSAbility;
class UMagicSystemComponent;
class UAbilitySystemComponent;
enum class ESkillObtainType : uint8;
class UGameplayAbility;
class ACharacter_Hero;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSkillSystemLoadDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSkillInitedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMagicLearnedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMagicEquipedDelegate, int32, MagicAbilityId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSkillActivateMsgDelegate, bool, bCanActivate, int32, SkillId, FString, Messages);
SKILL_GAS_SUPPORT_API DECLARE_LOG_CATEGORY_EXTERN(LogSkillSystem, Log, All);

UENUM(BlueprintType)
enum class ESkillUIStateType : uint8
{
	None = 0,
	BattleSkill = 1,
	ExtraBattleSkill1 = 2,	//v
	ExtraBattleSkill2 = 3,	//c
};

UCLASS()
class SKILL_GAS_SUPPORT_API USkillManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	
	USkillManager();
	
	UDataTable* GetTableByName(FName Name) const;

	template<typename T>
	T* GetTableRowsMapRowById(FName TableName, int32 Id)
	{
		TMap<int32, uint8*> TableRowsMap = SkillRelatedTablesRowsMap.FindRef(TableName);
		return reinterpret_cast<T*>(TableRowsMap.FindRef(Id));
	}

	void Test();
	
	void CheatTest(UWorld* World, const UWorld::InitializationValues IVS);

	void InitHeroSkillEntry(ACharacter_Hero* Hero);
	
	void InitHeroBattleSkill(ACharacter_Hero* Hero);

	void LegacyInitHeroBattleSkill(ACharacter_Hero* Hero);

	int32 GetOutRoomBattleSkillId(ACharacter_Hero* Hero) const;

	void InitHeroFactionBattleSkill(ACharacter_Hero* Hero);

	void InitHeroFactionMagic(ACharacter_Hero* Hero);

	void LegacyInitHeroFactionMagic(ACharacter_Hero* Hero);

	int32 GetOutRoomFactionMagicId(ACharacter_Hero* Hero) const;

	void InitHeroMagicAction(ACharacter_Hero* Hero);

	void InitHeroInitBuff(ACharacter_Hero* Hero);

	template<typename T>
	T* FindRowByCondition(UDataTable* DataTable, auto Condition);

	template<typename T>
	TArray<T*> FindRowsByCondition(UDataTable* DataTable, auto Condition);

	UFUNCTION(BlueprintCallable)
	FSkillRowStruct GetInitedSkillInfoByUIType(ESkillUIStateType UISkillType);

	UFUNCTION(BlueprintCallable)
	void SetCurrentActivatedSkillTags(const FGameplayTagContainer& Tags);

	void SkillInternalInputCancel(UAbilitySystemComponent* ASC);
	
	float GetSkillConfigCooldownValueById(int32 SkillId);

	//Magic
	UFUNCTION(BlueprintCallable)
	UMagicSystemComponent* GetLocalPlayerMagicSystemComponent();

	UMagicSystemComponent* GetPlayerMagicSystemComponent(AActor* Actor);
	
	bool EquipMagicAbilityByMagicId(int32 MagicAbilityId);

	bool EquipDarkMagicAbilityByDarkMagicId(int32 DarkMagicAbilityId);

	bool TryUseCurrentEquipedMagicAbility(UAbilitySystemComponent* ASC);

	bool TryUseCurrentEquipedDarkMagicAbility(UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable)
	USGSAbility* GetCurrentEquipedMagicClass(UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable)
	USGSAbility* GetCurrentEquipedDarkMagicClass(UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable)
	FGameplayTagContainer GetCurrentEquipedMagicCooldownTags(UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable)
	FGameplayTagContainer GetCurrentEquipedDarkMagicCooldownTags(UAbilitySystemComponent* ASC);
	
	UFUNCTION(BlueprintCallable)
	FGameplayTagContainer GetCurrentEquipedMagicAbilityTags(UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable)
	FGameplayTagContainer GetCurrentEquipedDarkMagicAbilityTags(UAbilitySystemComponent* ASC);
	
	UFUNCTION(BlueprintCallable)
	FGameplayTag GetCurrentEquipedMagicAbilityTriggerTag(UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable)
	FGameplayTag GetCurrentEquipedDarkMagicAbilityTriggerTag(UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable)
	FGameplayTag GetAbilityTriggerTagFromSpecHandle(UAbilitySystemComponent* ASC, FGameplayAbilitySpecHandle SpecHandle);
	
	UFUNCTION(BlueprintCallable)
	USGSAbility* GetPlayerLearnedMagicClassById(int32 MagicAbilityId);

	UFUNCTION(BlueprintCallable)
	FGameplayTagContainer GetPlayerLearnedMagicCooldownTagsById(int32 MagicAbilityId);

	UFUNCTION(BlueprintCallable)
	int32 GetEquipedMagicAbilityId();

	UFUNCTION(BlueprintCallable)
	void ClientAddCue(AActor* TargetActor, FGameplayTag CueTag, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters);

	UFUNCTION(BlueprintCallable)
	void ClientRemoveCue(AActor* TargetActor, FGameplayTag CueTag, const FGameplayCueParameters& Parameters);
	
private:

	void LoadDataTable(FName TableName, const FString& Path);

	void LoadDataTableRowsMap(FName TableName, UDataTable* DataTable);

	void OnLoadAbilityClassFinished(FSoftObjectPath Path, UAbilitySystemComponent* ASC, FSkillRowStruct* SRow, const ESkillGameType SkillGameType, bool bIsRootSkill);

	void GiveSkillToHero(UAbilitySystemComponent* ASC, UClass* SkillClass, FSkillRowStruct* SRow, const ESkillGameType& SkillGameType, bool bIsRootSkill);

	void LoadAndGiveMagicAction(FSoftObjectPath Path, UAbilitySystemComponent* ASC);

	void LoadInitBuff(FSoftObjectPath Path, UAbilitySystemComponent* ASC);
	
	void InitSkillAttributeSet(UAbilitySystemComponent* ASC);

private:

	UPROPERTY()
	TMap<FName, TObjectPtr<UDataTable>> SkillRelatedTables;

	UPROPERTY()
	TSubclassOf<UGameplayAbility> TestAbility;

	/*TMap<FName, TMap<int32, FSkillRowStruct>> SkillTableRowsMap;

	TMap<FName, TMap<int32, FBattleSkillRowStruct>> BattleSkillTableRowsMap;

	TMap<FName, TMap<int32, FMagicRowStruct>> MagicTableRowsMap;
	*/

	TMap<FName, TMap<int32, uint8*>> SkillRelatedTablesRowsMap;

	//server only
	UPROPERTY()
	TMap<FSkillRowStruct, TSubclassOf<UGameplayAbility>> LoadedAbilitiesMap;

	//TMap<int32, FGameplayAbilitySpecHandle> ActivatableMagicAbilities;
	
	//local Control (client or listen server)
	//包括狼人的额外战技
	TMap<ESkillUIStateType, FSkillRowStruct*> InitedBattleSkillInfo;

	FGameplayAbilitySpecHandle CurrentEquipedMagicAbilitySpecHandle;

	FGameplayAbilitySpecHandle CurrentEquipedDarkMagicAbilitySpecHandle;

	int32 EquipedMagicAbilityId = 0;

	int32 EquipedDarkMagicAbilityId = 0;

public:

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FSkillSystemLoadDelegate OnSkillSystemLoad;
	
	UPROPERTY(BlueprintAssignable)
	FSkillInitedDelegate OnSkillInited;

	UPROPERTY(BlueprintAssignable)
	FMagicLearnedDelegate OnMagicLearned;

	UPROPERTY(BlueprintAssignable)
	FMagicEquipedDelegate OnMagicEquiped;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FSkillActivateMsgDelegate OnReturnSkillActivateMsg;
	
	FGameplayTagContainer CurrentActivatedSkillTags;

	bool bUseOutRoomSkill = false;
};

//template FMagicRowStruct* USkillManager::GetTableRowsMapRowById<FMagicRowStruct>(FName TableName, int32 Id);
