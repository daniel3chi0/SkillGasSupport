#include "SGS_GATargetDataType.h"

bool FGameplayAbilityTargetData_ActorsOrHitResult::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	SourceLocation.NetSerialize(Ar, Map, bOutSuccess);
	SafeNetSerializeTArray_Default<31>(Ar, TargetActorArray);
	HitResult.NetSerialize(Ar, Map, bOutSuccess);
	MagicRow.NetSerialize(Ar, Map, bOutSuccess);
	Ar << CurrentEquipedMagicSpecHandle;
	
	bOutSuccess = true;
	return true;
}
