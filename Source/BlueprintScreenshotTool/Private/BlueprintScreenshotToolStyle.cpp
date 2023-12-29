// Copyright 2023 Gradess Games. All Rights Reserved.

#include "BlueprintScreenshotToolStyle.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleMacros.h"

static const FVector2D Icon20x20(20.0f, 20.0f);
static const FVector2D Icon40x40(40.0f, 40.0f);

TSharedPtr<FBlueprintScreenshotToolStyle::FStyle> FBlueprintScreenshotToolStyle::StyleInstance = nullptr;

FBlueprintScreenshotToolStyle::FStyle::FStyle() : FSlateStyleSet(FBlueprintScreenshotToolStyle::GetStyleSetName())
{
}

void FBlueprintScreenshotToolStyle::FStyle::Initialize()
{
	auto& PluginManager = IPluginManager::Get();
	const auto Plugin = PluginManager.FindPlugin("BlueprintScreenshotTool");
	const auto ResourcesDir = Plugin->GetBaseDir() / TEXT("Resources");

	StyleInstance->SetContentRoot(ResourcesDir);
	StyleInstance->Set("BlueprintScreenshotTool.TakeScreenshot", new IMAGE_BRUSH_SVG(TEXT("BlueprintScreenshotTool"), Icon40x40));
	StyleInstance->Set("BlueprintScreenshotTool.TakeScreenshot.Small", new IMAGE_BRUSH_SVG(TEXT("BlueprintScreenshotTool"), Icon20x20));

	FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
}

void FBlueprintScreenshotToolStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = MakeShareable(new FBlueprintScreenshotToolStyle::FStyle());
		StyleInstance->Initialize();
	}
}

void FBlueprintScreenshotToolStyle::Shutdown()
{
	if (StyleInstance.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance.Get());
		ensure(StyleInstance.IsUnique());
		StyleInstance.Reset();
	}
}

void FBlueprintScreenshotToolStyle::ReloadTextures()
{
	FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
}

const ISlateStyle& FBlueprintScreenshotToolStyle::Get()
{
	return *StyleInstance;
}

FName FBlueprintScreenshotToolStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("BlueprintScreenshotToolStyle"));
	return StyleSetName;
}
