// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "../Classes/AIEditorManager.h"
#include "Widgets/Input/SComboBox.h"
#include <iostream>
#include <fstream>
#include "Toolkits/BaseToolkit.h"

class FAIEditorEdModeToolkit : public FModeToolkit
{
public:

	FAIEditorEdModeToolkit();
	
	/** FModeToolkit interface */
	virtual void Init(const TSharedPtr<IToolkitHost>& InitToolkitHost) override;
	void PreCreateManager(AIEditorManager* &wayPointMGG);
	bool SceneHasAIEditorMg();
	/** IToolkit interface */
	virtual FName GetToolkitFName() const override;


	TArray<TSharedPtr<FString> > SelectorComboBoxOptions;
	TSharedPtr<FString>	SelectorComboBoxSelectedItem;

	void OnSelectionChanged(TSharedPtr<FString>  NewValue, ESelectInfo::Type);
		virtual FText GetBaseToolkitName() const override;
	virtual class FEdMode* GetEditorMode() const override;
	virtual TSharedPtr<class SWidget> GetInlineContent() const override { return NodeManagerToolkit; }
	const FVector2D mousepos;
	UPROPERTY(EditAnywhere)
		static float scaleValue;
	UPROPERTY(EditAnywhere)
		static TSharedPtr<AIEditorManager> waypointMG;
 
	

private:
	
	TSharedPtr<SWidget> NodeManagerToolkit;
 
 

};
