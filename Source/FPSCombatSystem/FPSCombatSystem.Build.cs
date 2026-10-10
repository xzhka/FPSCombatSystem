// FPS Combat project

using UnrealBuildTool;

public class FPSCombatSystem : ModuleRules
{
	public FPSCombatSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "GameplayAbilities", "GameplayTasks", 
			"GameplayTags", "ModularGameplay", "ModularGameplayActors", "NetCore", "IrisCore", "GameplayMessageRuntime", "IKRig", "Niagara", "PhysicsCore"
		});
	}
}
