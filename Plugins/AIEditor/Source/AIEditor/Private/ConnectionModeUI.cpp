// Fill out your copyright notice in the Description page of Project Settings.

#include "ConnectionModeUI.h"

#include "NodeModeUI.h"
TSharedPtr<STextBlock> ConnectionModeUI::fromNodeName = nullptr;
TSharedPtr<STextBlock> ConnectionModeUI::toNodeName = nullptr;
TSharedPtr<STextBlock> ConnectionModeUI::costName = nullptr;
TSharedPtr<SGridPanel> ConnectionModeUI::nodeConnectionsPanel;

TSharedPtr<SButton>    ConnectionModeUI::deleteConnectionButton;
TSharedPtr<SButton>    ConnectionModeUI::goToOriginNodeButton;
TSharedPtr<SButton>    ConnectionModeUI::goToDestinationNodeButton;
AAINodeConnecter*  ConnectionModeUI::currentConnection;


ConnectionModeUI::ConnectionModeUI()
{

}

ConnectionModeUI::~ConnectionModeUI()
{

}

TSharedRef<SWidget> ConnectionModeUI::addConnectedNodesToUI(AActor * actor)
{
	return SNew(SGridPanel)
		.FillColumn(0, 0.0f)
		.FillColumn(1, 0.0f)
		.FillColumn(2, 0.0f)
		+ SGridPanel::Slot(0, 0).Padding(-50, 10)
		[

			SNew(STextBlock)
			.Text(ManagerModeUI::getActorName(actor))
		]

	+ SGridPanel::Slot(1, 0).Padding(50, 10)
		[
			SNew(SButton).OnClicked_Static(&ManagerModeUI::moveCameraToPosition, actor->GetActorLocation())
			[
				SNew(STextBlock)
				.Text(FText::FromString("Go to node"))
			]

		]

	+ SGridPanel::Slot(2, 0).Padding(50, 10)
		[
			SNew(SButton).OnClicked_Static(&deleteConnectedNode, actor)
			[
				SNew(STextBlock)
				.Text(FText::FromString("X"))
			]
		];

}



FReply ConnectionModeUI::removeConnection()
{
	GEditor->BeginTransaction(FText::FromString("destroy connection"));
	if (currentConnection)
	{
		currentConnection->RemoveConnection();
	}
	currentConnection = nullptr;
	boardingLineNodeModeUI::currentBoardingLineNode = nullptr;
	GEditor->SelectNone(true, true);

	GEditor->EndTransaction();
	return FReply::Handled();
}



FReply ConnectionModeUI::deleteConnectedNode(AActor * connectedActor)
{
	GEditor->BeginTransaction(FText::FromString("delete node"));
	if (NodeModeUI::currentNode && ManagerModeUI::wayPointMGG)
	{
		for (int i = 0; i < NodeModeUI::currentNode->connections.Num(); i++)
		{
			if (NodeModeUI::currentNode->connections[i]->toActor == connectedActor)
			{
				NodeModeUI::currentNode->connections[i]->Destroy();
				NodeModeUI::currentNode->connections.RemoveAt(i);
			}
		}

		ManagerModeUI::wayPointMGG->removeNode(StaticCast<AAIBaseNode*>(connectedActor));

		connectedActor->Destroy();
		NodeModeUI::currentNode->hasConnectionUI = false;

		//ManagerModeUI::wayPointMGG->Recalculate();
	}
	GEditor->EndTransaction();
	return FReply::Handled();
}

void ConnectionModeUI::connectionsUIRegionGenerator()
{
	ConnectionModeUI::nodeConnectionsPanel->ClearChildren();


	if (NodeModeUI::currentNode && NodeModeUI::currentNode->nearNodes.Num() > 0 && ConnectionModeUI::nodeConnectionsPanel)
	{
		ConnectionModeUI::nodeConnectionsPanel->AddSlot(0, 0)
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Bottom)
			.Padding(FMargin(5.0f))
			[
				SNew(STextBlock).Text(FText::FromString("Number of Connections: " + FString::FromInt(NodeModeUI::currentNode->nearNodes.Num())))
			];
		for (int i = 0; i < NodeModeUI::currentNode->nearNodes.Num(); i++)
		{

			ConnectionModeUI::nodeConnectionsPanel->AddSlot(0, i + 1)
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Bottom)
				.Padding(FMargin(5.0f))
				[
					addConnectedNodesToUI(NodeModeUI::currentNode->nearNodes[i])

				];

		}


		NodeModeUI::currentNode->hasConnectionUI = true;
	}




	NodeModeUI::nodeName->SetText(ManagerModeUI::getActorName(NodeModeUI::currentNode));

}

 

EVisibility ConnectionModeUI::connectionModeToggle()
{
	if (currentConnection)
	{
		if (NodeModeUI::currentNode)
		{
			NodeModeUI::currentNode->SetConnectionsColor(FVector(0, 1, 0));
		}
		NodeModeUI::currentNode = nullptr;
		ManagerModeUI::currentMode = ManagerModeUI::connectionMode;

		return EVisibility::Visible;


	}
	else
	{
		return EVisibility::Collapsed;
	}
}
