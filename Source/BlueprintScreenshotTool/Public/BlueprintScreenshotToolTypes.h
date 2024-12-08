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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screenshot Data")
	FString CustomName;

	FORCEINLINE FIntPoint GetPointSize() const
	{
		return FIntPoint(Size.X, Size.Y);
	}

	FORCEINLINE bool IsValid() const
	{
		return ColorData.Num() > 0 && Size.X > 0 && Size.Y > 0;
	}
};

UENUM()
enum class EBSTImageFormat : uint8
{
	PNG = EImageFormat::PNG,
	JPG = EImageFormat::JPEG
};
