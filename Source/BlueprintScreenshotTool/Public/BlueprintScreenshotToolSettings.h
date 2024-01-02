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

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Blueprint Screenshot Tool")
	float ZoomAmount = 1.f;
};
