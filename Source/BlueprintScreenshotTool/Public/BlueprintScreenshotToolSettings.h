// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BlueprintScreenshotToolSettings.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Config = BluerpintScreenshotTool)
class BLUEPRINTSCREENSHOTTOOL_API UBlueprintScreenshotToolSettings : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool")
	FString ScreenshotBaseName = FString(TEXT("GraphScreenshot"));
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool")
	float ScreenshotPadding = 128.f;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool")
	float MinScreenshotSize = 128.f;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool")
	float MaxScreenshotSize = 15360.f;

	// Default zoom amount that is used when taking screenshot of selected nodes
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool")
	float ZoomAmount = 1.f;

	// If true, the node appearance will be fixed up before taking the screenshot. Causes a slight flicker.
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool")
	bool bNodeAppearanceFixup = true;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool")
	bool bShowNotification = true;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool|DeveloperMode")
	bool bDeveloperMode = false;

	// The text is used for searching Blueprint Diff toolbar buttons to inject "Take Screenshot" button
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool|DeveloperMode|Diff", meta = (EditCondition = "bDeveloperMode"))
	TArray<FText> DiffToolbarTexts = { FText::FromString(TEXT("Lock/Unlock")), FText::FromString(TEXT("Vertical/Horizontal")) };

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool|DeveloperMode|Diff", meta = (EditCondition = "bDeveloperMode"))
	FText DiffWindowButtonLabel = FText::FromString(TEXT("Take Screenshot"));

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool|DeveloperMode|Diff", meta = (EditCondition = "bDeveloperMode"))
	FText DiffWindowButtonToolTip = FText();

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool|DeveloperMode|Notification", meta = (EditCondition = "bDeveloperMode"))
	FText NotificationMessageFormat = FText::FromString("{Count}|plural(one=Screenshot,other=Screenshots) taken: ");

	// If true, the notification will be persistent until manually closed
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool|DeveloperMode|Notification", meta = (EditCondition = "bDeveloperMode"))
	bool bPersistentNotification = false;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool|DeveloperMode|Notification", meta = (EditCondition = "bDeveloperMode && !bPersistentNotification"))
	float ExpireDuration = 5.f;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "BlueprintScreenshotTool|DeveloperMode|Notification", meta = (EditCondition = "bDeveloperMode"))
	bool bUseSuccessFailIcons = true;

};
