// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/HarmCircle.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

AHarmCircle::AHarmCircle()
{
 	
	PrimaryActorTick.bCanEverTick = false;

}

TArray<FHitResult> AHarmCircle::CircleDetect(const TArray<TEnumAsByte<EObjectTypeQuery> >& ObjectTypes, TArray<AActor*> ActorsToIgnore)
{
	TArray<FHitResult> OutHits;

	UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(),
		StartAndEndLocation,
		StartAndEndLocation,
		SphereRadius,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		OutHits,
		false,
		FLinearColor::Red,
		FLinearColor::Green,
		2.0f);

	return OutHits;
}

void AHarmCircle::ExecuteCircleGE(AActor* Target, TSubclassOf<UGameplayEffect> EffectClass, FGameplayTag Tag, FGameplayTag GameplayCueTag, bool bDuration, float Magnitude)
{

	
	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Target);
	if (!ASCInterface) return;

	UAbilitySystemComponent* TargetASC = ASCInterface->GetAbilitySystemComponent();
	if (!TargetASC) return;

	if (!EffectClass)
	{
		return;
	}

	FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext(); // 创建上下文
	FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(EffectClass, 1.0f, ContextHandle);

	if (bDuration && Tag.IsValid())
	{
		SpecHandle.Data->SetDuration(Magnitude, false);
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Tag, Magnitude);
	}
	

	//应用GE
	TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);

	if (GameplayCueTag.IsValid())
	{
		FGameplayCueParameters Params;
		//	Params.RawMagnitude = Magnitude;

		//执行GameplayCue
		TargetASC->ExecuteGameplayCue(GameplayCueTag, Params);
	}

	

}

void AHarmCircle::ExecuteCircleGC(AActor* Target, FGameplayTag GameplayCueTag, FVector GCSpawnLocation, float Magnitude)
{
	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Target);
	if (!ASCInterface) return;

	UAbilitySystemComponent* TargetASC = ASCInterface->GetAbilitySystemComponent();
	if (!TargetASC) return;

	if (GameplayCueTag.IsValid())
	{
		FGameplayCueParameters Params;
		Params.RawMagnitude = Magnitude;
		Params.Location = GCSpawnLocation;

		//执行GameplayCue
		TargetASC->ExecuteGameplayCue(GameplayCueTag, Params);
	}

}


void AHarmCircle::BeginPlay()
{
	Super::BeginPlay();
	
}


