// Copyright 2024 Gradess Games. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

class BLUEPRINTSCREENSHOTTOOL_API FBlueprintScreenshotToolStyle
{
public:
	static const FString IconFileName;
	static const FName IconName;
	static const FName IconNameSmall;

public:
	class FStyle : public FSlateStyleSet
	{
	public:
		FStyle();

		void Initialize();
	};

	static void Initialize();
	static void Shutdown();
	static void ReloadTextures();

	static const ISlateStyle& Get();

	static FName GetStyleSetName();

private:
	static TSharedPtr<FStyle> StyleInstance;
};
