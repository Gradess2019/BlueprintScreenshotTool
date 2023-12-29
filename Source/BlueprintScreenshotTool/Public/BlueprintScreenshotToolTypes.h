// Copyright 2023 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintScreenshotToolTypes.generated.h"

USTRUCT(BlueprintType)
struct FBSTScreenshotData
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
