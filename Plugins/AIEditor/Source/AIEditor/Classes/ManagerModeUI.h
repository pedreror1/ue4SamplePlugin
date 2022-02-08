// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "AIEditorManager.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "AIEditorEdMode.h"
#include "UnrealEd/Public/LevelEditorViewport.h"
 #include "CoreMinimal.h"

/**
 * 
 */
class AIEDITOR_API ManagerModeUI
{
public:
	ManagerModeUI();
	~ManagerModeUI();
	static TSharedPtr<FString>						  selectedItem;
	static TSharedPtr<STextBlock>					  comboData;
	static TSharedPtr<SComboBox<TSharedPtr<FString>>> selectorComboBox;
 	static TSharedPtr<SEditableTextBox>				  xPos;
	static TSharedPtr<SEditableTextBox>				  yPos;
	static TSharedPtr<SEditableTextBox>				  zPos;
	static TSharedPtr<STextBlock> toNodeName;
	static AAIBaseNode* currentNode;
	static FVector lastPos;
	static bool isInAddMode;
	static AAINodeConnecter* currentConnection;
	static TSharedPtr<SEditableTextBox>				  levelID;
	static TSharedPtr<SEditableTextBox>				  levelLocation;
	static TSharedPtr<SEditableTextBox>				  width;
	static TSharedPtr<SEditableTextBox>				  height;
	static TSharedPtr<SEditableTextBox>				  depth;
	static TSharedPtr<SEditableTextBox>				  columns;
	static TSharedPtr<SEditableTextBox>				  rows;
	static TSharedPtr<SEditableTextBox>				  Layers;
	static TSharedPtr<FString>						  currentDirection;
	static TSharedPtr<STextBlock>					  directionComboData;
	static TSharedPtr<SComboBox<TSharedPtr<FString>>> directionComboBox;
	static AIEditorManager* wayPointMGG;


	static enum Mode
	{
		managerMode,
		nodeMode,
		connectionMode,
		addMode,
		eventMode,
		eventNodeMode,
		noMode,
		selectNodeIdMode,
		boardingNodeMode,
	};
	static Mode currentMode;
	static FReply SetupStartPos();
	static UWorld* getLocalWorld();
	 
	static FText getActorName(AActor* actor);
	static FReply moveCameraToPosition(FVector destination);
	 
};
