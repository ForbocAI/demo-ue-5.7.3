
using UnrealBuildTool;

public class DemoProject : ModuleRules
{
	public DemoProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "ForbocAI_SDK" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });
	}
}
