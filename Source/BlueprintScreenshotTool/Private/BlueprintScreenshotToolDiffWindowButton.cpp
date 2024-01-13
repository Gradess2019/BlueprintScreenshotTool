// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintScreenshotToolDiffWindowButton.h"

#include "BlueprintScreenshotToolSettings.h"
#include "BlueprintScreenshotToolStyle.h"

FBlueprintScreenshotToolDiffWindowButton::FBlueprintScreenshotToolDiffWindowButton(): FToolBarButtonBlock(
	GetDefault<UBlueprintScreenshotToolSettings>()->DiffWindowButtonLabel,
	GetDefault<UBlueprintScreenshotToolSettings>()->DiffWindowButtonToolTip,
	FSlateIcon(FBlueprintScreenshotToolStyle::GetStyleSetName(), FBlueprintScreenshotToolStyle::IconNameSmall),
	FUIAction(FExecuteAction::CreateRaw(this, &FBlueprintScreenshotToolDiffWindowButton::TakeScreenshot)),
	EUserInterfaceActionType::Button
)
{
}

void FBlueprintScreenshotToolDiffWindowButton::TakeScreenshot()
{
}
