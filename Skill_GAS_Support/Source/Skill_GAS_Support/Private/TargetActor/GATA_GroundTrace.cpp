#include "TargetActor/GATA_GroundTrace.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "Abilities/GameplayAbility.h"

AGATA_GroundTrace::AGATA_GroundTrace()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PostUpdateWork;
	bDestroyOnConfirmation = false;
}

void AGATA_GroundTrace::Configure(float InCollisionRadius, float InCollisionHeight, float InMaxRange, FCollisionProfileName InTraceProfile,
                                  bool InbTraceAffectsAimPitch, FGameplayAbilityTargetingLocationInfo InStartLocation, FWorldReticleParameters InReticleParams,
                                  TSubclassOf<AGameplayAbilityWorldReticle> InReticleClass, FGameplayTargetDataFilterHandle InFilter,
                                  bool InShouldProduceTargetDataOnServer, bool InbDebug)
{
	CollisionRadius = InCollisionRadius;
	CollisionHeight = InCollisionHeight;
	MaxRange = InMaxRange;
	TraceProfile = InTraceProfile;
	bTraceAffectsAimPitch = InbTraceAffectsAimPitch;
	StartLocation = InStartLocation;
	ReticleParams = InReticleParams;
	ReticleClass = InReticleClass;
	Filter = InFilter;
	ShouldProduceTargetDataOnServer = InShouldProduceTargetDataOnServer;
	bDebug = InbDebug;
}

void AGATA_GroundTrace::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);
	SetActorHiddenInGame(true);
}

void AGATA_GroundTrace::StartTargeting(UGameplayAbility* Ability)
{
	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);
	Super::StartTargeting(Ability);
}

void AGATA_GroundTrace::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AGATA_GroundTrace::Destroyed()
{
	Super::Destroyed();
}

void AGATA_GroundTrace::CancelTargeting()
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
	SetActorHiddenInGame(true);
}

void AGATA_GroundTrace::StopTargeting()
{
	// TargetActor doesn't have a StopTargeting function
	SetActorTickEnabled(false);
	SetActorHiddenInGame(true);

	// Clear added callbacks
	TargetDataReadyDelegate.RemoveAll(this);
	CanceledDelegate.RemoveAll(this);

	if (GenericDelegateBoundASC)
	{
		GenericDelegateBoundASC->GenericLocalConfirmCallbacks.RemoveDynamic(this, &AGameplayAbilityTargetActor::ConfirmTargeting);
		GenericDelegateBoundASC->GenericLocalCancelCallbacks.RemoveDynamic(this, &AGameplayAbilityTargetActor::CancelTargeting);
		GenericDelegateBoundASC = nullptr;
	}
}
