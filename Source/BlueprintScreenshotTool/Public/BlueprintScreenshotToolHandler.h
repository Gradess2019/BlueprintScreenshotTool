// Copyright 2023 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BlueprintScreenshotToolHandler.generated.h"

/**
 * 
 */
UCLASS()
class BLUEPRINTSCREENSHOTTOOL_API UBlueprintScreenshotToolHandler : public UObject
{
	GENERATED_BODY()

public:
	static void TakeScreenshot();

	static TSharedPtr<SWidget> FindParent(TSharedPtr<SWidget> InWidget, const FName& InParentWidgetType);
	static TSharedPtr<SWidget> FindChild(TSharedPtr<SWidget> InWidget, const FName& InChildWidgetType);
	static TSet<TSharedPtr<SWidget>> FindChildren(TSharedPtr<SWidget> InWidget, const FName& InChildWidgetType);
	static TSet<TSharedPtr<SWidget>> FindGraphEditors();
};
