#pragma once
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"

#include "SkillSystemFunctionLibrary.generated.h"

struct FGameplayAbilityTargetDataHandle;
class USGSAbility;
class UMagicSystemComponent;
struct FSkillRowStruct;
struct FMagicRowStruct;
class UAbilitySystemComponent;

UCLASS()
class SKILL_GAS_SUPPORT_API USkillSystemFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	float GetAngleForActorForwardVector(FVector RefPoint, FVector ContraPoint, AActor* RefActor);

	UFUNCTION(BlueprintCallable, Category = "SkillLibrary|Magic", meta=(WorldContext = "WorldContextObject"))
	static void GiveMagicAbilityById(UObject* WorldContextObject, UAbilitySystemComponent* ASC, int32 MagicAbilityId);

	UFUNCTION(BlueprintCallable, Category = "SkillLibrary|Magic", meta=(WorldContext = "WorldContextObject"))
	static FMagicRowStruct GetMagicInfoBySkillId(UObject* WorldContextObject, int32 SkillId);

	UFUNCTION(BlueprintCallable, Category = "SkillLibrary|Skill", meta=(WorldContext = "WorldContextObject"))
	static FSkillRowStruct GetSkillInfoBySkillId(UObject* WorldContextObject, int32 SkillId);

	UFUNCTION(BlueprintCallable, Category = "SkillLibrary|Skill")
	static bool IsAbilityActiveByTag(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTag& Tag);

	UFUNCTION(BlueprintPure, Category = "SkillLibrary|Magic", meta=(WorldContext = "WorldContextObject"))
	static UMagicSystemComponent* GetLocalPlayerMagicSystemComponent(UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "SkillLibrary|Magic", meta=(WorldContext = "WorldContextObject"))
	static UMagicSystemComponent* GetPlayerMagicSystemComponent(UObject* WorldContextObject, AActor* CheckActor);

	UFUNCTION(BlueprintCallable, Category = "SkillLibrary|Magic", meta=(WorldContext = "WorldContextObject"))
	static bool EquipMagicAbilityByMagicId(UObject* WorldContextObject, int32 MagicId);

	UFUNCTION(BlueprintCallable, Category = "SkillLibrary|Magic", meta=(WorldContext = "WorldContextObject"))
	static bool TryUseCurrentEquipedMagicAbility(UObject* WorldContextObject, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category = "SkillLibrary|Magic", meta=(WorldContext = "WorldContextObject"))
	static USGSAbility* GetCurrentEquipedMagicClass(UObject* WorldContextObject, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category = "SkillLibrary|Magic", meta=(WorldContext = "WorldContextObject"))
	static FGameplayTagContainer GetCurrentEquipedMagicCooldownTags(UObject* WorldContextObject, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category = "SkillLibrary|Magic", meta=(WorldContext = "WorldContextObject"))
	static USGSAbility* GetPlayerLearnedMagicClassById(UObject* WorldContextObject, int32 MagicAbilityId);

	UFUNCTION(BlueprintCallable, Category = "SkillLibrary|Magic", meta=(WorldContext = "WorldContextObject"))
	static FGameplayTagContainer GetPlayerLearnedMagicCooldownTagsById(UObject* WorldContextObject, int32 MagicAbilityId);

	UFUNCTION(BlueprintCallable, Category = "SkillLibrary|Magic")
	static bool HasGivedAbilityByTag(UAbilitySystemComponent* ASC, FGameplayTag Tag);

	UFUNCTION(BlueprintCallable, Category = "SkillLibrary|Magic", meta=(WorldContext = "WorldContextObject"))
	static FGameplayTagContainer GetCurrentEquipedMagicAbilityTags(UObject* WorldContextObject, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category = "SkillLibrary|Magic", meta=(WorldContext = "WorldContextObject"))
	static FGameplayTag GetCurrentEquipedMagicAbilityTriggerTag(UObject* WorldContextObject, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintPure, Category = "SkillLibrary|Magic", meta=(WorldContext = "WorldContextObject"))
	static FGameplayTag GetAbilityTriggerTagFromSpecHandle(UObject* WorldContextObject, UAbilitySystemComponent* ASC, FGameplayAbilitySpecHandle SpecHandle);

	UFUNCTION(BlueprintCallable, Category = "SkillLibrary|Magic", meta=(WorldContext = "WorldContextObject"))
	static void GetPlayerAllLearnedMagicAbilitiesIds(UObject* WorldContextObject, AActor* CheckActor, TArray<int32>& ResultArray);
	
	UFUNCTION(BlueprintPure, Category = "SkillLibrary|Magic")
	static bool MagicRowIsValid(const FMagicRowStruct& MagicRow);

	UFUNCTION(BlueprintPure, Category = "SkillLibrary|Magic")
	static bool SkillRowIsValid(const FSkillRowStruct& SkillRow);

	UFUNCTION(BlueprintPure, Category = "SkillLibrary|Skill")
	static bool SkillIdIsValid(const int32 SkillId);

	UFUNCTION(BlueprintPure, Category = "Ability|TargetData")
	static FMagicRowStruct GetMagicRowFromTargetData(const FGameplayAbilityTargetDataHandle& TargetData, int32 Index);

	UFUNCTION(BlueprintPure, Category = "Ability|TargetData")
	static FGameplayAbilitySpecHandle GetCurrentEquipedMagicSpecHandleFromTargetData(const FGameplayAbilityTargetDataHandle& TargetData, int32 Index);

	UFUNCTION(BlueprintPure, Category = "Ability|TargetData")
	static void GetCDTimeRemainingAndDurationByTag(UAbilitySystemComponent* ASC, FGameplayTagContainer InCooldownTags, float& TimeRemaining, float& CooldownDuration);

	UFUNCTION(BlueprintCallable,Category = "Ability")
	static bool SendGameplayEventToActorByReturn(AActor* Actor, FGameplayTag EventTag, FGameplayEventData Payload);
	
private:

	static void OnMagicAbilityLoaded(FSoftObjectPath Path, FSkillRowStruct* MagicSkillRow, UAbilitySystemComponent* ASC, UObject* Context);
};
