// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class vamps : ModuleRules
{
	public vamps(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"SlateCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"vamps",
			"vamps/Variant_Platforming",
			"vamps/Variant_Platforming/Animation",
			"vamps/Variant_Combat",
			"vamps/Variant_Combat/AI",
			"vamps/Variant_Combat/Animation",
			"vamps/Variant_Combat/Gameplay",
			"vamps/Variant_Combat/Interfaces",
			"vamps/Variant_Combat/UI",
			"vamps/Variant_SideScrolling",
			"vamps/Variant_SideScrolling/AI",
			"vamps/Variant_SideScrolling/Gameplay",
			"vamps/Variant_SideScrolling/Interfaces",
			"vamps/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
