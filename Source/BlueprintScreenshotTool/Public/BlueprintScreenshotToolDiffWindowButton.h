// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class BLUEPRINTSCREENSHOTTOOL_API FBlueprintScreenshotToolDiffWindowButton : public FToolBarButtonBlock
{
public:
	explicit FBlueprintScreenshotToolDiffWindowButton(const FButtonArgs& ButtonArgs)
		: FToolBarButtonBlock(ButtonArgs)
	{
	}

	FBlueprintScreenshotToolDiffWindowButton();
};
