// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "../Public/AIEditor.h"
#include "../Public/AIEditorEdMode.h"
#include "../Classes/AIEditorStyle.h"

#define LOCTEXT_NAMESPACE "FAIEditorModule"

void FAIEditorModule::StartupModule()
{
	AIEditorStyle::Initialize();
	FName xxx = AIEditorStyle::Get()->GetStyleSetName();

	FSlateIcon AIEIcon = FSlateIcon(xxx, "AIEditor.Icon");
	
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FEditorModeRegistry::Get().RegisterMode<FAIEditorEdMode>(FAIEditorEdMode::EM_AIEditorEdModeId, LOCTEXT("AIEditorEdModeName", "AIEditorEdMode"), AIEIcon, true);
}

void FAIEditorModule::ShutdownModule()
{
	AIEditorStyle::Shutdown();
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FEditorModeRegistry::Get().UnregisterMode(FAIEditorEdMode::EM_AIEditorEdModeId);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAIEditorModule, AIEditor)