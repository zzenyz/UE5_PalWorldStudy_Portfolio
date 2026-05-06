// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PalWorld_Study : ModuleRules
{
	public PalWorld_Study(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "AIModule", "NavigationSystem", "UMG", "Slate", "SlateCore" });
	}
}
