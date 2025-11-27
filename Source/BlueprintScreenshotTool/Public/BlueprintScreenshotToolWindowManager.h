// Copyright 2024 Gradess Games. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BlueprintScreenshotToolWindowManager.generated.h"

class SBlueprintDiff;

UCLASS()
class BLUEPRINTSCREENSHOTTOOL_API UBlueprintScreenshotToolWindowManager : public UObject, public FTickableEditorObject
{
	GENERATED_BODY()

public:
	//~ Begin FTickableEditorObject Interface
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsAllowedToTick() const override;
	//~ End FTickableEditorObject Interface

	static TSharedPtr<SWidget> FindParent(TSharedPtr<SWidget> InWidget, const FName& InParentWidgetType);
	static TSharedPtr<SWidget> FindChild(TSharedPtr<SWidget> InWidget, const FName& InChildWidgetType);
	static TSet<TSharedPtr<SWidget>> FindChildren(TSharedPtr<SWidget> InWidget, const FName& InChildWidgetType);
	static TSet<TSharedPtr<SGraphEditor>> FindGraphEditors(TSharedRef<SWindow> InWindow);
	static TSet<TSharedPtr<SGraphEditor>> FindActiveGraphEditors();
	static TSet<TSharedPtr<SGraphEditor>> FindAllGraphEditors();

#if !PLATFORM_MAC
	template <typename T>
	static FName GetTemplateName()
	{
		const auto ClassDefinition = FString(typeid(T).name());
		return *ClassDefinition.Replace(TEXT("class"), TEXT("")).TrimStartAndEnd();
	}

	template <typename T>
	static TSharedPtr<T> FindParent(TSharedPtr<SWidget> InWidget)
	{
		return StaticCastSharedPtr<T>(FindParent(InWidget, GetTemplateName<T>()));
	}

	template <typename T>
	static TSharedPtr<T> FindChild(TSharedPtr<SWidget> InWidget)
	{
		return StaticCastSharedPtr<T>(FindChild(InWidget, GetTemplateName<T>()));
	}

	template <typename T>
	static TSet<TSharedPtr<T>> FindChildren(TSharedPtr<SWidget> InWidget)
	{
		auto Widgets = FindChildren(InWidget, GetTemplateName<T>());

		TSet<TSharedPtr<T>> Result;

		Algo::Transform(Widgets, Result, [](TSharedPtr<SWidget> Widget) { return StaticCastSharedPtr<T>(Widget); });
		return Result;
	}
#else
	template <typename T>
	static TSharedPtr<T> FindParent(TSharedPtr<SWidget> InWidget, const FName& InParentWidgetType)
	{
		return StaticCastSharedPtr<T>(FindParent(InWidget, InParentWidgetType));
	}

	template <typename T>
	static TSharedPtr<T> FindChild(TSharedPtr<SWidget> InWidget, const FName& InChildWidgetType)
	{
		return StaticCastSharedPtr<T>(FindChild(InWidget, InChildWidgetType));
	}

	template <typename T>
	static TSet<TSharedPtr<T>> FindChildren(TSharedPtr<SWidget> InWidget, const FName& InChildWidgetType)
	{
		auto Widgets = FindChildren(InWidget, InChildWidgetType);

		TSet<TSharedPtr<T>> Result;

		Algo::Transform(Widgets, Result, [](TSharedPtr<SWidget> Widget) { return StaticCastSharedPtr<T>(Widget); });
		return Result;
	}
#endif // PLATFORM_MAC

	static TArray<TSharedRef<SWindow>> GetWindows();
	static TArray<TSharedRef<SBlueprintDiff>> GetBlueprintDiffs();

protected:
	static void AddScreenshotButtonToDiffs();
	static void AddButtonToDiffWindow(TSharedRef<SBlueprintDiff> InBlueprintDiff);
};
