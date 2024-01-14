// Copyright 2024 Gradess Games. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "IImageWrapper.h"
#include "BlueprintScreenshotToolTypes.generated.h"

USTRUCT(BlueprintType)
struct BLUEPRINTSCREENSHOTTOOL_API FBSTScreenshotData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screenshot Data")
	TArray<FColor> ColorData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screenshot Data")
	FIntVector Size;

	FORCEINLINE FIntPoint GetPointSize() const
	{
		return FIntPoint(Size.X, Size.Y);
	}
};

UENUM()
enum class BLUEPRINTSCREENSHOTTOOL_API EBSTImageFormat : uint8
{
	PNG = EImageFormat::PNG,
	JPG = EImageFormat::JPEG,
	BMP = EImageFormat::BMP,
};
