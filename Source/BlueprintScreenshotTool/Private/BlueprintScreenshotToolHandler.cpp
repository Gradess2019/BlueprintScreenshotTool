// Copyright 2023 Gradess Games. All Rights Reserved.


#include "BlueprintScreenshotToolHandler.h"
#include "BlueprintEditor.h"
#include "BlueprintScreenshotToolSettings.h"
#include "BlueprintScreenshotToolTypes.h"
#include "IImageWrapperModule.h"
#include "ImageUtils.h"
#include "ImageWriteQueue.h"
#include "ImageWriteTask.h"
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

	auto Widget = GraphEditors[FSetElementId::FromInteger(0)];
	if (Widget.IsValid())
	{
		auto GraphEditor = StaticCastSharedPtr<SGraphEditor>(Widget);
		if (GraphEditor.IsValid())
		{
			auto [ColorData, Size] = CaptureGraphEditor(GraphEditor);
			const auto ScreenshotDir = FPaths::ScreenShotDir();
			const auto BaseName = GetDefault<UBlueprintScreenshotToolSettings>()->ScreenshotBaseName;
			const auto FileExtension = FString(TEXT("png"));
			const auto Path = FPaths::Combine(ScreenshotDir, BaseName);
			FString Filename;
			FFileHelper::GenerateNextBitmapFilename(Path, FileExtension, Filename);

			FImageView ImageView = FImageView(ColorData.GetData(), Size.X, Size.Y);
			FImageUtils::SaveImageByExtension(*Filename, ImageView);
		}
	}
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
	// auto WidgetWindow = FSlateApplication::Get().FindWidgetWindow(InGraphEditor.ToSharedRef());
	FSlateApplication::Get().AddWindow(NewWindow, bShowImmediately);
	FSlateApplication::Get().Tick();

	InGraphEditor->Invalidate(EInvalidateWidgetReason::Paint);
	NewWindow->ShowWindow();

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


TSet<TSharedPtr<SWidget>> UBlueprintScreenshotToolHandler::FindGraphEditors()
{
	const auto ActiveTab = FGlobalTabmanager::Get()->GetActiveTab();
	const auto StandaloneAssetEditorToolkitHost = FindParent(ActiveTab, TEXT("SStandaloneAssetEditorToolkitHost"));
	if (!StandaloneAssetEditorToolkitHost.IsValid())
	{
		return {};
	}

	const auto GraphEditors = FindChildren(StandaloneAssetEditorToolkitHost, TEXT("SGraphEditorImpl"));

	return GraphEditors;
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
