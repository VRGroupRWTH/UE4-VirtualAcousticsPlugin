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
                "Engine",
                "DisplayCluster", 
                "DisplayClusterExtensions",
                "InputCore"

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


        // Path pPath = Path.Combine(ModuleDirectory, "..", "VALibrary", "lib");
        PublicLibraryPaths.Add(Path.Combine(ModuleDirectory, "..", "VALibrary", "lib"));
        PublicRuntimeLibraryPaths.Add(Path.Combine(ModuleDirectory, "..", "VALibrary", "lib"));


        if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
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
        else // if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            // PublicRuntimeLibraryPaths.Add(Path.Combine(ModuleDirectory, "..", "VALibrary", "lib"));

            // Tried PublicAdditionalLibraries("VABase.so");

            // PublicAdditionalLibraries.Add("VistaBase.so");
            // PublicAdditionalLibraries.Add("VistaAspects.so");
            // PublicAdditionalLibraries.Add("VistaInterProcComm.so");
            // PublicAdditionalLibraries.Add("VABase.so");
            // PublicAdditionalLibraries.Add("VANet.so");

            // PublicAdditionalLibraries.Add("VABase.so");
            // PublicAdditionalLibraries.Add("VistaBase.so");
            // PublicAdditionalLibraries.Add("VistaAspects.so");
            // PublicAdditionalLibraries.Add("VistaInterProcComm.so");
            // PublicAdditionalLibraries.Add("VANet.so");


            // Path.GetFullPath(Path.Combine(ModuleDirectory, actualPath))

            // run fr 3.5. 18.00
            // PublicDelayLoadDLLs.Add("VABase.so");
            // PublicDelayLoadDLLs.Add("VistaBase.so");
            // PublicDelayLoadDLLs.Add("VistaAspects.so");
            // PublicDelayLoadDLLs.Add("VistaInterProcComm.so");
            // PublicDelayLoadDLLs.Add("VANet.so");

            // PublicAdditionalLibraries.Add("VABase.so");
            // PublicAdditionalLibraries.Add("VistaBase.so");
            // PublicAdditionalLibraries.Add("VistaAspects.so");
            // PublicAdditionalLibraries.Add("VistaInterProcComm.so");
            // PublicAdditionalLibraries.Add("VANet.so");


            // PublicAdditionalLibraries.Add("VistaBase.so");
            // PublicAdditionalLibraries.Add("VistaAspects.so");
            // PublicAdditionalLibraries.Add("VistaInterProcComm.so");
            // PublicAdditionalLibraries.Add("VABase.so");
            // PublicAdditionalLibraries.Add("VANet.so");

            // from USDImporter.Build.cs (G:\Programme\UE_4.21\Engine\Plugins\Editor\USDImporter\Source\USDImporter)
            /*
            else if (Target.Platform == UnrealTargetPlatform.Linux && Target.Architecture.StartsWith("x86_64"))
            {
                // link directly to runtime libs on Linux, as this also puts them into rpath
                string RuntimeLibraryPath = Path.Combine(ModuleDirectory, "../../Binaries", Target.Platform.ToString(), Target.Architecture.ToString());
                PrivateRuntimeLibraryPaths.Add(RuntimeLibraryPath);
                PublicAdditionalLibraries.Add(RuntimeLibraryPath + "/libUnrealUSDWrapper.so");

                foreach (string FilePath in Directory.EnumerateFiles(RuntimeLibraryPath, "*.so*", SearchOption.AllDirectories))
                {
                    RuntimeDependencies.Add(FilePath);
                }
            }
            */

            // PrivateRuntimeLibraryPaths.Add(RuntimeLibraryPath);
            string RuntimeLibraryPath = Path.Combine(ModuleDirectory, "..", "VALibrary", "lib");
            PrivateRuntimeLibraryPaths.Add(RuntimeLibraryPath);
            PublicLibraryPaths.Add(RuntimeLibraryPath);

            foreach (string FilePath in Directory.EnumerateFiles(RuntimeLibraryPath, "*.so", SearchOption.AllDirectories))
            {
                PublicAdditionalLibraries.Add(FilePath);
                RuntimeDependencies.Add(FilePath);
            }
			foreach (string FilePath in Directory.EnumerateFiles(RuntimeLibraryPath, "*.so.*", SearchOption.AllDirectories))
            {
                RuntimeDependencies.Add(FilePath);
            }

            // RuntimeDependencies.Add("VistaBase.so");
            // RuntimeDependencies.Add("VistaAspects.so");
            // RuntimeDependencies.Add("VistaInterProcComm.so");
            // RuntimeDependencies.Add("VABase.so");
            // RuntimeDependencies.Add("VANet.so");

        }
    }
}
