// Copyright 2024 Gradess Games. All Rights Reserved.


#include "BlueprintScreenshotToolWindowManager.h"
#include "BlueprintScreenshotToolDiffWindowButton.h"
#include "BlueprintScreenshotToolSettings.h"
#include "SBlueprintDiff.h"
#include "Algo/RemoveIf.h"

void UBlueprintScreenshotToolWindowManager::Tick(float DeltaTime)
{
	AddScreenshotButtonToDiffs();
}

TStatId UBlueprintScreenshotToolWindowManager::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UBlueprintScreenshotToolWindowManager, STATGROUP_Tickables);
}

bool UBlueprintScreenshotToolWindowManager::IsAllowedToTick() const
{
	return IsValid(this);
}

TSharedPtr<SWidget> UBlueprintScreenshotToolWindowManager::FindParent(TSharedPtr<SWidget> InWidget, const FName& InParentWidgetType)
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

TSharedPtr<SWidget> UBlueprintScreenshotToolWindowManager::FindChild(TSharedPtr<SWidget> InWidget, const FName& InChildWidgetType)
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

TSet<TSharedPtr<SWidget>> UBlueprintScreenshotToolWindowManager::FindChildren(TSharedPtr<SWidget> InWidget, const FName& InChildWidgetType)
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

TSet<TSharedPtr<SGraphEditor>> UBlueprintScreenshotToolWindowManager::FindGraphEditors(TSharedRef<SWindow> InWindow)
{
	const auto Widgets = FindChildren(InWindow, TEXT("SGraphEditorImpl"));
	TArray<TSharedPtr<SGraphEditor>> GraphEditors;

	Algo::Transform(Widgets, GraphEditors, [](TSharedPtr<SWidget> Widget) { return StaticCastSharedPtr<SGraphEditor>(Widget); });
	auto Index = Algo::RemoveIf(GraphEditors, [](TSharedPtr<SGraphEditor> GraphEditor) { return !GraphEditor.IsValid(); });

	return TSet<TSharedPtr<SGraphEditor>>(GraphEditors);
}

TSet<TSharedPtr<SGraphEditor>> UBlueprintScreenshotToolWindowManager::FindActiveGraphEditors()
{
	const auto ActiveTab = FSlateApplication::Get().GetActiveTopLevelWindow();
	if (!ActiveTab.IsValid())
	{
		return {};
	}

	return FindGraphEditors(ActiveTab.ToSharedRef());
}

TSet<TSharedPtr<SGraphEditor>> UBlueprintScreenshotToolWindowManager::FindAllGraphEditors()
{
	auto Windows = GetWindows();

	TSet<TSharedPtr<SGraphEditor>> GraphEditors;
	for (const auto Window : Windows)
	{
		auto GraphEditorsInWindow = FindGraphEditors(Window);
		GraphEditors.Append(GraphEditorsInWindow);
	}

	return GraphEditors;
}

TArray<TSharedRef<SWindow>> UBlueprintScreenshotToolWindowManager::GetWindows()
{
	TArray<TSharedRef<SWindow>> Windows;
	FSlateApplication::Get().GetAllVisibleWindowsOrdered(Windows);
	return Windows;
}

TArray<TSharedRef<SBlueprintDiff>> UBlueprintScreenshotToolWindowManager::GetBlueprintDiffs()
{
	auto Windows = GetWindows();

	TArray<TSharedRef<SBlueprintDiff>> BlueprintDiffs;

	for (const auto Window : Windows)
	{
		const auto Child = FindChild<SBlueprintDiff>(Window);
		if (Child.IsValid())
		{
			BlueprintDiffs.Add(Child.ToSharedRef());
		}
	}

	return BlueprintDiffs;
}

void UBlueprintScreenshotToolWindowManager::AddScreenshotButtonToDiffs()
{
	const auto Diffs = GetBlueprintDiffs();
	for (const auto BlueprintDiff : Diffs)
	{
		AddButtonToDiffWindow(BlueprintDiff);
	}
}

void UBlueprintScreenshotToolWindowManager::AddButtonToDiffWindow(TSharedRef<SBlueprintDiff> InBlueprintDiff)
{
	const auto DiffToolBars = UBlueprintScreenshotToolWindowManager::FindChildren<SMultiBoxWidget>(InBlueprintDiff);
	if (DiffToolBars.Num() <= 0)
	{
		return;
	}

	TArray<TSharedPtr<SMultiBoxWidget>> FilteredToolBars = DiffToolBars.Array();
	FilteredToolBars = FilteredToolBars.FilterByPredicate([](const TSharedPtr<SMultiBoxWidget>& ToolBar)
	{
		bool bTakeScreenshotButtonExists = false;
		auto ToolbarTextToCheck = GetDefault<UBlueprintScreenshotToolSettings>()->DiffToolbarTexts;

		const auto TextBlocks = UBlueprintScreenshotToolWindowManager::FindChildren<STextBlock>(ToolBar);
		for (const auto TextBlock : TextBlocks)
		{
			const auto& ButtonText = TextBlock->GetText();
			const auto NumRemovedElems = ToolbarTextToCheck.RemoveAll([ButtonText](const FText& InText) { return InText.EqualToCaseIgnored(ButtonText); });

			if (NumRemovedElems == 0 && !bTakeScreenshotButtonExists)
			{
				const auto Label = GetDefault<UBlueprintScreenshotToolSettings>()->DiffWindowButtonLabel;
				bTakeScreenshotButtonExists = Label.EqualToCaseIgnored(ButtonText);
			}
		}

		const bool bAppropriateDiffToolBar = ToolbarTextToCheck.Num() <= 0;
		return bAppropriateDiffToolBar && !bTakeScreenshotButtonExists;
	});


	for (auto ToolBar : FilteredToolBars)
	{
		TSharedRef<FBlueprintScreenshotToolDiffWindowButton> NewToolBarButtonBlock(new FBlueprintScreenshotToolDiffWindowButton());
		TSharedRef<FMultiBox> MultiBoxCopy = MakeShareable(new FMultiBox(ToolBar->GetMultiBox().Get()));
		MultiBoxCopy->AddMultiBlock(NewToolBarButtonBlock);
		ToolBar->SetMultiBox(MultiBoxCopy);
		ToolBar->BuildMultiBoxWidget();
	}
}
