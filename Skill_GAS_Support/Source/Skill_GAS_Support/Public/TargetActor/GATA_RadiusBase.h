#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GATA_RadiusBase.generated.h"

class UGameplayAbility;

/** Selects everything within a given radius of the source actor. */
UCLASS(Blueprintable, notplaceable)
class WESTERN_MAP_API AGATA_RadiusBase : public AGameplayAbilityTargetActor
{
	GENERATED_UCLASS_BODY()

public:

	virtual void StartTargeting(UGameplayAbility* Ability) override;
	
	virtual void ConfirmTargetingAndContinue() override;

	/** Radius of target acquisition around the ability's start location. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = Radius)
	float Radius;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = Collision)
	TEnumAsByte<ECollisionChannel> ObjectQueryCollisionChannel;

protected:

	TArray<TWeakObjectPtr<AActor> >	PerformOverlap(const FVector& Origin);

	FGameplayAbilityTargetDataHandle MakeTargetData(const TArray<TWeakObjectPtr<AActor>>& Actors, const FVector& Origin) const;

};
