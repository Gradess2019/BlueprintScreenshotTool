// Copyright 2023 Gradess Games. All Rights Reserved.


#include "BlueprintScreenshotToolHandler.h"
#include "BlueprintEditor.h"
#include "BlueprintScreenshotToolSettings.h"
#include "BlueprintScreenshotToolTypes.h"
#include "BlueprintScreenshotToolWindowManager.h"
#include "IImageWrapperModule.h"
#include "ImageUtils.h"
#include "ImageWriteQueue.h"
#include "ImageWriteTask.h"
#include "SBlueprintDiff.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Misc/FileHelper.h"

struct FWidgetSnapshotTextureData;

TArray<FString> UBlueprintScreenshotToolHandler::TakeScreenshotWithPaths()
{
	auto GraphEditors = UBlueprintScreenshotToolWindowManager::FindActiveGraphEditors();
	if (GraphEditors.Num() <= 0)
	{
		return {};
	}

	TArray<FString> Paths;

	const auto bHasSelectedNodes = HasAnySelectedNodes(GraphEditors);
	for (const auto GraphEditor : GraphEditors)
	{
		if (bHasSelectedNodes && GraphEditor->GetSelectedNodes().Num() <= 0)
		{
			continue;
		}

		auto [ColorData, Size] = CaptureGraphEditor(GraphEditor);
		const auto Path = SaveScreenshot(ColorData, Size);
		if (!Path.IsEmpty())
		{
			Paths.Add(Path);
		}
	}

	return Paths; 
}

void UBlueprintScreenshotToolHandler::TakeScreenshot()
{
	TakeScreenshotWithPaths();
}

FString UBlueprintScreenshotToolHandler::SaveScreenshot(const TArray<FColor>& InColorData, const FIntVector& InSize)
{
	const auto ScreenshotDir = FPaths::ScreenShotDir();
	const auto BaseName = GetDefault<UBlueprintScreenshotToolSettings>()->ScreenshotBaseName;
	const auto FileExtension = FString(TEXT("png"));
	const auto Path = FPaths::Combine(ScreenshotDir, BaseName);
	FString Filename;
	FFileHelper::GenerateNextBitmapFilename(Path, FileExtension, Filename);

	const auto ImageView = FImageView(InColorData.GetData(), InSize.X, InSize.Y);
	const auto bSuccess = FImageUtils::SaveImageByExtension(*Filename, ImageView);

	return bSuccess ? Filename : FString();
}

FBSTScreenshotData UBlueprintScreenshotToolHandler::CaptureGraphEditor(TSharedPtr<SGraphEditor> InGraphEditor)
{
	if (!InGraphEditor)
	{
		return FBSTScreenshotData();
	}

	FVector2D CurrentViewLocation;
	FVector2D NewViewLocation;
	FVector2D WindowSize;
	float CachedZoomAmount;

	const auto* Settings = GetDefault<UBlueprintScreenshotToolSettings>();

	InGraphEditor->GetViewLocation(CurrentViewLocation, CachedZoomAmount);

	const auto& CachedGeometry = InGraphEditor->GetCachedGeometry();
	const auto SizeOfWidget = CachedGeometry.GetLocalSize();

	const auto& SelectedNodes = InGraphEditor->GetSelectedNodes();
	float DPIScale = FPlatformApplicationMisc::GetDPIScaleFactorAtPoint(0.0f, 0.0f);

	if (SelectedNodes.Num() > 0)
	{
		FSlateRect BoundsForSelectedNodes;

		InGraphEditor->GetBoundsForSelectedNodes(BoundsForSelectedNodes, Settings->ScreenshotPadding);
		NewViewLocation = BoundsForSelectedNodes.GetTopLeft();
		WindowSize = BoundsForSelectedNodes.GetSize();

		InGraphEditor->SetViewLocation(NewViewLocation, Settings->ZoomAmount);
	}
	else
	{
		NewViewLocation = CurrentViewLocation;
		WindowSize = SizeOfWidget * DPIScale;

		InGraphEditor->SetViewLocation(NewViewLocation, CachedZoomAmount);
	}

	WindowSize = WindowSize.ClampAxes(Settings->MinScreenshotSize, Settings->MaxScreenshotSize);

	InGraphEditor->ClearSelectionSet();

	const auto NewWindow = CreateTransparentWindowWithContent(WindowSize, InGraphEditor.ToSharedRef());
	const bool bShowImmediately = false;
	FSlateApplication::Get().AddWindow(NewWindow, bShowImmediately);
	NewWindow->ShowWindow();

	if (Settings->bNodeAppearanceFixup)
	{
		FixGraphNodesAppearance(InGraphEditor);
	}


	FBSTScreenshotData ScreenshotData;
	FSlateApplication::Get().TakeScreenshot(NewWindow, ScreenshotData.ColorData, ScreenshotData.Size);

	InGraphEditor->SetViewLocation(CurrentViewLocation, CachedZoomAmount);

	for (auto* SelectedNode : SelectedNodes)
	{
		if (UEdGraphNode* SelectedEdGraphNode = Cast<UEdGraphNode>(SelectedNode))
		{
			InGraphEditor->SetNodeSelection(SelectedEdGraphNode, true);
		}
	}

	NewWindow->HideWindow();
	NewWindow->RequestDestroyWindow();

	if (Settings->bNodeAppearanceFixup)
	{
		FixGraphNodesAppearance(InGraphEditor);
	}

	return ScreenshotData;
}


TSharedRef<SWindow> UBlueprintScreenshotToolHandler::CreateTransparentWindow(const FVector2D& InWindowSize)
{
	return SNew(SWindow)
		.CreateTitleBar(false)
		.ClientSize(InWindowSize)
		.ScreenPosition(FVector2D(0.0f, 0.0f))
		.AdjustInitialSizeAndPositionForDPIScale(false)
		.SaneWindowPlacement(false)
		.SupportsTransparency(EWindowTransparency::PerWindow)
		.InitialOpacity(0.0f);
}

TSharedRef<SWindow> UBlueprintScreenshotToolHandler::CreateTransparentWindowWithContent(const FVector2D& InWindowSize, TSharedRef<SWidget> InContent)
{
	auto Window = CreateTransparentWindow(InWindowSize);
	Window->SetContent(InContent);

	return Window;
}

void UBlueprintScreenshotToolHandler::ShowWindow(TSharedRef<SWindow> InWindow)
{
	const bool bShowImmediately = false;
	FSlateApplication::Get().AddWindow(InWindow, bShowImmediately);

	InWindow->ShowWindow();
	InWindow->Invalidate(EInvalidateWidgetReason::LayoutAndVolatility);
}

void UBlueprintScreenshotToolHandler::FixGraphNodesAppearance(TSharedPtr<SGraphEditor> InGraphEditor)
{
	InGraphEditor->Invalidate(EInvalidateWidgetReason::Paint);
	FSlateApplication::Get().Tick();
}

bool UBlueprintScreenshotToolHandler::HasAnySelectedNodes(const TSet<TSharedPtr<SGraphEditor>>& InGraphEditors)
{
	for (const auto& GraphEditor : InGraphEditors)
	{
		if (GraphEditor->GetSelectedNodes().Num() > 0)
		{
			return true;
		}
	}

	return false;
}
