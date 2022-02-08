// Fill out your copyright notice in the Description page of Project Settings.

#include "boardingLineNodeUI.h"
#include "boardingLineNode.h"
#include "AIEditorEdMode.h"
#include "AIEditorEdMode.h"
#include "Editor.h"
#include "../Classes/AIEditorStyle.h"
#include "DrawDebugHelpers.h"
#include "UnrealEd/Public/LevelEditorViewport.h"

AEventObject* boardingLineNodeUI::EventMG;
TSharedPtr<SGridPanel> boardingLineNodeUI::boardingLinenodePanel;
bool boardingLineNodeUI::UIReady=false;
boardingLineNodeUI::boardingLineNodeUI()
{
}

boardingLineNodeUI::~boardingLineNodeUI()
{
	
}

EVisibility boardingLineNodeUI::boardingLineNodeModeToggle()
{

	CreateEventNodesList();
	return (EventMG && EventMG->FlagID=="2")? EVisibility::Visible:EVisibility::Collapsed;
}

void boardingLineNodeUI::CreateNewNodeAtPosition()
{
	UIReady = false;
	FViewport* f = GEditor->GetActiveViewport();
	FCanvas* fcz = f->GetDebugCanvas();
	FLevelEditorViewportClient* ViewportClient = (FLevelEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();

	FSceneViewport* fs = static_cast<FSceneViewport*>(f);

	check(fs);
	FGeometry geom = fs->GetCachedGeometry();
	FVector2D viewPortPos = fs->GetCachedGeometry().AbsolutePosition;

	FVector2D mpos = FSlateApplication::Get().GetCursorPos();

	FVector WorldPosition;
	FVector WorldDirection;
	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
		ViewportClient->Viewport,
		ViewportClient->GetScene(),
		ViewportClient->EngineShowFlags));


	FSceneView* View = ViewportClient->CalcSceneView(&ViewFamily);
	FIntRect fr = View->UnconstrainedViewRect;

	View->DeprojectFVector2D(mpos - viewPortPos, WorldPosition, WorldDirection);
	FVector endposition = WorldPosition + (WorldDirection * 10000);
	FVector startPos = View->ViewLocation;
	//	DrawDebugLine(ViewportClient->GetWorld(), startPos, endposition, FColor(1, 0, 0), true,99,0,50);

	if (mpos.X > viewPortPos.X && mpos.X < viewPortPos.X + fs->GetCachedGeometry().Size.X
		&& mpos.Y >viewPortPos.Y && mpos.Y < viewPortPos.Y + fs->GetCachedGeometry().Size.Y)
	{
		//DrawDebugLine(ViewportClient->GetWorld(), startPos, endposition, FColor(1, 0, 0), true, 99, 0, 50);
		//createNode(endposition, FVector(0), 1, ViewportClient);
		FHitResult OutHit;
		FCollisionQueryParams CollisionParams;
		if (ViewportClient->GetWorld()->LineTraceSingleByChannel(OutHit, startPos, endposition, ECollisionChannel::ECC_WorldStatic, CollisionParams))
		{
			if (OutHit.bBlockingHit)
			{
				createBoardingLineNodeObject(EventMG, OutHit.ImpactPoint, OutHit.ImpactPoint,0, ViewportClient->GetWorld());




			}
		}

		//create

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("out of range"));
	}


}
 
void boardingLineNodeUI::CreateNewUnboardingNodeAtPosition()
{
	UIReady = false;
	FViewport* f = GEditor->GetActiveViewport();
	FCanvas* fcz = f->GetDebugCanvas();
	FLevelEditorViewportClient* ViewportClient = (FLevelEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();

	FSceneViewport* fs = static_cast<FSceneViewport*>(f);

	check(fs);
	FGeometry geom = fs->GetCachedGeometry();
	FVector2D viewPortPos = fs->GetCachedGeometry().AbsolutePosition;

	FVector2D mpos = FSlateApplication::Get().GetCursorPos();

	FVector WorldPosition;
	FVector WorldDirection;
	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
		ViewportClient->Viewport,
		ViewportClient->GetScene(),
		ViewportClient->EngineShowFlags));


	FSceneView* View = ViewportClient->CalcSceneView(&ViewFamily);
	FIntRect fr = View->UnconstrainedViewRect;

	View->DeprojectFVector2D(mpos - viewPortPos, WorldPosition, WorldDirection);
	FVector endposition = WorldPosition + (WorldDirection * 10000);
	FVector startPos = View->ViewLocation;
	//	DrawDebugLine(ViewportClient->GetWorld(), startPos, endposition, FColor(1, 0, 0), true,99,0,50);

	if (mpos.X > viewPortPos.X && mpos.X < viewPortPos.X + fs->GetCachedGeometry().Size.X
		&& mpos.Y >viewPortPos.Y && mpos.Y < viewPortPos.Y + fs->GetCachedGeometry().Size.Y)
	{
		//DrawDebugLine(ViewportClient->GetWorld(), startPos, endposition, FColor(1, 0, 0), true, 99, 0, 50);
		//createNode(endposition, FVector(0), 1, ViewportClient);
		FHitResult OutHit;
		FCollisionQueryParams CollisionParams;
		if (ViewportClient->GetWorld()->LineTraceSingleByChannel(OutHit, startPos, endposition, ECollisionChannel::ECC_WorldStatic, CollisionParams))
		{
			if (OutHit.bBlockingHit)
			{
				createBoardingLineNodeObject(EventMG, OutHit.ImpactPoint, OutHit.ImpactPoint, 0, ViewportClient->GetWorld(),true);




			}
		}

		//create

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("out of range"));
	}

}

