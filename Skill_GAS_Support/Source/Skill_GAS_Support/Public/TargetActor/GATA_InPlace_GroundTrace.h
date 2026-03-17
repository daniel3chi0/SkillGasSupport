#pragma once
#include "GATA_GroundTrace.h"
#include "GATA_InPlace_GroundTrace.generated.h"

UCLASS()
class SKILL_GAS_SUPPORT_API AGATA_InPlace_GroundTrace : public AGATA_GroundTrace
{
	GENERATED_BODY()
	
public:

	AGATA_InPlace_GroundTrace();

	virtual FHitResult PerformTrace(AActor* InSourceActor) override;
	
};
