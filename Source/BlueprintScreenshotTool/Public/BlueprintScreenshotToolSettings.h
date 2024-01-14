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
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Blueprint Screenshot Tool")
	FString ScreenshotBaseName = FString(TEXT("GraphScreenshot"));
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Blueprint Screenshot Tool")
	float ScreenshotPadding = 128.f;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Blueprint Screenshot Tool")
	float MinScreenshotSize = 128.f;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Blueprint Screenshot Tool")
	float MaxScreenshotSize = 15360.f;

	// Default zoom amount that is used when taking screenshot of selected nodes
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Blueprint Screenshot Tool")
	float ZoomAmount = 1.f;

	// If true, the node appearance will be fixed up before taking the screenshot. Causes a slight flicker.
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Blueprint Screenshot Tool")
	bool bNodeAppearanceFixup = true;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Blueprint Screenshot Tool | Developer Mode")
	bool bDeveloperMode = true;

	// The text is used for searching Blueprint Diff toolbar buttons to inject "Take Screenshot" button
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Blueprint Screenshot Tool | Developer Mode")
	TArray<FText> DiffToolbarTexts = { FText::FromString(TEXT("Lock/Unlock")), FText::FromString(TEXT("Vertical/Horizontal")) };

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Blueprint Screenshot Tool | Developer Mode")
	FText DiffWindowButtonLabel = FText::FromString(TEXT("Take Screenshot"));

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Blueprint Screenshot Tool | Developer Mode")
	FText DiffWindowButtonToolTip = FText(); 
};
