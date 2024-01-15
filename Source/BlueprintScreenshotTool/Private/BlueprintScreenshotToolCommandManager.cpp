// Copyright 2024 Gradess Games. All Rights Reserved.


#include "BlueprintScreenshotToolCommandManager.h"
#include "BlueprintScreenshotToolCommands.h"
#include "BlueprintScreenshotToolHandler.h"
#include "Toolkits/AssetEditorToolkit.h"

void FBlueprintScreenshotToolCommandManager::RegisterCommands()
{
	FBlueprintScreenshotToolCommands::Register();

	MapCommands();
	RegisterToolbarExtension();
}

void FBlueprintScreenshotToolCommandManager::UnregisterCommands()
{
	UnregisterToolbarExtension();
	FBlueprintScreenshotToolCommands::Unregister();
}

void FBlueprintScreenshotToolCommandManager::OnTakeScreenshot()
{
}

void FBlueprintScreenshotToolCommandManager::MapCommands()
{
	CommandList = MakeShareable(new FUICommandList());

	CommandList->MapAction(
		FBlueprintScreenshotToolCommands::Get().TakeScreenshot,
		FExecuteAction::CreateStatic(UBlueprintScreenshotToolHandler::TakeScreenshot),
		FCanExecuteAction());
}

void FBlueprintScreenshotToolCommandManager::RegisterToolbarExtension()
{
	auto ExtensibilityManager = FAssetEditorToolkit::GetSharedToolBarExtensibilityManager();
	checkf(ExtensibilityManager.IsValid(), TEXT("ToolBarExtensibilityManager is not valid"));
	
	ToolbarExtension = MakeShareable(new FExtender());
	ToolbarExtension->AddToolBarExtension("Asset", EExtensionHook::After, CommandList, FToolBarExtensionDelegate::CreateStatic(&FBlueprintScreenshotToolCommandManager::AddToolbarExtension));
	ExtensibilityManager->AddExtender(ToolbarExtension);
}

void FBlueprintScreenshotToolCommandManager::UnregisterToolbarExtension()
{
	const auto ExtensibilityManager = FAssetEditorToolkit::GetSharedToolBarExtensibilityManager();
	if (ExtensibilityManager.IsValid())
	{
		ExtensibilityManager->RemoveExtender(ToolbarExtension);
	}
}

void FBlueprintScreenshotToolCommandManager::AddToolbarExtension(FToolBarBuilder& ToolBarBuilder)
{
	ToolBarBuilder.AddToolBarButton(FBlueprintScreenshotToolCommands::Get().TakeScreenshot);
}
