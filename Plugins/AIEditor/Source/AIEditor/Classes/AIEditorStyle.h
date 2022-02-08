// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Styling/ISlateStyle.h"

/**
 * 
 */
class AIEditorStyle
{
public:
	static void Initialize();
	static void Shutdown();
	static TSharedPtr<class ISlateStyle> Get();

private:
	static FString InContent(const FString& RelativePath, const ANSICHAR* Extension);

private:
	static TSharedPtr<class FSlateStyleSet> Styleset;
};
