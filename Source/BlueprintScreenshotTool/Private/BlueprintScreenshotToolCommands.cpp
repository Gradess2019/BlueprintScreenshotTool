// Copyright 2024 Gradess Games. All Rights Reserved.


#include "BlueprintScreenshotToolCommands.h"
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
	UI_COMMAND(TakeScreenshot, "Take Screenshot", "Take active blueprint screenshot", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
