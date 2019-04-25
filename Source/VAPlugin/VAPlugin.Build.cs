// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class VAPlugin : ModuleRules
{
	public VAPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
                //"VALibrary/include"
            }
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
                "VALibrary/include"
            }
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "Projects",
                "CoreUObject",
                "Engine"
                //,
                //"VALibrary"
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


        Path pPath = Path.Combine(ModuleDirectory, "..", "VALibrary", "lib");
        PublicLibraryPaths.Add(pPath);
        PublicRuntimeLibraryPaths.Add(pPath);

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            // Add the import library
            //PublicLibraryPaths.Add("VALibrary/lib");
            //PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "x64", "Release"));
            PublicAdditionalLibraries.Add("VABase.lib");
            PublicAdditionalLibraries.Add("VistaBase.lib");
            PublicAdditionalLibraries.Add("VistaAspects.lib");
            PublicAdditionalLibraries.Add("VistaInterProcComm.lib");
            PublicAdditionalLibraries.Add("VANet.lib");

            // Delay-load the DLL, so we can load it from the right place first
            PublicDelayLoadDLLs.Add("VABase.dll");
            PublicDelayLoadDLLs.Add("VistaBase.dll");
            PublicDelayLoadDLLs.Add("VistaAspects.dll");
            PublicDelayLoadDLLs.Add("VistaInterProcComm.dll");
            PublicDelayLoadDLLs.Add("VANet.dll");
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {

            PublicAdditionalLibraries.Add("VistaBase.so");
            PublicAdditionalLibraries.Add("VistaAspects.so");
            PublicAdditionalLibraries.Add("VistaInterProcComm.so");
            PublicAdditionalLibraries.Add("VABase.so");
            PublicAdditionalLibraries.Add("VANet.so");
        }
    }
}
