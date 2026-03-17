// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HarmCircle.generated.h"

UCLASS()
class WESTERN_MAP_API AHarmCircle : public AActor
{
	GENERATED_BODY()
	
public:	
	AHarmCircle();

	//法阵检测范围
	UFUNCTION(BlueprintCallable, Category = "HarmCircle | Hit")
	TArray<FHitResult> CircleDetect(const TArray<TEnumAsByte<EObjectTypeQuery> >& ObjectTypes, TArray<AActor*> ActorsToIgnore);

	//执行法阵GE 和 GC如果有GameplayCueTag的话
	//Duration 是否是可以设置是长的GE
	//Tag 是GE时长的标签   Magnitude  时长
	UFUNCTION(BlueprintCallable, Category = "HarmCircle | Buff")
	void ExecuteCircleGE(AActor* Target, TSubclassOf<UGameplayEffect> EffectClass, FGameplayTag Tag, FGameplayTag GameplayCueTag, bool bDuration = false, float Magnitude = 0.0f);

	//生成GC
	UFUNCTION(BlueprintCallable, meta = (GameplayTagFilter = "GameplayCue"), Category = "HarmCircle | Buff")
	void ExecuteCircleGC(AActor* Target, FGameplayTag GameplayCueTag, FVector GCSpawnLocation, float Magnitude = 0.0f);

protected:

	virtual void BeginPlay() override;

protected:
	//攻击人
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = "true"), Category = "value")
	AActor* Attacker = nullptr;
	
	//判定大小
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = "true"), Category = "value")
	float SphereRadius = 100.f;
	
	//伤害
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = "true"), Category = "value")
	float DataDamage = 0.0f;

	//真实伤害
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = "true"), Category = "value")
	float DataTrueDamage = 0.0f;
	
	//恢复值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "value")
	float RecoverValue = 0.0f;

	//判定生成位置
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = "true"), Category = "value")
	FVector StartAndEndLocation = FVector::Zero();


};
