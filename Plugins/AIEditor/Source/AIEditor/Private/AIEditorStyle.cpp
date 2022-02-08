// Fill out your copyright notice in the Description page of Project Settings.
#include "../Classes/AIEditorStyle.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "HAL/FileManager.h"
#include "Interfaces/IPluginManager.h"
//#include "Interfaces/IPluginManager.h"
TSharedPtr<class ISlateStyle> AIEditorStyle::Get()
{
	return Styleset;
}
void AIEditorStyle::Initialize()
{
	if (Styleset.IsValid())
	{
		return;
	}

	Styleset = MakeShareable(new FSlateStyleSet("AIEditorStyleSlate"));

	FString ProjectResourceDir = FPaths::ProjectPluginsDir() / TEXT("AIEditor/Resources");
	FString EngineResourceDir = FPaths::EnginePluginsDir() / TEXT("AIEditor/Resources");

	if (IFileManager::Get().DirectoryExists(*ProjectResourceDir)) //Is the plugin in the project? In that case, use those resources
	{
		Styleset->SetContentRoot(ProjectResourceDir);
		Styleset->SetCoreContentRoot(ProjectResourceDir);
	}
	else //Otherwise, use the global ones
	{
		Styleset->SetContentRoot(EngineResourceDir);
		Styleset->SetCoreContentRoot(EngineResourceDir);
	}
	FString f = AIEditorStyle::InContent("brain", ".png");
	Styleset->Set("AIEditor.Icon", new FSlateImageBrush(f, FVector2D(40.0f, 40.0f)));

	f = AIEditorStyle::InContent("graphnode", ".png");
	Styleset->Set("waypoint.Icon", new FSlateImageBrush(f, FVector2D(150, 150.0f)));
	f = AIEditorStyle::InContent("roboto", ".png");
	Styleset->Set("Roboto.icon", new FSlateImageBrush(f, FVector2D(150, 150.0f)));
	
	f = AIEditorStyle::InContent("EventIcon", ".png");
	Styleset->Set("EventIcon.icon", new FSlateImageBrush(f, FVector2D(150, 150.0f)));
	
	f = AIEditorStyle::InContent("boardingLineNode", ".png");
	Styleset->Set("boardingLineNode.icon", new FSlateImageBrush(f, FVector2D(150, 150.0f)));

	f = AIEditorStyle::InContent("unboardingLineNode", ".png");
	Styleset->Set("unboardingLineNode.icon", new FSlateImageBrush(f, FVector2D(150, 150.0f)));

	FSlateStyleRegistry::RegisterSlateStyle(*Styleset.Get());
}
void AIEditorStyle::Shutdown()
{
	if (Styleset.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*Styleset.Get());
		ensure(Styleset.IsUnique());
		Styleset.Reset();
	}
}


TSharedPtr<FSlateStyleSet> AIEditorStyle::Styleset = nullptr;

FString AIEditorStyle::InContent(const FString & RelativePath, const ANSICHAR * Extension)
{
	auto myself = IPluginManager::Get().FindPlugin(TEXT("AIEditor"));
	check(myself.IsValid());
	static FString ContentDir = myself->GetBaseDir() / TEXT("Resources");
	FString url = "../../../Resources/" + RelativePath + Extension;
	//return url;
	return (ContentDir / RelativePath) + Extension;

}
