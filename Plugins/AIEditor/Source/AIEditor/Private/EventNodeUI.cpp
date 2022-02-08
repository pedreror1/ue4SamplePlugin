// Fill out your copyright notice in the Description page of Project Settings.

#include "EventNodeUI.h"

AEventObject* EventNodeUI::currentEventNode  ;
TSharedPtr<SSlider> EventNodeUI::EVRadiusSlider  ;
TSharedPtr<STextBlock> EventNodeUI::EVNodeIDData  ;
TSharedPtr<STextBlock>	EventNodeUI::EVRadiusLabel  ;
TSharedPtr<STextBlock>EventNodeUI::EVflagData  ;
TSharedPtr<SEditableTextBox> EventNodeUI::EVxPos  ;
TSharedPtr<SEditableTextBox> EventNodeUI::EVyPos  ;
TSharedPtr<SEditableTextBox> EventNodeUI::EVzPos  ;
TSharedPtr<SComboBox<TSharedPtr<FString>>>  EventNodeUI::EVNodeIDComboBox;
TSharedPtr<SComboBox<TSharedPtr<FString>>>  EventNodeUI::EVflagComboBox;
int EventNodeUI::selectedNodeID  ;
TSharedPtr<FString>						  EventNodeUI::EVSelectedNodeIDItem;
TSharedPtr<FString>						 EventNodeUI::EVSelectedFlagItem;
TSharedPtr<SGridPanel> EventNodeUI::EventsnodePanel;
EventNodeUI::EventNodeUI()
{

}

EventNodeUI::~EventNodeUI()
{

}

TSharedRef<SWidget> EventNodeUI::addEventNodesToUI(AActor * actor)
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
				.Text(FText::FromString("Go to Event"))
			]

		]

	+ SGridPanel::Slot(2, 0).Padding(10, 10)
		[
			SNew(SButton).OnClicked_Static(&DeleteEventNode, actor)
			[
				SNew(STextBlock)
				.Text(FText::FromString("X"))
			]
		];
}

