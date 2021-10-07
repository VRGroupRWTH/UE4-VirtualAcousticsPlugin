// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class VAPlugin : ModuleRules
{
	public VAPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		bEnableExceptions = true;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
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
                "Engine",
                "DisplayCluster", 
                "RWTHVRToolkit",
                "InputCore",
                "Sockets",
                "Networking",
                "DeveloperSettings"
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

        string LibPath = Path.Combine(ModuleDirectory, "..", "VALibrary", "lib");
        PublicRuntimeLibraryPaths.Add(LibPath);

        if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
        {
            // Add the import library
            //PublicLibraryPaths.Add("VALibrary/lib");
            //PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "x64", "Release"));
            PublicAdditionalLibraries.Add(Path.Combine(LibPath,"VABase.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(LibPath,"VistaBase.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(LibPath,"VistaAspects.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(LibPath,"VistaInterProcComm.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(LibPath,"VANet.lib"));

            // Delay-load the DLL, so we can load it from the right place first
            PublicDelayLoadDLLs.Add("VABase.dll");
            PublicDelayLoadDLLs.Add("VistaBase.dll");
            PublicDelayLoadDLLs.Add("VistaAspects.dll");
            PublicDelayLoadDLLs.Add("VistaInterProcComm.dll");
            PublicDelayLoadDLLs.Add("VANet.dll");

            //also add runtime dependecy so they are packaged correctly?
            RuntimeDependencies.Add(Path.Combine(LibPath,"VABase.dll"));
            RuntimeDependencies.Add(Path.Combine(LibPath,"VistaBase.dll"));
            RuntimeDependencies.Add(Path.Combine(LibPath,"VistaAspects.dll"));
            RuntimeDependencies.Add(Path.Combine(LibPath,"VistaInterProcComm.dll"));
            RuntimeDependencies.Add(Path.Combine(LibPath,"VANet.dll"));


        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            PrivateRuntimeLibraryPaths.Add(LibPath);
			
			PublicAdditionalLibraries.Add(Path.Combine(LibPath,"libVABase.so"));
            PublicAdditionalLibraries.Add(Path.Combine(LibPath,"libVistaBase.so"));
            PublicAdditionalLibraries.Add(Path.Combine(LibPath,"libVistaAspects.so"));
            PublicAdditionalLibraries.Add(Path.Combine(LibPath,"libVistaInterProcComm.so"));
            PublicAdditionalLibraries.Add(Path.Combine(LibPath,"libVANet.so"));
			
			RuntimeDependencies.Add(Path.Combine(LibPath,"libVABase.so"));
            RuntimeDependencies.Add(Path.Combine(LibPath,"libVistaBase.so"));
            RuntimeDependencies.Add(Path.Combine(LibPath,"libVistaAspects.so"));
            RuntimeDependencies.Add(Path.Combine(LibPath,"libVistaInterProcComm.so"));
            RuntimeDependencies.Add(Path.Combine(LibPath,"libVANet.so"));

            /*foreach (string FilePath in Directory.EnumerateFiles(LibPath, "*.so", SearchOption.AllDirectories))
            {
                PublicAdditionalLibraries.Add(FilePath);
                RuntimeDependencies.Add(FilePath);
            }*/
        }

        //this is needed to register on Editor delegates, i.e., BeginPIE and EndPIE, but only in Editor builds
        if (Target.Type == TargetRules.TargetType.Editor)
        {
            PrivateDependencyModuleNames.AddRange(
            new string[]
            {
            "UnrealEd"
            }
            );
        }
    }
}
