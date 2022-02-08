// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Widgets/Text/STextBlock.h"
#include "CoreMinimal.h"
#include "AIEditorEdMode.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SScrollBox.h"

#include "EventNodeUI.h"
#include "EventObject.h"

/**
 * 
 */
class AIEDITOR_API boardingLineNodeUI
{

public:

 	boardingLineNodeUI();
	~boardingLineNodeUI();
	static EVisibility boardingLineNodeModeToggle();
	static void createBoardingLineNodeObject(AActor* eventParent, FVector WorldPosition, FVector direction,float angle, UWorld* world, bool isUnboarding=false);
	static void CreateEventNodesList();
	static void EventNodesUIRegionGenerator();
	static FReply DeleteEventNode(int num);
	static FReply DeleteUnboardingEventNode(int num);

	//static TSharedRef<SWidget> addEventNodesToUI(AActor* actor);
	static AEventObject* EventMG;
	static bool UIReady ;
	static void CreateNewNodeAtPosition();
	static void CreateNewUnboardingNodeAtPosition();

	static void  UpdateLines(UWorld* world);
	static TSharedPtr<SGridPanel> boardingLinenodePanel;

}; 
