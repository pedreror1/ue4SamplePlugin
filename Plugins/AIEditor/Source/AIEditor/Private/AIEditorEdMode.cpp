// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "AIEditorEdMode.h"
#include "AIEditorEdModeToolkit.h"
#include "Toolkits/ToolkitManager.h"
#include "EditorModeManager.h"

const FEditorModeID FAIEditorEdMode::EM_AIEditorEdModeId = TEXT("EM_AIEditorEdMode");

FAIEditorEdMode::FAIEditorEdMode()
{
	
}

FAIEditorEdMode::~FAIEditorEdMode()
{

}

void FAIEditorEdMode::Enter()
{
 	FEdMode::Enter();

	if (!Toolkit.IsValid() && UsesToolkits())
	{
		Toolkit = MakeShareable(new FAIEditorEdModeToolkit);
		Toolkit->Init(Owner->GetToolkitHost());
	}
}

void FAIEditorEdMode::Exit()
{
 	if (Toolkit.IsValid())
	{
		FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
		Toolkit.Reset();
	}

	// Call base Exit method to ensure proper cleanup
	FEdMode::Exit();
}

bool FAIEditorEdMode::UsesToolkits() const
{
	return true;
	return true;
}




