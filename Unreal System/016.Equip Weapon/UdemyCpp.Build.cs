// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UdemyCpp : ModuleRules
{
	public UdemyCpp(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // "EnhancedInput" : InputAction를 사용하기 위한 모듈
        // "HairStrandsCore" : Groom를 사용하기 위한 모듈
        // "Niagara" : "HairStrandsCore"를 사용하면 빌드 오류 해결을 위해서 사용
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "HairStrandsCore", "Niagara" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
