// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "EventObject.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Input/SButton.h"
#include "ManagerModeUI.h"
#include "UnrealEd/Public/LevelEditorViewport.h"
#include "boardingLineNodeUI.h"
#include "boardingLineNodeModeUI.h"

#include "Kismet/GameplayStatics.h"


/**
 *
 */
class AIEDITOR_API EventNodeUI
{
public:
	EventNodeUI();
	~EventNodeUI();
	static AEventObject* currentEventNode;
	static TSharedPtr<SSlider>						  EVRadiusSlider;
	static TSharedPtr<STextBlock>					  EVNodeIDData;
	static TSharedPtr<STextBlock>					  EVRadiusLabel;
	static TSharedPtr<SComboBox<TSharedPtr<FString>>> EVflagComboBox;
	static TSharedPtr<STextBlock>					  EVflagData;
	static TSharedPtr<SEditableTextBox>				  EVxPos;
	static TSharedPtr<SEditableTextBox>				  EVyPos;
	static TSharedPtr<SEditableTextBox>				  EVzPos;
	static TSharedPtr<SComboBox<TSharedPtr<FString>>> EVNodeIDComboBox;
	static TSharedPtr<FString>						  EVSelectedNodeIDItem;
	static TSharedPtr<FString>						  EVSelectedFlagItem;
	static int selectedNodeID;

	static TSharedRef<SWidget> addEventNodesToUI(AActor* actor);
	static TSharedPtr<SGridPanel> EventsnodePanel;


	static FReply DeleteEventNode(AActor* actor);
	static FReply DeleteCurrentEventNode();
	static FReply exitEventNodeMode();
	static void EventNodesUIRegionGenerator();
	static void CreateEventNodesList();
	static EVisibility EventNodeModeToggle();


};
