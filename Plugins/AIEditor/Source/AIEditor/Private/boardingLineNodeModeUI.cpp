// Fill out your copyright notice in the Description page of Project Settings.

#include "boardingLineNodeModeUI.h"


TSharedPtr<SSlider>						  boardingLineNodeModeUI::BoardingLineRadiusSlider;
TSharedPtr<STextBlock>					  boardingLineNodeModeUI::BoardingLineRadiusLabel;
AboardingLineNode*							boardingLineNodeModeUI::currentBoardingLineNode;

boardingLineNodeModeUI::boardingLineNodeModeUI()
{
}

boardingLineNodeModeUI::~boardingLineNodeModeUI()
{
}

EVisibility boardingLineNodeModeUI::boardingLineNodeModeToggle()
{
	if (boardingLineNodeModeUI::currentBoardingLineNode)
	{
		if (boardingLineNodeModeUI::currentBoardingLineNode->Moved)
		{
			FlushPersistentDebugLines(ManagerModeUI::getLocalWorld());
			boardingLineNodeModeUI::currentBoardingLineNode->Moved = false;
			boardingLineNodeUI::UpdateLines(ManagerModeUI::getLocalWorld());
		}
		if (boardingLineNodeModeUI::currentBoardingLineNode->eventParent)
		{
			for (int i = 0; i < currentBoardingLineNode->eventParent->boardingLinePositions.Num(); i++)
			{
				DrawDirectionLine(currentBoardingLineNode->eventParent->boardingLinePositions[i]);
			}
			for (int i = 0; i < currentBoardingLineNode->eventParent->unBoardingLinePositions.Num(); i++)
			{
				DrawDirectionLine(currentBoardingLineNode->eventParent->unBoardingLinePositions[i]);
			}
		}
		else
		{
			DrawDirectionLine();
		}
		boardingLineNodeModeUI::BoardingLineRadiusLabel->SetText(FText::FromString("Angle : " +
			(FString::FromInt(boardingLineNodeModeUI::currentBoardingLineNode->Angle))));
		boardingLineNodeModeUI::BoardingLineRadiusSlider->SetValue(boardingLineNodeModeUI::currentBoardingLineNode->Angle / 360);

		ManagerModeUI::currentMode = ManagerModeUI::boardingNodeMode;
		return EVisibility::Visible;
	}
	else
	{
		return EVisibility::Collapsed;
	}
}

void boardingLineNodeModeUI::DrawDirectionLine(AboardingLineNode * boardingNode)
{
	AboardingLineNode* localBLnode = (boardingNode) ? boardingNode : boardingLineNodeModeUI::currentBoardingLineNode;
	if (localBLnode)
	{

		FVector startPos = localBLnode->GetActorLocation();
		FVector endpos = startPos +
			(localBLnode->GetActorForwardVector().
				RotateAngleAxis(localBLnode->Angle,
					localBLnode->GetActorUpVector()))
			* 50;
		localBLnode->direction = endpos;
		DrawDebugLine(ManagerModeUI::getLocalWorld(), startPos, endpos, FColor::Purple, true, 99.0f, 0, 3.0);

	}
}
 
FReply boardingLineNodeModeUI::DeleteboardingLineNode()
{
	GEditor->BeginTransaction(FText::FromString("deleate current boarding mode"));

	if (boardingLineNodeModeUI::currentBoardingLineNode && ManagerModeUI::wayPointMGG)
	{
		for (int i = 0; i < ManagerModeUI::wayPointMGG->Events.Num(); i++)
		{
			if (ManagerModeUI::wayPointMGG->Events[i]->FlagID == "2")
			{
				if (currentBoardingLineNode->isUnboardingNode)
				{
					if (ManagerModeUI::wayPointMGG->Events[i]->unBoardingLinePositions.Contains(boardingLineNodeModeUI::currentBoardingLineNode))
					{
						ManagerModeUI::wayPointMGG->Events[i]->unBoardingLinePositions.Remove(boardingLineNodeModeUI::currentBoardingLineNode);
					}
				}

				else
				{

					if (ManagerModeUI::wayPointMGG->Events[i]->boardingLinePositions.Contains(boardingLineNodeModeUI::currentBoardingLineNode))
					{
						ManagerModeUI::wayPointMGG->Events[i]->boardingLinePositions.Remove(boardingLineNodeModeUI::currentBoardingLineNode);
					}


				}
			}
		}
		
		FlushPersistentDebugLines(ManagerModeUI::getLocalWorld());

		boardingLineNodeUI::UpdateLines(ManagerModeUI::getLocalWorld());
		boardingLineNodeModeUI::currentBoardingLineNode->Destroy();
	}

	exitBoardingMode();
	GEditor->EndTransaction();
	return FReply::Handled();
}

FReply boardingLineNodeModeUI::exitBoardingMode()
{
	GEditor->BeginTransaction(FText::FromString("exit boarding mode"));
	ManagerModeUI::currentMode = ManagerModeUI::managerMode;
	GEditor->SelectNone(true, true);
	if (boardingLineNodeModeUI::currentBoardingLineNode->eventParent)
	{
		GEditor->SelectActor(boardingLineNodeModeUI::currentBoardingLineNode->eventParent, true, true);
		EventNodeUI::currentEventNode = boardingLineNodeModeUI::currentBoardingLineNode->eventParent;
		ManagerModeUI::currentMode = ManagerModeUI::eventNodeMode;
	}

	boardingLineNodeModeUI::currentBoardingLineNode = nullptr;



	GEditor->EndTransaction();
	return FReply::Handled();
}

void boardingLineNodeModeUI::updateAngle(float NewValue)
{
	if (boardingLineNodeModeUI::currentBoardingLineNode)
	{
		boardingLineNodeModeUI::currentBoardingLineNode->Angle = NewValue * 360;
		boardingLineNodeModeUI::BoardingLineRadiusLabel->SetText(FText::FromString("Angle : " + (FString::FromInt(NewValue * 360))));
		FlushPersistentDebugLines(ManagerModeUI::getLocalWorld());
		boardingLineNodeUI::UpdateLines(ManagerModeUI::getLocalWorld());
		boardingLineNodeModeUI::DrawDirectionLine();


	}
}

TSharedRef<SWidget> boardingLineNodeModeUI::makeBoardingLineNodeModeUI()
{
	return 	SNew(SScrollBox).Visibility_Static(&boardingLineNodeModeToggle)


		+ SScrollBox::Slot()
		.Padding(5.0f)
		[


			SNew(SGridPanel)
			.FillColumn(0, 0.25f)
		.FillColumn(1, 0.25f)
		.FillColumn(2, 0.25f)
		.FillColumn(3, 0.25f)
		+ SGridPanel::Slot(2, 0).Padding(0, 10)
		[
			SNew(SButton).OnClicked_Static(&exitBoardingMode)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Exit Mode"))
			]
		]
	+ SGridPanel::Slot(2, 1).Padding(0, 10)
		[
			SNew(SButton).OnClicked_Static(&DeleteboardingLineNode)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Delete"))

			]
		]
	+ SGridPanel::Slot(0, 2).Padding(0, 10)
		[
			SAssignNew(BoardingLineRadiusLabel, STextBlock)
			.Text(FText::FromString("Angle"))
		]
	+ SGridPanel::Slot(1, 2).Padding(0, 10)
		[
			SAssignNew(BoardingLineRadiusSlider, SSlider).OnValueChanged_Static(&updateAngle).Value(0)

		]

		];
}
