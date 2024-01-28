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
	static FString SaveScreenshot(const FBSTScreenshotData& InData);
	static FBSTScreenshotData CaptureGraphEditor(TSharedPtr<SGraphEditor> InGraphEditor);

	static void OpenDirectory();

protected:
	static void RestoreNodeSelection(TSharedPtr<SGraphEditor> InGraphEditor, const TSet<UObject*>& InSelectedNodes);
	static bool HasAnySelectedNodes(const TSet<TSharedPtr<SGraphEditor>>& InGraphEditors);
	static void ShowNotification(const TArray<FString>& InPaths);
	static void ShowDirectoryErrorNotification(const FString& InPath);
	static UTextureRenderTarget2D* DrawGraphEditor(TSharedPtr<SGraphEditor> InGraphEditor, const FVector2D& InWindowSize);
	static FString GetExtension(EBSTImageFormat InFormat);
	static FString GenerateScreenshotName(TSharedPtr<SGraphEditor> InGraphEditor);
};


