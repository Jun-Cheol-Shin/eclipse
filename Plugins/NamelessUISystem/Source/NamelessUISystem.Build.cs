// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NamelessUISystem : ModuleRules
{
	public NamelessUISystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject", 
				"Engine", 
				"InputCore",
                "EnhancedInput",
                "CommonUI",
                "CommonInput",
                "Slate",
                "SlateCore",
                "UMG",
                "ApplicationCore",
                "DeveloperSettings",
                "UnrealEd",
                "GameplayTags",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// ... add private dependencies that you statically link with here ...	

			
            }
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