//FIX PARAMETERS
void boardingLineNodeUI::createBoardingLineNodeObject(AActor * eventParent, FVector WorldPosition, FVector direction, float angle, UWorld * world, bool isUnboarding)
{
	FVector objectPosition(WorldPosition + FVector(0, 0, 3));
	FRotator objectRotation(0, 0, 0); //in degrees
	FVector objectScale(1, 1, 1);
	FTransform objectTrasform(objectRotation, objectPosition, objectScale);
	UWorld* c = world;
	ULevel* level = c->GetCurrentLevel();


	AboardingLineNode* newBoardingNode =StaticCast<AboardingLineNode*>(
		GEditor->AddActor(level,  AboardingLineNode::StaticClass(), objectTrasform, true, RF_Public | RF_Transactional));




	EventMG = StaticCast<AEventObject*>(eventParent);
	 
 	if (newBoardingNode != NULL)
	{
		newBoardingNode->location = objectPosition;
		newBoardingNode->direction = direction;
		 	
		newBoardingNode->Angle = angle;
		newBoardingNode->isUnboardingNode = isUnboarding;
		newBoardingNode->UpdateMat();
		if (EventMG)
		{
			if (isUnboarding)
			{
				EventMG->unBoardingLinePositions.Add(newBoardingNode);

				newBoardingNode->SetActorLabel("unboarding position" + FString::FromInt(EventMG->unBoardingLinePositions.Num()));
			}
			else
			{
				EventMG->boardingLinePositions.Add(newBoardingNode);
				newBoardingNode->SetActorLabel("boarding position" + FString::FromInt(EventMG->boardingLinePositions.Num()));
			}
		

			
			newBoardingNode->eventParent = EventMG;
		}
	//	newBoardingLineNode->FlagID = FString::FromInt(0);
	}
 	newBoardingNode->RerunConstructionScripts();
	//newBoardingLineNode->parentEvent = static_cast<AEventObject*>(eventParent);
	//EventMG = static_cast<AEventObject*>(eventParent);;
 	GLevelEditorModeTools().MapChangeNotify();
	if (EventMG)
	GEditor->SelectActor(EventMG, true, true);
}
void boardingLineNodeUI::UpdateLines(UWorld* world)
{
	if (EventMG)
	{
		for (int i = 0; i < EventMG->boardingLinePositions.Num(); i++)
		{
			if (i == 0)
			{
				DrawDebugLine(world, EventMG->GetActorLocation(), EventMG->boardingLinePositions[0]->GetActorLocation(), FColor::Cyan, true, 99.0f);
			}
			else
			{
				DrawDebugLine(world, EventMG->boardingLinePositions[i - 1]->GetActorLocation(), EventMG->boardingLinePositions[i]->GetActorLocation(), FColor::Cyan, true, 99.0f);
			}
		}
		for (int i = 0; i < EventMG->unBoardingLinePositions.Num(); i++)
		{
			if (i == 0)
			{
				DrawDebugLine(world, EventMG->GetActorLocation(), EventMG->unBoardingLinePositions[0]->GetActorLocation(), FColor::Emerald, true, 99.0f);
			}
			else
			{
				DrawDebugLine(world, EventMG->unBoardingLinePositions[i - 1]->GetActorLocation(), EventMG->unBoardingLinePositions[i]->GetActorLocation(), FColor::Emerald, true, 99.0f);
			}
		}
	}
}
void boardingLineNodeUI::CreateEventNodesList()
{ 
	
	if (EventMG && EventMG->FlagID== "2" && boardingLinenodePanel && !UIReady)
	{
		 
		 
			EventNodesUIRegionGenerator();
			UIReady = true;
		 
	}
	
}

