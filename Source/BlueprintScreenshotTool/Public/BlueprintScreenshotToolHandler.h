// Copyright 2023 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BlueprintScreenshotToolHandler.generated.h"


struct FBSTScreenshotData;

/**
 * 
 */
UCLASS()
class BLUEPRINTSCREENSHOTTOOL_API UBlueprintScreenshotToolHandler : public UObject
{
	GENERATED_BODY()

public:
	static void TakeScreenshot();
	static FBSTScreenshotData CaptureGraphEditor(TSharedPtr<SGraphEditor> InGraphEditor);

	static TSharedPtr<SWidget> FindParent(TSharedPtr<SWidget> InWidget, const FName& InParentWidgetType);
	static TSharedPtr<SWidget> FindChild(TSharedPtr<SWidget> InWidget, const FName& InChildWidgetType);
	static TSet<TSharedPtr<SWidget>> FindChildren(TSharedPtr<SWidget> InWidget, const FName& InChildWidgetType);
	static TSet<TSharedPtr<SWidget>> FindGraphEditors();

protected:
	static TSharedRef<SWindow> CreateTransparentWindow(const FVector2D& InWindowSize);
	static TSharedRef<SWindow> CreateTransparentWindowWithContent(const FVector2D& InWindowSize, TSharedRef<SWidget> InContent);
	static void ShowWindow(TSharedRef<SWindow> InWindow);
};
