#pragma once
#include "GATA_RadiusBase.h"
#include "Abilities/GameplayAbilityTargetActor_Radius.h"
#include "GATA_Radius.generated.h"

UCLASS()
class SKILL_GAS_SUPPORT_API AGATA_Radius : public AGATA_RadiusBase
{
	GENERATED_BODY()
	
public:

	AGATA_Radius();

	UFUNCTION(BlueprintCallable)
	void Configure(
		UPARAM(DisplayName = "Radius") float InRadius,
		UPARAM(DisplayName = "StartLocation") FGameplayAbilityTargetingLocationInfo InStartLocation,
		UPARAM(DisplayName = "ReticleParams") FWorldReticleParameters InReticleParams,
		UPARAM(DisplayName = "ReticleClass") TSubclassOf<AGameplayAbilityWorldReticle> InReticleClass,
		UPARAM(DisplayName = "Filter") FGameplayTargetDataFilterHandle InFilter,
		UPARAM(DisplayName = "ObjectQueryCollisionChannel") ECollisionChannel InObjectQueryCollisionChannel,
		UPARAM(DisplayName = "ShouldProduceTargetDataOnServer") bool InShouldProduceTargetDataOnServer,
		UPARAM(DisplayName = "Debug") bool InbDebug
	);

	virtual void BeginPlay() override;
	
	virtual void StartTargeting(UGameplayAbility* Ability) override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void Destroyed() override;
	
	virtual void CancelTargeting() override;
};