FReply EventNodeUI::DeleteEventNode(AActor * actor)
{
	GEditor->BeginTransaction(FText::FromString("delete event node"));

	if (ManagerModeUI::wayPointMGG)
	{
		ManagerModeUI::wayPointMGG->hasEventUI = false;
		if (ManagerModeUI::wayPointMGG->Events.Contains(actor))
		{
			ManagerModeUI::wayPointMGG->Events.Remove(StaticCast<AEventObject*>(actor));
		}
		ManagerModeUI::wayPointMGG->hasEventUI = false;
		//CreateEventNodesList();
		actor->Destroy();

	}
	else
	{
		FLevelEditorViewportClient* ViewportClient = (FLevelEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();
		UWorld* myWorld = ViewportClient->GetWorld();
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(myWorld, AIEditorManager::StaticClass(), FoundActors);
		if (FoundActors.Num() == 1)
		{
			ManagerModeUI::wayPointMGG = static_cast<AIEditorManager*>(FoundActors[0]);


			ManagerModeUI::wayPointMGG->RerunConstructionScripts();
			if (ManagerModeUI::wayPointMGG)
			{

				if (ManagerModeUI::wayPointMGG->Events.Contains(EventNodeUI::currentEventNode))
				{
					ManagerModeUI::wayPointMGG->Events.Remove(EventNodeUI::currentEventNode);

				}
				ManagerModeUI::wayPointMGG->hasEventUI = false;
			//	CreateEventNodesList();
				actor->Destroy();

			}
		}
	}
	GEditor->SelectNone(true, true);
	GEditor->EndTransaction();
	return FReply::Handled();
}

FReply EventNodeUI::DeleteCurrentEventNode()
{
	GEditor->BeginTransaction(FText::FromString("delete event node"));
	GEditor->SelectNone(true, true);
	FlushPersistentDebugLines(ManagerModeUI::getLocalWorld());

	if (ManagerModeUI::wayPointMGG)
	{
		if (ManagerModeUI::wayPointMGG->Events.Contains(EventNodeUI::currentEventNode))
		{
			ManagerModeUI::wayPointMGG->Events.Remove(EventNodeUI::currentEventNode);
		}

		EventNodeUI::currentEventNode->Destroy();
		EventNodeUI::currentEventNode = nullptr;
		boardingLineNodeUI::UIReady = false;

		boardingLineNodeUI::EventMG = static_cast<AEventObject*>(EventNodeUI::currentEventNode);

	}
	else
	{
		FLevelEditorViewportClient* ViewportClient = (FLevelEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();
		UWorld* myWorld = ViewportClient->GetWorld();
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(myWorld, AIEditorManager::StaticClass(), FoundActors);
		if (FoundActors.Num() == 1)
		{
			ManagerModeUI::wayPointMGG = static_cast<AIEditorManager*>(FoundActors[0]);


			ManagerModeUI::wayPointMGG->RerunConstructionScripts();
			if (ManagerModeUI::wayPointMGG)
			{
				if (ManagerModeUI::wayPointMGG->Events.Contains(EventNodeUI::currentEventNode))
				{
					ManagerModeUI::wayPointMGG->Events.Remove(EventNodeUI::currentEventNode);
				}
				EventNodeUI::currentEventNode->Destroy();
				EventNodeUI::currentEventNode = nullptr;
				boardingLineNodeUI::UIReady = false;

				boardingLineNodeUI::EventMG = static_cast<AEventObject*>(EventNodeUI::currentEventNode);

			}
		}
	}

	GEditor->EndTransaction();
	return FReply::Handled();
}

FReply EventNodeUI::exitEventNodeMode()
{
	GEditor->BeginTransaction(FText::FromString("exit event node mode"));
	GEditor->SelectNone(true, true);
	ManagerModeUI::currentMode = ManagerModeUI::eventMode;
	FlushPersistentDebugLines(ManagerModeUI::getLocalWorld());

	EventNodeUI::currentEventNode = nullptr;
	boardingLineNodeUI::UIReady = false;

	boardingLineNodeUI::EventMG = static_cast<AEventObject*>(EventNodeUI::currentEventNode);

	GEditor->EndTransaction();
	return FReply::Handled();
}

void EventNodeUI::EventNodesUIRegionGenerator()
{
	if (EventsnodePanel)
	{
		EventsnodePanel->ClearChildren();


		if (ManagerModeUI::wayPointMGG && ManagerModeUI::wayPointMGG->Events.Num() > 0 && EventsnodePanel)
		{
			EventsnodePanel->AddSlot(0, 0)
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Bottom)
				.Padding(-50, 0)
				[
					SNew(STextBlock).Text(FText::FromString("Number of Event Triggers: " + FString::FromInt(ManagerModeUI::wayPointMGG->Events.Num())))
				];
			for (int i = 0; i < ManagerModeUI::wayPointMGG->Events.Num(); i++)
			{

				EventsnodePanel->AddSlot(0, i + 1)
					.HAlign(HAlign_Right)
					.VAlign(VAlign_Bottom)
					.Padding(FMargin(5.0f))
					[


						addEventNodesToUI(ManagerModeUI::wayPointMGG->Events[i])

					];

			}


			ManagerModeUI::wayPointMGG->hasEventUI = true;
		}

	}
}

void EventNodeUI::CreateEventNodesList()
{
	if (ManagerModeUI::wayPointMGG && !ManagerModeUI::wayPointMGG->hasEventUI)
	{
		EventNodesUIRegionGenerator();
	}
}

EVisibility EventNodeUI::EventNodeModeToggle()
{
	if (ManagerModeUI::currentMode == ManagerModeUI::eventNodeMode || ManagerModeUI::currentMode == ManagerModeUI::selectNodeIdMode)
	{
		if (EventNodeUI::currentEventNode)
		{
			FlushPersistentDebugLines(ManagerModeUI::getLocalWorld());
			boardingLineNodeUI::UpdateLines(ManagerModeUI::getLocalWorld());
			for (int i = 0; i < EventNodeUI::currentEventNode->boardingLinePositions.Num(); i++)
			{

				boardingLineNodeModeUI::DrawDirectionLine(EventNodeUI::currentEventNode->boardingLinePositions[i]);
			}
		}
		return EVisibility::Visible;
	}
	else
	{
		//FlushPersistentDebugLines(getLocalWorld());

		return EVisibility::Collapsed;
	}
}
  

