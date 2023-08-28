// Copyright 2023 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FSlateStyleSet;

class BLUEPRINTSCREENSHOTTOOL_API FBlueprintScreenshotToolStyle
{
public:
	static void Initialize();
	static void Shutdown();
	static void ReloadTextures();
	
	static const ISlateStyle& Get();

	static FName GetStyleSetName();

private:
	static TSharedPtr<FSlateStyleSet> StyleInstance;
};
