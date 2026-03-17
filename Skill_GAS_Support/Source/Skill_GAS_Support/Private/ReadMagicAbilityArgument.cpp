// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ReadMagicAbilityArgument.h"


bool UReadMagicAbilityArgument::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	MagicRow.NetSerialize(Ar, Map, bOutSuccess);
	Ar << CurrentEquipedMagicSpecHandle;
	bOutSuccess = true;
	return true;
}
