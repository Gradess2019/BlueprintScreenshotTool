﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintScreenshotToolDiffWindowButton.h"

#include "BlueprintScreenshotToolHandler.h"
#include "BlueprintScreenshotToolSettings.h"
#include "BlueprintScreenshotToolStyle.h"

FBlueprintScreenshotToolDiffWindowButton::FBlueprintScreenshotToolDiffWindowButton(): FToolBarButtonBlock(
	GetDefault<UBlueprintScreenshotToolSettings>()->DiffWindowButtonLabel,
	GetDefault<UBlueprintScreenshotToolSettings>()->DiffWindowButtonToolTip,
	FSlateIcon(FBlueprintScreenshotToolStyle::GetStyleSetName(), FBlueprintScreenshotToolStyle::IconNameSmall),
	FUIAction(FExecuteAction::CreateStatic(&UBlueprintScreenshotToolHandler::TakeScreenshot)),
	EUserInterfaceActionType::Button
)
{
}
