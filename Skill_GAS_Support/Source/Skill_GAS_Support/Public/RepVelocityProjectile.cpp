#include "RepVelocityProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PhysicsEngine/PhysicsSettings.h"

void ARepVelocityProjectile::PostNetReceiveVelocity(const FVector& NewVelocity)
{
	const FPhysicsPredictionSettings& PhysicsPredictionSettings = UPhysicsSettings::Get()->PhysicsPrediction;
	if (PhysicsPredictionSettings.bEnablePhysicsPrediction)
	{
		Super::PostNetReceiveVelocity(NewVelocity);
	}

	UMovementComponent* const MoveComponent = GetMovementComponent();
	if ( MoveComponent )
	{
		MoveComponent->Velocity = NewVelocity;
	}
}

UProjectileMovementComponent* ARepVelocityProjectile::GetMovementComponent() const
{
	return FindComponentByClass<UProjectileMovementComponent>();
}
