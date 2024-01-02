// Copyright 2023 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class BLUEPRINTSCREENSHOTTOOL_API FBlueprintScreenshotToolCommands : public TCommands<FBlueprintScreenshotToolCommands>
{
public:
	TSharedPtr<FUICommandInfo> TakeScreenshot;
	
public:
	FBlueprintScreenshotToolCommands();
	
	virtual void RegisterCommands() override;
};