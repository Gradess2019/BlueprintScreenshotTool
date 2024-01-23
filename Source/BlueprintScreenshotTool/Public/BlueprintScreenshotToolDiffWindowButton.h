// Copyright 2024 Gradess Games. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "Framework/MultiBox/SToolBarButtonBlock.h"

class BLUEPRINTSCREENSHOTTOOL_API FBlueprintScreenshotToolDiffWindowButton : public FToolBarButtonBlock
{
public:
#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 3
	explicit FBlueprintScreenshotToolDiffWindowButton(const FButtonArgs& ButtonArgs)
		: FToolBarButtonBlock(ButtonArgs)
	{
	}
#endif
	
	FBlueprintScreenshotToolDiffWindowButton();
};
