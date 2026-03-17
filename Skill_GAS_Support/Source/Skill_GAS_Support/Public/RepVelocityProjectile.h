#pragma once
#include "RepVelocityProjectile.generated.h"

class UProjectileMovementComponent;

UCLASS()
class SKILL_GAS_SUPPORT_API ARepVelocityProjectile : public AActor
{
	GENERATED_BODY()

public:

	virtual void PostNetReceiveVelocity(const FVector& NewVelocity) override;
	
	UFUNCTION(BlueprintCallable)
	virtual UProjectileMovementComponent* GetMovementComponent() const;
};
