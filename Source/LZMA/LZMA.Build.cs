using UnrealBuildTool;
using System.IO;

public class LZMA : ModuleRules
{
	protected virtual string IncDirectory { get { return Path.Combine(ModuleDirectory, "include"); } }

	protected virtual string LibDirectory { get { return Path.Combine(ModuleDirectory, "lib"); } }

	protected virtual string BinDirectory { get { return Path.Combine(ModuleDirectory, "bin"); } }

	public LZMA(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;

        PublicDefinitions.Add("LINK_LZMA_DLL=1");

        PublicIncludePaths.Add(IncDirectory);

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			//PublicAdditionalLibraries.Add(Path.Combine(LibDirectory, "Win64", "libmpg123-0.lib"));
			RuntimeDependencies.Add(Path.Combine(BinDirectory, "Win64", "LZMA.dll"));
			PublicDelayLoadDLLs.Add("LZMA.dll");
		}
		// TODO: Add linux/mac binaries
    }
}