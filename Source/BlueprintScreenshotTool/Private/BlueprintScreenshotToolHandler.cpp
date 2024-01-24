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
#include "RenderingThread.h"
#include "Engine/TextureRenderTarget2D.h"

#include "Framework/Notifications/NotificationManager.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Misc/FileHelper.h"
#include "Slate/WidgetRenderer.h"
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
	
	const auto SelectedNodes = InGraphEditor->GetSelectedNodes();
	
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

		const float DPIScale = FPlatformApplicationMisc::GetDPIScaleFactorAtPoint(0.0f, 0.0f);
		WindowSize = SizeOfWidget * DPIScale;
	
		InGraphEditor->SetViewLocation(NewViewLocation, CachedZoomAmount);
	}
	
	WindowSize = WindowSize.ClampAxes(Settings->MinScreenshotSize, Settings->MaxScreenshotSize);

	InGraphEditor->ClearSelectionSet();

	const auto RenderTarget = TStrongObjectPtr<UTextureRenderTarget2D>(DrawGraphEditor(InGraphEditor, WindowSize));

	FBSTScreenshotData ScreenshotData;
	ScreenshotData.Size = FIntVector(WindowSize.X, WindowSize.Y, 0);
	RenderTarget->GameThread_GetRenderTargetResource()->ReadPixels(ScreenshotData.ColorData);

	RestoreNodeSelection(InGraphEditor, SelectedNodes);
	InGraphEditor->SetViewLocation(CurrentViewLocation, CachedZoomAmount);

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

void UBlueprintScreenshotToolHandler::RestoreNodeSelection(TSharedPtr<SGraphEditor> InGraphEditor, const TSet<UObject*>& InSelectedNodes)
{
	for (const auto NodeObject : InSelectedNodes)
	{
		if (auto* SelectedNode = Cast<UEdGraphNode>(NodeObject))
		{
			InGraphEditor->SetNodeSelection(SelectedNode, true);
		}
	}
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

UTextureRenderTarget2D* UBlueprintScreenshotToolHandler::DrawGraphEditor(TSharedPtr<SGraphEditor> InGraphEditor, const FVector2D& InWindowSize)
{
	constexpr auto bUseGamma = true;
	constexpr auto DrawTimes = 2;
	constexpr auto Filter = TF_Default;
	
	FWidgetRenderer* WidgetRenderer = new FWidgetRenderer(bUseGamma, true);
	UTextureRenderTarget2D* RenderTarget = FWidgetRenderer::CreateTargetFor(
		InWindowSize,
		Filter,
		bUseGamma
	);

	if (!ensureMsgf(IsValid(RenderTarget), TEXT("RenderTarget is not valid")))
	{
		return nullptr;
	}
	
	if (bUseGamma)
	{
		RenderTarget->bForceLinearGamma = true;
		RenderTarget->UpdateResourceImmediate(true);
	}

	for (int32 Count = 0; Count < DrawTimes; Count++)
	{
		constexpr auto RenderingScale = 1.f;
		constexpr auto DeltaTime = 0.f;
		WidgetRenderer->DrawWidget(
			RenderTarget,
			InGraphEditor.ToSharedRef(),
			RenderingScale,
			InWindowSize,
			DeltaTime
		);
		
		FlushRenderingCommands();
	}
	
	BeginCleanup(WidgetRenderer);

	return RenderTarget;
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
