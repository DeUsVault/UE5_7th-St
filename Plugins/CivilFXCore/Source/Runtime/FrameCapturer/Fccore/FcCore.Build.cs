using UnrealBuildTool;
using System.IO;
using System;

public class FcCore : ModuleRules
{
    public FcCore(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;

        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));

        string LibPath = Path.Combine(PluginDirectory, "Binaries/ThirdParty/FcCore/Win64");

        Console.WriteLine(LibPath);

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicAdditionalLibraries.Add(Path.Combine(LibPath, "fccore.lib"));

            PublicDelayLoadDLLs.Add("fccore.dll");
            RuntimeDependencies.Add(Path.Combine(LibPath, "fccore.dll"));
        }
    }
}