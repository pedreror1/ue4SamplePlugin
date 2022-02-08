// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIBaseNode.h"
#include "Widgets/Text/STextBlock.h"
#include "UnrealEd/Public/LevelEditorViewport.h"
#include "../Classes/AIEditorStyle.h"
#include "ConnectionModeUI.h"
#include "ConnectionModeUI.h"
#include "ManagerModeUI.h"
#include "CoreMinimal.h"

/**
 *
 */
class AIEDITOR_API NodeModeUI
{
public:
	NodeModeUI();
	~NodeModeUI();
	static bool firstNode;
	static TSharedPtr<STextBlock> nodeName;
	static AAIBaseNode* currentNode;
	static bool isInAddMode;
	static int numOfNode;
	static TArray<TSharedPtr <FString>> nodesIDList;
	static TSharedPtr<STextBlock> addButtonLabel;
	static FReply addConnectionToNode();

	static AAIBaseNode* currentAddNode;

	static EVisibility NodeModeToggle();
	static FReply addConectionModeToggle();
	static void createNode(FVector WorldPosition, UWorld* world);
	static FReply SelectNode(int FromTo);
	static FReply deleteCurrentNode();
	static FReply exitNodeMode();
	static TSharedRef<SWidget> makeNodeButton(FText InLabel, const ANSICHAR* imageName);
	static EVisibility nodeDataModeToggle();
	static EVisibility nodeADDDataModeToggle();

	static bool nodeManagerValidator();
	static void endDragDrop();
};
