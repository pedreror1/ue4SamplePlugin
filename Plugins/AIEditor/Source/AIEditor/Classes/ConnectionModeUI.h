// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Input/SButton.h"
#include "AINodeConnecter.h"
#include "boardingLineNodeModeUI.h"
#include "CoreMinimal.h"
#include "ManagerModeUI.h"

/**
 *
 */
class AIEDITOR_API ConnectionModeUI
{
public:
	ConnectionModeUI();
	~ConnectionModeUI();

	static TSharedPtr<STextBlock> fromNodeName;
	static TSharedPtr<STextBlock> toNodeName;
	static TSharedPtr<STextBlock> costName;
	static TSharedPtr<SGridPanel> nodeConnectionsPanel;

	static TSharedPtr<SButton>    deleteConnectionButton;
	static TSharedPtr<SButton>    goToOriginNodeButton;
	static TSharedPtr<SButton>    goToDestinationNodeButton;
	static AAINodeConnecter* currentConnection;

	static TSharedRef<SWidget> addConnectedNodesToUI(AActor* actor);
 	static FReply removeConnection();
	static FReply deleteConnectedNode(AActor* connectedActor);
	static void connectionsUIRegionGenerator();
 	static EVisibility connectionModeToggle();




};
