// Fill out your copyright notice in the Description page of Project Settings.
using System;
using System.IO;
using UnrealBuildTool;

public class Face : ModuleRules
{
	public Face(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");
        // if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
        // {
        //		if (UEBuildConfiguration.bCompileSteamOSS == true)
        //		{
        //			DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
        //		}
        // }

        var arch = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86";
        PrivateIncludePaths.Add(Path.Combine(Environment.GetEnvironmentVariable("KINECTSDK20_DIR"), "inc"));
        PublicLibraryPaths.Add(Path.Combine(Environment.GetEnvironmentVariable("KINECTSDK20_DIR"), "Lib", arch));
        PublicAdditionalLibraries.Add("Kinect20.lib");
        PublicAdditionalLibraries.Add("Kinect20.Face.lib");
    }
}
