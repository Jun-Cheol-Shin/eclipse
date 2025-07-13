// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EclipseUIFramework : ModuleRules
{
	public EclipseUIFramework(ReadOnlyTargetRules Target) : base(Target)
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
                "Slate",
                "SlateCore",
                "CommonUI", 
				"UMG", 
				"ApplicationCore", 
				"CommonInput", 
				"DeveloperSettings", 
				"UnrealEd"

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