void boardingLineNodeUI::EventNodesUIRegionGenerator()
{ 
	//if (boardingLinenodePanel )
	{
		int cSlot = 0;
		FViewport* f = GEditor->GetActiveViewport();

		FCanvas* fcz = f->GetDebugCanvas();
		FLevelEditorViewportClient* ViewportClient = (FLevelEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();
		UWorld* world = ViewportClient->GetWorld();
		boardingLinenodePanel->ClearChildren();
		FlushPersistentDebugLines(world);
		boardingLinenodePanel->AddSlot(0, 0)
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Bottom)
			.Padding(-250, 0)
			[
				SNew(STextBlock).Text(FText::FromString("Number of boarding Positions: " + FString::FromInt(EventMG->boardingLinePositions.Num())))
			];
		boardingLinenodePanel->AddSlot(0, 1)
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Bottom)
			.Padding(-250, 0)
			[
				SNew(SButton)
				.Text(FText::FromString("Create New Boarding line Position")).OnReleased_Static(&CreateNewNodeAtPosition)
			.Content()
			[
				SNew(SImage).Image(AIEditorStyle::Get()->GetBrush("boardingLineNode.icon"))
			]

			];
				
				
			for (int i = 0; i < EventMG->boardingLinePositions.Num(); i++)
			{
				
				UpdateLines(world);
				boardingLinenodePanel->AddSlot(0, i + 2)
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Bottom)
					.Padding(-250, 0)
					[

						SNew(STextBlock)
						.Text(FText::FromString("Position "+ FString::FromInt(i)))
						//addEventNodesToUI(EventMG->boardingLinePositions[i])

					];
				boardingLinenodePanel->AddSlot(1, i+2)
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Bottom)
					.Padding(-50, 0)
					[
						SNew(SButton).OnClicked_Static(&DeleteEventNode,i)
						[
						SNew(STextBlock)
						.Text(FText::FromString("X"))
						]
					//addEventNodesToUI(EventMG->boardingLinePositions[i])

					];
				cSlot = i + 2;

			}
			cSlot++;
			//////////////////////
			
			boardingLinenodePanel->AddSlot(0, cSlot+1)
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Bottom)
				.Padding(-250, 0)
				[
					SNew(STextBlock).Text(FText::FromString("Number of boarding Positions: " + FString::FromInt(EventMG->boardingLinePositions.Num())))
				];
			boardingLinenodePanel->AddSlot(0, cSlot + 2)
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Bottom)
				.Padding(-250, 0)
				[
					SNew(SButton)
					.Text(FText::FromString("Create New Boarding line Position")).OnReleased_Static(&CreateNewUnboardingNodeAtPosition)
				.Content()
				[
					SNew(SImage).Image(AIEditorStyle::Get()->GetBrush("unboardingLineNode.icon"))
				]

				];
			cSlot++;
			for (int i = 0; i < EventMG->unBoardingLinePositions.Num(); i++)
			{

				UpdateLines(world);
				boardingLinenodePanel->AddSlot(0, cSlot +i + 2)
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Bottom)
					.Padding(-250, 0)
					[

						SNew(STextBlock)
						.Text(FText::FromString("UPosition " + FString::FromInt(i)))
					//addEventNodesToUI(EventMG->boardingLinePositions[i])

					];
				boardingLinenodePanel->AddSlot(1, cSlot + i + 2)
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Bottom)
					.Padding(-50, 0)
					[
						SNew(SButton).OnClicked_Static(&DeleteUnboardingEventNode, i)
						[
							SNew(STextBlock)
							.Text(FText::FromString("X"))
						]
				//addEventNodesToUI(EventMG->boardingLinePositions[i])

					];
				

			}




			
		
	}
	 
}

FReply boardingLineNodeUI::DeleteEventNode(int num)
{
	GEditor->BeginTransaction(FText::FromString("delete boarding line node"));
	
	if (num>=0 && EventMG && EventMG->boardingLinePositions.Num()>num)
	{
		AboardingLineNode* BLM = EventMG->boardingLinePositions[num];
		if (BLM)
		{
			EventMG->boardingLinePositions.Remove(BLM);
			BLM->Destroy();
			UIReady = false;
		}
	}
	
	GEditor->SelectActor(EventMG, true,true);
	GEditor->EndTransaction();

	return FReply::Handled();
}

FReply boardingLineNodeUI::DeleteUnboardingEventNode(int num)
{
	GEditor->BeginTransaction(FText::FromString("delete unboarding line node"));

	if (num >= 0 && EventMG && EventMG->unBoardingLinePositions.Num() > num)
	{
		AboardingLineNode* BLM = EventMG->unBoardingLinePositions[num];
		if (BLM)
		{
			EventMG->unBoardingLinePositions.Remove(BLM);
			BLM->Destroy();
			UIReady = false;
		}
	}

	GEditor->SelectActor(EventMG, true, true);
	GEditor->EndTransaction();

	return FReply::Handled();
}

/*TSharedRef<SWidget> boardingLineNodeUI::addEventNodesToUI(AActor * actor)
{
	return TSharedRef<SWidget>();
}*/
