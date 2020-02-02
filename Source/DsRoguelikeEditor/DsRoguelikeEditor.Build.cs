// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class DsRoguelikeEditor : ModuleRules
{
	public DsRoguelikeEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "DsRoguelike", "Core", "CoreUObject", "Engine", "PropertyEditor", "InputCore", "UnrealEd" });

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "AssetTools",
                "Slate",
                "SlateCore",
                "GraphEditor",
                "EditorStyle",
                "Kismet",
                "KismetWidgets",
                "ApplicationCore",
                "ToolMenus",
				// ... add private dependencies that you statically link with here ...
			}
            );

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
