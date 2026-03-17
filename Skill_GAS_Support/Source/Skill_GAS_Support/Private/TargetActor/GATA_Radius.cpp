#include "TargetActor/GATA_Radius.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "Abilities/GameplayAbility.h"

AGATA_Radius::AGATA_Radius()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PostUpdateWork;
	bDestroyOnConfirmation = false;
}

void AGATA_Radius::Configure(float InRadius, FGameplayAbilityTargetingLocationInfo InStartLocation, FWorldReticleParameters InReticleParams,
	TSubclassOf<AGameplayAbilityWorldReticle> InReticleClass, FGameplayTargetDataFilterHandle InFilter, ECollisionChannel InObjectQueryCollisionChannel,
	bool InShouldProduceTargetDataOnServer, bool InbDebug)
{
	Radius = InRadius;
	StartLocation = InStartLocation;
	ReticleParams = InReticleParams;
	ReticleClass = InReticleClass;
	Filter = InFilter;
	ObjectQueryCollisionChannel = InObjectQueryCollisionChannel;
	ShouldProduceTargetDataOnServer = InShouldProduceTargetDataOnServer;
	bDebug = InbDebug;
}

void AGATA_Radius::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);
}

void AGATA_Radius::StartTargeting(UGameplayAbility* Ability)
{
	SetActorTickEnabled(true);
	Super::StartTargeting(Ability);
}

void AGATA_Radius::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(bDebug)
	{
		PerformOverlap(StartLocation.GetTargetingTransform().GetLocation());
	}
}

void AGATA_Radius::Destroyed()
{
	Super::Destroyed();
}

void AGATA_Radius::CancelTargeting()
{
	const FGameplayAbilityActorInfo* ActorInfo = (OwningAbility ? OwningAbility->GetCurrentActorInfo() : nullptr);
	UAbilitySystemComponent* ASC = (ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr);
	if (ASC)
	{
		ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GenericCancel, OwningAbility->GetCurrentAbilitySpecHandle(), OwningAbility->GetCurrentActivationInfo().GetActivationPredictionKey()).Remove(GenericCancelHandle);
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("AGameplayAbilityTargetActor::CancelTargeting called with null ASC! Actor %s"), *GetName());
	}

	CanceledDelegate.Broadcast(FGameplayAbilityTargetDataHandle());

	SetActorTickEnabled(false);
}
