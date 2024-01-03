// Copyright 2023 Gradess Games. All Rights Reserved.


#include "BlueprintScreenshotToolHandler.h"
#include "BlueprintEditor.h"
#include "BlueprintScreenshotToolSettings.h"
#include "BlueprintScreenshotToolTypes.h"
#include "IImageWrapperModule.h"
#include "ImageUtils.h"
#include "ImageWriteQueue.h"
#include "ImageWriteTask.h"
#include "Algo/RemoveIf.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Misc/FileHelper.h"

struct FWidgetSnapshotTextureData;

void UBlueprintScreenshotToolHandler::TakeScreenshot()
{
	auto GraphEditors = FindGraphEditors();
	if (GraphEditors.Num() <= 0)
	{
		return;
	}

	const auto Windows = FSlateApplication::Get().GetTopLevelWindows();
	for (auto Window : Windows)
	{
		const auto BlueprintDiff = FindChild(Window, TEXT("SBlueprintDiff"));
		if (!BlueprintDiff.IsValid())
		{
			continue;
		}

		const auto DiffToolBar = FindChild(BlueprintDiff, TEXT("SClippingHorizontalBox"));
		if (!DiffToolBar.IsValid())
		{
			continue;
		}

		const auto TextBlocks = FindChildren(DiffToolBar, TEXT("STextBlock"));
		for (auto TextBlock : TextBlocks)
		{
			const auto CastedTextBlock = StaticCastSharedPtr<STextBlock>(TextBlock);
			if (CastedTextBlock.IsValid())
			{
				const auto Text = CastedTextBlock->GetText().ToString();
				UE_LOG(LogTemp, Warning, TEXT("Text: %s"), *Text);
			}
		}
	}

	const auto bHasSelectedNodes = HasAnySelectedNodes(GraphEditors);
	for (const auto GraphEditor : GraphEditors)
	{
		if (bHasSelectedNodes && GraphEditor->GetSelectedNodes().Num() <= 0)
		{
			continue;
		}
		
		auto [ColorData, Size] = CaptureGraphEditor(GraphEditor);
		SaveScreenshot(ColorData, Size);
	}
}

void UBlueprintScreenshotToolHandler::SaveScreenshot(const TArray<FColor>& InColorData, const FIntVector& InSize)
{
	const auto ScreenshotDir = FPaths::ScreenShotDir();
	const auto BaseName = GetDefault<UBlueprintScreenshotToolSettings>()->ScreenshotBaseName;
	const auto FileExtension = FString(TEXT("png"));
	const auto Path = FPaths::Combine(ScreenshotDir, BaseName);
	FString Filename;
	FFileHelper::GenerateNextBitmapFilename(Path, FileExtension, Filename);

	const FImageView ImageView = FImageView(InColorData.GetData(), InSize.X, InSize.Y);
	FImageUtils::SaveImageByExtension(*Filename, ImageView);
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

TSharedPtr<SWidget> UBlueprintScreenshotToolHandler::FindParent(TSharedPtr<SWidget> InWidget, const FName& InParentWidgetType)
{
	if (!InWidget.IsValid())
	{
		return nullptr;
	}

	auto Parent = InWidget->GetParentWidget();
	if (Parent.IsValid())
	{
		const auto& Type = Parent->GetType();
		if (Type.IsEqual(InParentWidgetType))
		{
			return Parent;
		}

		return FindParent(Parent, InParentWidgetType);
	}

	return nullptr;
}

TSharedPtr<SWidget> UBlueprintScreenshotToolHandler::FindChild(TSharedPtr<SWidget> InWidget, const FName& InChildWidgetType)
{
	if (!InWidget.IsValid())
	{
		return nullptr;
	}

	auto* Children = InWidget->GetChildren();
	if (!Children)
	{
		return nullptr;
	}

	for (int32 i = 0; i < Children->Num(); i++)
	{
		TSharedRef<SWidget> Child = Children->GetChildAt(i);
		const auto& Type = Child->GetType();
		if (Type.IsEqual(InChildWidgetType))
		{
			return Child;
		}

		auto Widget = FindChild(Child, InChildWidgetType);
		if (Widget.IsValid())
		{
			return Widget;
		}
	}

	return nullptr;
}

TSet<TSharedPtr<SWidget>> UBlueprintScreenshotToolHandler::FindChildren(TSharedPtr<SWidget> InWidget, const FName& InChildWidgetType)
{
	if (!InWidget.IsValid())
	{
		return {};
	}

	auto* Children = InWidget->GetChildren();
	if (!Children)
	{
		return {};
	}

	TSet<TSharedPtr<SWidget>> Result;
	for (int32 i = 0; i < Children->Num(); i++)
	{
		TSharedRef<SWidget> Child = Children->GetChildAt(i);
		const auto& Type = Child->GetType();
		if (Type.IsEqual(InChildWidgetType))
		{
			Result.Add(Child);
		}

		auto Widgets = FindChildren(Child, InChildWidgetType);
		if (Widgets.Num() > 0)
		{
			Result.Append(Widgets);
		}
	}

	return Result;
}


TSet<TSharedPtr<SGraphEditor>> UBlueprintScreenshotToolHandler::FindGraphEditors()
{
	const auto ActiveTab = FGlobalTabmanager::Get()->GetActiveTab();
	const auto StandaloneAssetEditorToolkitHost = FindParent(ActiveTab, TEXT("SStandaloneAssetEditorToolkitHost"));
	if (!StandaloneAssetEditorToolkitHost.IsValid())
	{
		return {};
	}

	const auto Widgets = FindChildren(StandaloneAssetEditorToolkitHost, TEXT("SGraphEditorImpl"));
	TArray<TSharedPtr<SGraphEditor>> GraphEditors;

	Algo::Transform(Widgets, GraphEditors, [](TSharedPtr<SWidget> Widget) { return StaticCastSharedPtr<SGraphEditor>(Widget); });
	Algo::RemoveIf(GraphEditors, [](TSharedPtr<SGraphEditor> GraphEditor) { return !GraphEditor.IsValid(); });

	return TSet<TSharedPtr<SGraphEditor>>(GraphEditors);
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
