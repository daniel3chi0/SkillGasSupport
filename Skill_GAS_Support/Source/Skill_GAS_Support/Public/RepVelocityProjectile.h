#pragma once
#include "RepVelocityProjectile.generated.h"

class UProjectileMovementComponent;

UCLASS()
class WESTERN_MAP_API ARepVelocityProjectile : public AActor
{
	GENERATED_BODY()

public:

	virtual void PostNetReceiveVelocity(const FVector& NewVelocity) override;
	
	UFUNCTION(BlueprintCallable)
	virtual UProjectileMovementComponent* GetMovementComponent() const;
};
