#pragma once
#include "Abilities/GameplayAbilityTargetActor_GroundTrace.h"
#include "GATA_GroundTrace.generated.h"

UCLASS()
class WESTERN_MAP_API AGATA_GroundTrace : public AGameplayAbilityTargetActor_GroundTrace
{
	GENERATED_BODY()
	
public:

	AGATA_GroundTrace();

	UFUNCTION(BlueprintCallable)
	void Configure(
		UPARAM(DisplayName = "Collision Radius") float InCollisionRadius,
		UPARAM(DisplayName = "Collision Height") float InCollisionHeight,
		UPARAM(DisplayName = "Max Range") float InMaxRange,
		UPARAM(DisplayName = "Trace Profile") FCollisionProfileName InTraceProfile,
		UPARAM(DisplayName = "bTraceAffectsAimPitch") bool InbTraceAffectsAimPitch,
		UPARAM(DisplayName = "StartLocation") FGameplayAbilityTargetingLocationInfo InStartLocation,
		UPARAM(DisplayName = "ReticleParams") FWorldReticleParameters InReticleParams,
		UPARAM(DisplayName = "ReticleClass") TSubclassOf<AGameplayAbilityWorldReticle> InReticleClass,
		UPARAM(DisplayName = "Filter") FGameplayTargetDataFilterHandle InFilter,
		UPARAM(DisplayName = "ShouldProduceTargetDataOnServer") bool InShouldProduceTargetDataOnServer,
		UPARAM(DisplayName = "Debug") bool InbDebug
	);

	virtual void BeginPlay() override;
	
	virtual void StartTargeting(UGameplayAbility* Ability) override;

	virtual void Tick(float DeltaSeconds) override;
	
	virtual void Destroyed() override;

	virtual void CancelTargeting() override;

	virtual void StopTargeting();
};
