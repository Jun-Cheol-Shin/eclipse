// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Eclipse : ModuleRules
{
	public Eclipse(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput",
			"NamelessUISystem",

            });

		PrivateDependencyModuleNames.AddRange(new string[] { 
			"UMG", 
			"ApplicationCore",
            "GameplayTags",
            "GameplayTasks",
            "GameplayAbilities",
			"NetCore",
            "Niagara",
            "CommonUI",
            "CommonInput",
            "DeveloperSettings",
            "GameInputBase",
            "GameInputBaseEditor",
            "GameInputWindows",
            });

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
