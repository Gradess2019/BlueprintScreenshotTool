// Copyright 2024 Gradess Games. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"

class BLUEPRINTSCREENSHOTTOOL_API FBlueprintScreenshotToolCommandManager
{
private:
	TSharedPtr<FUICommandList> CommandList;
	TSharedPtr<FExtender> ToolbarExtension;

public:
	void RegisterCommands();
	void UnregisterCommands();

	void OnTakeScreenshot();

private:
	void MapCommands();
	void RegisterToolbarExtension();
	void UnregisterToolbarExtension();
	static void AddToolbarExtension(FToolBarBuilder& ToolBarBuilder);
};
