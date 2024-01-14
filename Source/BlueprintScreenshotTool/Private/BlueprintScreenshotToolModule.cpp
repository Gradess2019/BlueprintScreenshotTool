// Copyright Epic Games, Inc. All Rights Reserved.


#include "BlueprintScreenshotToolModule.h"
#include "BlueprintScreenshotToolCommandManager.h"
#include "BlueprintScreenshotToolSettings.h"
#include "BlueprintScreenshotToolStyle.h"
#include "ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FBlueprintScreenshotToolModule"

void FBlueprintScreenshotToolModule::StartupModule()
{
	RegisterStyle();
	RegisterCommands();
	RegisterSettings();
}

void FBlueprintScreenshotToolModule::ShutdownModule()
{
	UnregisterStyle();
	UnregisterCommands();
	UnregisterSettings();
}

void FBlueprintScreenshotToolModule::RegisterStyle()
{
	FBlueprintScreenshotToolStyle::Initialize();
	FBlueprintScreenshotToolStyle::ReloadTextures();
}

void FBlueprintScreenshotToolModule::RegisterCommands()
{
	CommandManager = MakeShareable(new FBlueprintScreenshotToolCommandManager());
	CommandManager->RegisterCommands();
}

void FBlueprintScreenshotToolModule::RegisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings(
			"Editor", "Plugins", "Blueprint Screenshot Tool",
			LOCTEXT("BlueprintScreenshotTool_Label", "Blueprint Screenshot Tool"),
			LOCTEXT("BlueprintScreenshotTool_Description", "Configure the Blueprint Screenshot Tool plugin"),
			GetMutableDefault<UBlueprintScreenshotToolSettings>()
		);
	}
}

void FBlueprintScreenshotToolModule::UnregisterStyle()
{
	FBlueprintScreenshotToolStyle::Shutdown();
}

void FBlueprintScreenshotToolModule::UnregisterCommands()
{
	CommandManager->UnregisterCommands();
	CommandManager.Reset();
}

void FBlueprintScreenshotToolModule::UnregisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Editor", "Plugins", "Blueprint Screenshot Tool");
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBlueprintScreenshotToolModule, BlueprintScreenshotTool)
