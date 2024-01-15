// Copyright 2024 Gradess Games. All Rights Reserved.


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
#include "Framework/Notifications/NotificationManager.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Misc/FileHelper.h"
#include "Widgets/Notifications/SNotificationList.h"

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

TArray<FString> UBlueprintScreenshotToolHandler::TakeScreenshotWithNotification()
{
	const auto Paths = TakeScreenshotWithPaths();

	if (Paths.Num() > 0)
	{
		ShowNotification(Paths);
	}

	return Paths;
}

void UBlueprintScreenshotToolHandler::TakeScreenshot()
{
	if (GetDefault<UBlueprintScreenshotToolSettings>()->bShowNotification)
	{
		TakeScreenshotWithNotification();
	}
	else
	{
		TakeScreenshotWithPaths();
	}
}

FString UBlueprintScreenshotToolHandler::SaveScreenshot(const TArray<FColor>& InColorData, const FIntVector& InSize)
{
	const auto* Settings = GetDefault<UBlueprintScreenshotToolSettings>();
	const auto ScreenshotDir = Settings->SaveDirectory.Path;
	const auto& BaseName = Settings->ScreenshotBaseName;
	const auto FileExtension = GetExtension(Settings->Extension);
	const auto Path = FPaths::Combine(ScreenshotDir, BaseName);

	FString Filename;
	FFileHelper::GenerateNextBitmapFilename(Path, FileExtension, Filename);

	const auto ImageView = FImageView(InColorData.GetData(), InSize.X, InSize.Y);
	const auto Quality = Settings->Extension == EBSTImageFormat::JPG ? Settings->Quality : 0;
	const auto bSuccess = FImageUtils::SaveImageByExtension(*Filename, ImageView, Quality);

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

void UBlueprintScreenshotToolHandler::OpenDirectory()
{
	const auto Path = FPaths::ConvertRelativePathToFull(GetDefault<UBlueprintScreenshotToolSettings>()->SaveDirectory.Path);
	if (FPaths::DirectoryExists(Path))
	{
		FPlatformProcess::ExploreFolder(*Path);
		
	}
	else
	{
		ShowDirectoryErrorNotification(Path);
	}
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

void UBlueprintScreenshotToolHandler::ShowNotification(const TArray<FString>& InPaths)
{
	checkf(InPaths.Num() > 0, TEXT("InPaths must not be empty"));

	FFormatOrderedArguments Arguments;
	Arguments.Add(InPaths.Num());

	const auto* Settings = GetDefault<UBlueprintScreenshotToolSettings>();
	const auto Message = FText::Format(Settings->NotificationMessageFormat, Arguments);

	FNotificationInfo NotificationInfo(Message);
	NotificationInfo.ExpireDuration = Settings->ExpireDuration;
	NotificationInfo.bFireAndForget = true;
	NotificationInfo.bUseSuccessFailIcons = Settings->bUseSuccessFailIcons;

	FString HyperLinkText;
	for (auto Id = 0; Id < InPaths.Num(); ++Id)
	{
		const auto& Path = InPaths[Id];
		const auto FullPath = FPaths::ConvertRelativePathToFull(Path);
		const auto bLast = Id == InPaths.Num() - 1;
		HyperLinkText += FString::Printf(TEXT("%s%s"), *FullPath, bLast ? TEXT("") : TEXT("\n"));
	}

	NotificationInfo.HyperlinkText = FText::FromString(HyperLinkText);

	FString HyperLinkPath = FPaths::ConvertRelativePathToFull(InPaths[0]);
	NotificationInfo.Hyperlink = FSimpleDelegate::CreateLambda([HyperLinkPath] { FPlatformProcess::ExploreFolder(*HyperLinkPath); });

	const auto Notification = FSlateNotificationManager::Get().AddNotification(NotificationInfo);
	Notification->SetCompletionState(SNotificationItem::CS_Success);
}

void UBlueprintScreenshotToolHandler::ShowDirectoryErrorNotification(const FString& InPath)
{
	const auto* Settings = GetDefault<UBlueprintScreenshotToolSettings>();
	FNotificationInfo NotificationInfo(FText::FromString(TEXT("Directory does not exist: \n") + InPath));
	NotificationInfo.ExpireDuration = Settings->ExpireDuration;
	NotificationInfo.bFireAndForget = true;
	NotificationInfo.bUseSuccessFailIcons = Settings->bUseSuccessFailIcons;

	const auto Notification = FSlateNotificationManager::Get().AddNotification(NotificationInfo);
	Notification->SetCompletionState(SNotificationItem::CS_Fail);
}

FString UBlueprintScreenshotToolHandler::GetExtension(EBSTImageFormat InFormat)
{
	switch (InFormat)
	{
	case EBSTImageFormat::PNG:
		return TEXT("png");
	case EBSTImageFormat::JPG:
		return TEXT("jpg");
	case EBSTImageFormat::BMP:
		return TEXT("bmp");
	default:
		checkf(false, TEXT("Unknown image format"));
		return TEXT("png");
	}
}
