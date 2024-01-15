// Copyright 2024 Gradess Games. All Rights Reserved.


#include "BlueprintScreenshotToolCommands.h"
#include "BlueprintScreenshotToolSettings.h"
#include "BlueprintScreenshotToolStyle.h"

#define LOCTEXT_NAMESPACE "BlueprintScreenshotTool"

FBlueprintScreenshotToolCommands::FBlueprintScreenshotToolCommands()
	: TCommands<FBlueprintScreenshotToolCommands>(
		TEXT("BlueprintScreenshotTool"),
		NSLOCTEXT("Contexts", "BlueprintScreenshotTool", "BlueprintScreenshotTool Commands"),
		NAME_None,
		FBlueprintScreenshotToolStyle::Get().GetStyleSetName()
	)
{
}

void FBlueprintScreenshotToolCommands::RegisterCommands()
{
	UI_COMMAND(TakeScreenshot, "Take Screenshot", "Take active blueprint screenshot", EUserInterfaceActionType::Button, GetDefault<UBlueprintScreenshotToolSettings>()->TakeScreenshotHotkey);
	UI_COMMAND(OpenDirectory, "Open Directory", "Open directory with screenshots", EUserInterfaceActionType::None, GetDefault<UBlueprintScreenshotToolSettings>()->OpenDirectoryHotkey);
}

#undef LOCTEXT_NAMESPACE
