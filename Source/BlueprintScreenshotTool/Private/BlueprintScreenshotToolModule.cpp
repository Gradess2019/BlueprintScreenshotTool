// Copyright Epic Games, Inc. All Rights Reserved.

#include "BlueprintScreenshotToolModule.h"

#include "BlueprintScreenshotToolCommandManager.h"
#include "BlueprintScreenshotToolStyle.h"

#define LOCTEXT_NAMESPACE "FBlueprintScreenshotToolModule"

void FBlueprintScreenshotToolModule::StartupModule()
{
	RegisterStyle();
	RegisterCommands();
}

void FBlueprintScreenshotToolModule::ShutdownModule()
{
	UnregisterStyle();
	UnregisterCommands();
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

void FBlueprintScreenshotToolModule::UnregisterStyle()
{
	FBlueprintScreenshotToolStyle::Shutdown();
}

void FBlueprintScreenshotToolModule::UnregisterCommands()
{
	CommandManager->UnregisterCommands();
	CommandManager.Reset();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBlueprintScreenshotToolModule, BlueprintScreenshotTool)
