#pragma once
#include "GameplayAbilitySpecHandle.h"
#include "SkillStructType.h"
#include "Abilities/GameplayAbilityTargetActor_GroundTrace.h"
#include "MagicSystemComponent.generated.h"

class AGATA_InPlace_GroundTrace;
class AGATA_Radius;
class AGameplayAbilityTargetActor_Radius;
class AGATA_SphereTrace;

UENUM(BlueprintType)
enum class EStopReadMagicState : uint8
{
	CompleteChant,
	InterruptChant,
	ReleaseChantSkill,
};

UENUM(BlueprintType)
enum class EReadMagicChantState : uint8
{
	ChantingStart,
	Chanting,
	ChantingEnd,
	NotChanting,
	CompleteChant,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLearnedMagicDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReadMagicStateChanged, EReadMagicChantState, ReadMagicChantState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStopMagicChantStateDelegate, EStopReadMagicState, StopReadMagicState);

UCLASS(ClassGroup=AbilitySystem, meta=(BlueprintSpawnableComponent))
class SKILL_GAS_SUPPORT_API UMagicSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMagicSystemComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	bool AddToLearnedMagicInfoArray(FLearnedMagicDataWrap& LearnedMagicData);

	UFUNCTION(BlueprintCallable)
	TArray<FLearnedMagicDataWrap>& GetLearnedMagicInfoArray();

	UFUNCTION(BlueprintCallable)
	FGameplayAbilitySpecHandle GetLearnedMagicGASpecHandleByMagicId(int32 MagicId);

	UFUNCTION(BlueprintCallable)
	void FillMagicChantTime(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void CancelMagicChant(bool bForced = false);

	UFUNCTION(BlueprintCallable, Category = "Magic|Targeting")
	AGATA_SphereTrace* GetSphereTraceTargetActor();

	UFUNCTION(BlueprintCallable, Category = "Magic|Targeting")
	AGameplayAbilityTargetActor_GroundTrace* GetGroundTraceTargetActorByClass(TSubclassOf<AGameplayAbilityTargetActor_GroundTrace> TargetActorClass);

	UFUNCTION(BlueprintCallable, Category = "Magic|Targeting")
	AGATA_InPlace_GroundTrace* GetInPlaceGroundTraceTargetActorByClass(TSubclassOf<AGATA_InPlace_GroundTrace> TargetActorClass);
	
	UFUNCTION(BlueprintCallable, Category = "Magic|Targeting")
	AGATA_Radius* GetRadiusTargetActor();
	
	UFUNCTION(BlueprintCallable, Category = "Magic")
	void GetAllLearnedMagicAbilitiesIds(TArray<int32>& ResultArray);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Magic")
	void ServerSetEquipedMagicAbilityId(int32 InEquipedMagicAbilityId);

	UFUNCTION(BlueprintCallable)
	FGameplayAbilitySpecHandle GetCurrentEquipedMagicAbilitySpecHandle(EMagicType MagicType);
	
public:
	//Magic System
	TMap<int32, FGameplayAbilitySpecHandle> LearnedMagicLinkMap;

	UPROPERTY(ReplicatedUsing = OnRep_UpdateLearnedMagicInfoArray)
	TArray<FLearnedMagicDataWrap> LearnedMagicInfoArray;

	UPROPERTY(BlueprintAssignable)
	FLearnedMagicDelegate OnLearnedMagic;

	UPROPERTY(BlueprintReadWrite)
	float CurrentChantTime;

	UPROPERTY(BlueprintReadWrite)
	bool bIsChanting;

	UPROPERTY(BlueprintReadWrite, Replicated)
	int32 EquipedMagicAbilityId;

	UPROPERTY(BlueprintReadWrite, Replicated)
	int32 EquipedDarkMagicAbilityId;
	
	UPROPERTY(BlueprintReadWrite)
	EReadMagicChantState ReadMagicChantState = EReadMagicChantState::NotChanting;
	
	UPROPERTY(BlueprintAssignable)
	FOnReadMagicStateChanged OnReadMagicStateChanged;

	UPROPERTY(BlueprintAssignable)
	FOnStopMagicChantStateDelegate OnStopMagicChantState;
	
protected:
	
	UFUNCTION()
	void OnRep_UpdateLearnedMagicInfoArray();

	UFUNCTION()
	void OnRep_GroundTraceTargetActor();

protected:

	UPROPERTY()
	AGATA_SphereTrace* SphereTraceTargetActor;

	UPROPERTY(ReplicatedUsing=OnRep_GroundTraceTargetActor)
	AGameplayAbilityTargetActor_GroundTrace* GroundTraceTargetActor;

	UPROPERTY()
	AGATA_InPlace_GroundTrace* InPlaceGroundTraceTargetActor;

	UPROPERTY()
	AGATA_Radius* RadiusTargetActor;
};
