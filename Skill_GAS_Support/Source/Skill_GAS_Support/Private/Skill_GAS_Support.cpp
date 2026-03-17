// Copyright Epic Games, Inc. All Rights Reserved.

#include "Skill_GAS_Support.h"

#define LOCTEXT_NAMESPACE "FSkill_GAS_SupportModule"

void FSkill_GAS_SupportModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FSkill_GAS_SupportModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSkill_GAS_SupportModule, Skill_GAS_Support)