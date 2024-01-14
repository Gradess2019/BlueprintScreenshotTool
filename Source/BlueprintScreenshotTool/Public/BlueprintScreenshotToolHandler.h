// Copyright 2024 Gradess Games. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BlueprintScreenshotToolHandler.generated.h"


struct FBSTScreenshotData;
enum class EBSTImageFormat : uint8;

UCLASS()
class BLUEPRINTSCREENSHOTTOOL_API UBlueprintScreenshotToolHandler : public UObject
{
	GENERATED_BODY()

public:
	static TArray<FString> TakeScreenshotWithPaths();
	static TArray<FString> TakeScreenshotWithNotification();
	static void TakeScreenshot();

	static FString SaveScreenshot(const TArray<FColor>& InColorData, const FIntVector& InSize);
	static FBSTScreenshotData CaptureGraphEditor(TSharedPtr<SGraphEditor> InGraphEditor);

protected:
	static TSharedRef<SWindow> CreateTransparentWindow(const FVector2D& InWindowSize);
	static TSharedRef<SWindow> CreateTransparentWindowWithContent(const FVector2D& InWindowSize, TSharedRef<SWidget> InContent);
	static void ShowWindow(TSharedRef<SWindow> InWindow);
	static void FixGraphNodesAppearance(TSharedPtr<SGraphEditor> InGraphEditor);
	static bool HasAnySelectedNodes(const TSet<TSharedPtr<SGraphEditor>>& InGraphEditors);
	static void ShowNotification(const TArray<FString>& InPaths);
	static FString GetExtension(EBSTImageFormat InFormat);
};
