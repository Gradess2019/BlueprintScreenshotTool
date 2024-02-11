// Copyright 2024 Gradess Games. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "BlueprintScreenshotToolTypes.h"
#include "UObject/Object.h"
#include "BlueprintScreenshotToolSettings.generated.h"

UCLASS(BlueprintType, Config = BluerpintScreenshotTool)
class BLUEPRINTSCREENSHOTTOOL_API UBlueprintScreenshotToolSettings : public UObject
{
	GENERATED_BODY()

public:
	// If enabled the screenshot will be saved with the custom name
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool|General", meta = (InlineEditConditionToggle))
	bool bOverrideScreenshotNaming = false;

	// Will be used as a base name for the screenshot, instead of format <AssetName>_<GraphName>
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool|General", meta = (EditCondition = "bOverrideScreenshotNaming"))
	FString ScreenshotBaseName = FString(TEXT("GraphScreenshot"));

	// Screenshot file format
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool|General")
	EBSTImageFormat Extension = EBSTImageFormat::PNG;

	// Quality of jpg image. 10-100
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool|General", meta = (EditCondition = "Extension == EBSTImageFormat::JPG", ClampMin = "10", ClampMax = "100", UIMin = "10", UIMax = "100"))
	int32 Quality = 100;
	
	// Directory where the screenshots will be saved
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool|General", meta = (RelativePath))
	FDirectoryPath SaveDirectory = { FPaths::ScreenShotDir() };
	
	// Padding around selected graph nodes in pixels when taking screenshot
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool|General")
	int32 ScreenshotPadding = 128;

	// Minimum screenshot size in pixels
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool|General")
	int32 MinScreenshotSize = 128;

	// Maximum screenshot size in pixels
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool|General")
	int32 MaxScreenshotSize = 15360;

	// Default zoom amount that is used when taking screenshot of selected nodes. ATTENTION: It will scale the size of the screenshot as well. This operation can be slow.
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool|General", meta = (ClampMin = "0.1", ClampMax = "2", UIMin = "0.1", UIMax = "2"))
	float ZoomAmount = 1.f;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool|Hotkeys", meta = (ConfigRestartRequired = true))
	FInputChord TakeScreenshotHotkey = FInputChord(EModifierKey::Control, EKeys::F7);
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool|Hotkeys", meta = (ConfigRestartRequired = true))
	FInputChord OpenDirectoryHotkey = FInputChord(EModifierKey::Control, EKeys::F8);

	// If true, the notification with hyperlink to the screenshot will be shown after taking the screenshot
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool|Notification")
	bool bShowNotification = true;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool|DeveloperMode|Notification", meta = (EditCondition="bDeveloperMode"))
	FText NotificationMessageFormat = FText::FromString("{Count}|plural(one=Screenshot,other=Screenshots) taken: ");

	// How long the notification will be shown in seconds
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool|Notification")
	float ExpireDuration = 5.f;

	// If true, the notification will use success/fail icons instead of the default info icon
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool|DeveloperMode|Notification", meta = (EditCondition="bDeveloperMode"))
	bool bUseSuccessFailIcons = true;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool|DeveloperMode")
	bool bDeveloperMode = false;

	// The text is used for searching Blueprint Diff toolbar buttons to inject "Take Screenshot" button
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool|DeveloperMode", meta = (EditCondition = "bDeveloperMode"))
	TArray<FText> DiffToolbarTexts = {FText::FromString(TEXT("Lock/Unlock")), FText::FromString(TEXT("Vertical/Horizontal"))};

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool|DeveloperMode", meta = (EditCondition = "bDeveloperMode"))
	FText DiffWindowButtonLabel = FText::FromString(TEXT("Take Screenshot"));

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool|DeveloperMode", meta = (EditCondition = "bDeveloperMode"))
	FText DiffWindowButtonToolTip = FText::FromString(TEXT("Take screenshot of the shown diff graphs"));
};
