// Copyright 2023 Gradess Games. All Rights Reserved.


#include "BlueprintScreenshotToolHandler.h"
#include "BlueprintEditor.h"

void UBlueprintScreenshotToolHandler::TakeScreenshot()
{
}

TSharedPtr<SWidget> UBlueprintScreenshotToolHandler::FindParent(TSharedPtr<SWidget> InWidget, const FName& InParentWidgetType)
{
	if (!InWidget.IsValid())
	{
		return nullptr;
	}

	auto Parent = InWidget->GetParentWidget();
	if (Parent.IsValid())
	{
		const auto& Type = Parent->GetType();
		if (Type.IsEqual(InParentWidgetType))
		{
			return Parent;
		}

		return FindParent(Parent, InParentWidgetType);
	}

	return nullptr;
}

TSharedPtr<SWidget> UBlueprintScreenshotToolHandler::FindChild(TSharedPtr<SWidget> InWidget, const FName& InChildWidgetType)
{
	if (!InWidget.IsValid())
	{
		return nullptr;
	}

	auto* Children = InWidget->GetChildren();
	if (!Children)
	{
		return nullptr;
	}

	for (int32 i = 0; i < Children->Num(); i++)
	{
		TSharedRef<SWidget> Child = Children->GetChildAt(i);
		const auto& Type = Child->GetType();
		if (Type.IsEqual(InChildWidgetType))
		{
			return Child;
		}

		auto Widget = FindChild(Child, InChildWidgetType);
		if (Widget.IsValid())
		{
			return Widget;
		}
	}
	
	return nullptr;
}

TSet<TSharedPtr<SWidget>> UBlueprintScreenshotToolHandler::FindChildren(TSharedPtr<SWidget> InWidget, const FName& InChildWidgetType)
{
	if (!InWidget.IsValid())
	{
		return {};
	}

	auto* Children = InWidget->GetChildren();
	if (!Children)
	{
		return {};
	}

	TSet<TSharedPtr<SWidget>> Result;
	for (int32 i = 0; i < Children->Num(); i++)
	{
		TSharedRef<SWidget> Child = Children->GetChildAt(i);
		const auto& Type = Child->GetType();
		if (Type.IsEqual(InChildWidgetType))
		{
			Result.Add(Child);
		}

		auto Widgets = FindChildren(Child, InChildWidgetType);
		if (Widgets.Num() > 0)
		{
			Result.Append(Widgets);
		}
	}

	return Result;
}


TSet<TSharedPtr<SWidget>> UBlueprintScreenshotToolHandler::FindGraphEditors()
{
	const auto ActiveTab = FGlobalTabmanager::Get()->GetActiveTab();
	const auto StandaloneAssetEditorToolkitHost = FindParent(ActiveTab, TEXT("SStandaloneAssetEditorToolkitHost"));
	if (!StandaloneAssetEditorToolkitHost.IsValid())
	{
		return {};
	}

	const auto GraphEditors = FindChildren(StandaloneAssetEditorToolkitHost, TEXT("SGraphEditorImpl"));

	return GraphEditors;
}
