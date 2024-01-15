// Copyright Epic Games, Inc. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FBlueprintScreenshotToolCommandManager;

class FBlueprintScreenshotToolModule : public IModuleInterface
{
private:
	TSharedPtr<FBlueprintScreenshotToolCommandManager> CommandManager;

public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

protected:
	void RegisterStyle();
	void RegisterCommands();
	void RegisterSettings();

	void UnregisterStyle();
	void UnregisterCommands();
	void UnregisterSettings();
};
