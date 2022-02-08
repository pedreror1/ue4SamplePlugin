// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "AIEditorEdModeToolkit.h"
#include "AIEditorEdMode.h"
#include "EditorModeManager.h"
#include "../Classes/AIEditorStyle.h"
#include "Engine.h"
#include "Engine/Selection.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/Application/SlateApplication.h"
#include "slate/SceneViewport.h"
#include "UnrealEd/Public/LevelEditorViewport.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "../Classes/AIBaseNode.h"
#include "../Classes/EventObject.h"
#include "../Classes/NodeModeUI.h"
#include "../Classes/EventNodeUI.h"
#include "../Classes/ConnectionModeUI.h"
#include "../Classes/ManagerModeUI.h"
#include "../Classes/boardingLineNode.h"
#include "../Classes/boardingLineNodeModeUI.h"
#include "boardingLineNodeUI.h"
#include <iostream>
#include <fstream>
#include <iosfwd>
#include "Core/Public/HAL/PlatformFilemanager.h"

#define LOCTEXT_NAMESPACE "FAIEditorEdModeToolkit"

FAIEditorEdModeToolkit::FAIEditorEdModeToolkit()
{
}
TArray<TSharedPtr <FString>> DirectionOptions = { MakeShareable<FString>(new FString("Up")), MakeShareable<FString>(new FString("Dw")), MakeShareable<FString>(new FString("NONE")) };
TArray<TSharedPtr <FString>> FlagIDList = { MakeShareable<FString>(new FString("0")) };

void FAIEditorEdModeToolkit::PreCreateManager(AIEditorManager* &wayPointMGG)
{
	FLevelEditorViewportClient* ViewportClient = (FLevelEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();
	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
		ViewportClient->Viewport,
		ViewportClient->GetScene(),
		ViewportClient->EngineShowFlags));

	UWorld* myWorld = ViewportClient->GetWorld();
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(myWorld, AIEditorManager::StaticClass(), FoundActors);

	if (FoundActors.Num() == 0)
	{

		AActor* actor = GEditor->AddActor(myWorld->GetCurrentLevel(),
			AIEditorManager::StaticClass(),
			FTransform(), true, RF_Public | RF_Transactional);

		wayPointMGG = static_cast<AIEditorManager*>(actor);


	}

	else if (FoundActors.Num() == 1)
	{


		wayPointMGG = static_cast<AIEditorManager*>(FoundActors[0]);
	}
}
 	 
void FAIEditorEdModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost)
{ 
#pragma region Variables




	ManagerModeUI::currentDirection = DirectionOptions[2];
	 EventNodeUI::EVSelectedNodeIDItem = NodeModeUI::nodesIDList[0];
	 EventNodeUI::EVSelectedFlagItem = FlagIDList[0];
	static UWorld* worldGlobal;
	//static AAIBaseNode* currentNode;
	

	//static AAINodeConnecter* currentConnection;
	
	//NODE

	



	static int numOfEventObject = 0;
	//static bool isInAddMode= false;

	static FVector lastPos = FVector();
	static FString levelName="BenchMap";
	

	for (int i = 1; i < 4; i++)
	{
		FString x = FString::FromInt(i);
		FlagIDList.Add(MakeShareable<FString>(new FString(x)));
	}
	PreCreateManager(ManagerModeUI::wayPointMGG);

#pragma endregion
	 
#pragma region  UISetupFunctions


	struct Locals
	{
		//Manager
		static EVisibility managerModeToggle()
		{
			FLevelEditorViewportClient* ViewportClient = (FLevelEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();

			UWorld* myWorld = ViewportClient->GetWorld();
			
			if (myWorld->WorldType != EWorldType::Editor)
			{
				 
				return EVisibility::Collapsed;

			}
			if (ManagerModeUI::currentMode == ManagerModeUI::eventMode )
			{
				if (GEditor->GetSelectedActors()->Num() > 0)
				{
					USelection* SelectedActors = GEditor->GetSelectedActors();
					TArray<AActor*> Actors;
					TArray<ULevel*> UniqueLevels;
					for (FSelectionIterator Iter(*SelectedActors); Iter; ++Iter)
					{
						AActor* Actor = Cast<AActor>(*Iter);	
						lastPos = Actor->GetActorLocation();
						if (Actor->GetClass()->IsChildOf(AEventObject::StaticClass()))
						{
							EventNodeUI::currentEventNode = static_cast<AEventObject*>(Actor);
							boardingLineNodeUI::EventMG = static_cast<AEventObject*>(EventNodeUI::currentEventNode);
							EventNodeUI::EVRadiusSlider->SetValue((EventNodeUI::currentEventNode->Radius*0.1) / EventNodeUI::currentEventNode->MaxRadius);
							NodeModeUI::currentAddNode = nullptr;
							if (NodeModeUI::currentNode)
							{
								NodeModeUI::currentNode->SetConnectionsColor(FVector(0, 1, 0));
							}
							NodeModeUI::currentNode = nullptr;
							ConnectionModeUI::currentConnection = nullptr;
							boardingLineNodeModeUI::currentBoardingLineNode = nullptr;
 							NodeModeUI::isInAddMode = false;
							ManagerModeUI::currentMode = ManagerModeUI::eventNodeMode;
						}
					}
				}
				return EVisibility::Collapsed;
			}

			if (ManagerModeUI::currentMode == ManagerModeUI::selectNodeIdMode)
			{
				if (GEditor->GetSelectedActors()->Num() > 0)
				{
  					for (FSelectionIterator Iter(*GEditor->GetSelectedActors()); Iter; ++Iter)
					{
						AActor* Actor = Cast<AActor>(*Iter);
						lastPos = Actor->GetActorLocation();

						if (Actor->GetClass()->IsChildOf(AAIBaseNode::StaticClass()))
						{
							EventNodeUI::selectedNodeID = ManagerModeUI::wayPointMGG->nodes.IndexOfByKey(static_cast<AAIBaseNode*>(Actor));
							if (EventNodeUI::currentEventNode)
							{
								EventNodeUI::EVNodeIDData->SetText(FString::FromInt(EventNodeUI::selectedNodeID));
								EventNodeUI::currentEventNode->nodeID = FString::FromInt(EventNodeUI::selectedNodeID);
							}
							GEditor->SelectNone(true,true);
							GEditor->SelectActor(EventNodeUI::currentEventNode, true, true, true, true);

							ManagerModeUI::currentMode = ManagerModeUI::eventNodeMode;

						}
					}
				}
				return EVisibility::Collapsed;
			}

			if (GEditor->GetSelectedActors()->Num() > 0 && ManagerModeUI::wayPointMGG)
			{

				USelection* SelectedActors = GEditor->GetSelectedActors();
				TArray<AActor*> Actors;
				TArray<ULevel*> UniqueLevels;
				for (FSelectionIterator Iter(*SelectedActors); Iter; ++Iter)
				{
					AActor* Actor = Cast<AActor>(*Iter);
					lastPos = Actor->GetActorLocation();

					if (Actor->GetClass()->IsChildOf(AAIBaseNode::StaticClass()))
					{
						if (!NodeModeUI::isInAddMode || !NodeModeUI::currentNode)
						{
							//set old selected node connections color to green
							if (NodeModeUI::currentNode)
							{
								NodeModeUI::currentNode->SetConnectionsColor(FVector(0, 1, 0));
							}
							if (EventNodeUI::currentEventNode)
							{
								EventNodeUI::currentEventNode = nullptr;
								boardingLineNodeUI::UIReady = false;
								boardingLineNodeUI::EventMG = static_cast<AEventObject*>(EventNodeUI::currentEventNode);

								ManagerModeUI::currentMode = ManagerModeUI::nodeMode;
							}
							NodeModeUI::currentNode = static_cast<AAIBaseNode*>(Actor);
							if (NodeModeUI::currentNode)
							{
								NodeModeUI::currentNode->SetConnectionsColor(FVector(1, 0, 0));
								EventNodeUI::selectedNodeID = ManagerModeUI::wayPointMGG->nodes.IndexOfByKey(NodeModeUI::currentNode);

							}
							ConnectionModeUI::currentConnection = nullptr;
							boardingLineNodeModeUI::currentBoardingLineNode = nullptr;
						}
						else
						{
							if (Actor != NodeModeUI::currentNode)
							{
								NodeModeUI::currentAddNode = static_cast<AAIBaseNode*>(Actor);
								if (NodeModeUI::currentNode)
								{
									NodeModeUI::currentNode->SetConnectionsColor(FVector(1, 0, 0));
									if (NodeModeUI::currentAddNode != NodeModeUI::currentNode && NodeModeUI::currentAddNode && NodeModeUI::currentNode && NodeModeUI::isInAddMode)
									{
										NodeModeUI::currentNode->AddObjectToConnections(NodeModeUI::currentAddNode);
										GEditor->SelectNone(true, true);
										GEditor->SelectActor(NodeModeUI::currentNode, true, true);
									}

								}
								if (EventNodeUI::currentEventNode)
								{
									EventNodeUI::currentEventNode = nullptr;
									boardingLineNodeUI::UIReady = false;

									boardingLineNodeUI::EventMG = static_cast<AEventObject*>(EventNodeUI::currentEventNode);

									ManagerModeUI::currentMode = ManagerModeUI::addMode;
								}
							}

						}
						return EVisibility::Collapsed;

						
					}
					else if (Actor->GetClass()->IsChildOf(AAINodeConnecter::StaticClass()))
					{
						if (NodeModeUI::currentNode)
						{
							NodeModeUI::currentNode->SetConnectionsColor(FVector(0, 1, 0));
						}
						if (boardingLineNodeModeUI::currentBoardingLineNode)
						{
							boardingLineNodeModeUI::currentBoardingLineNode = nullptr;
						}
						if (EventNodeUI::currentEventNode)
						{
							EventNodeUI::currentEventNode = nullptr;
							boardingLineNodeUI::UIReady = false;

							boardingLineNodeUI::EventMG = static_cast<AEventObject*>(EventNodeUI::currentEventNode);

							ManagerModeUI::currentMode = ManagerModeUI::connectionMode;
						}
						if (!ConnectionModeUI::currentConnection)
						{
							ConnectionModeUI::currentConnection = static_cast<AAINodeConnecter*>(Actor);
							if (NodeModeUI::currentNode)
							{
								NodeModeUI::currentNode->SetConnectionsColor(FVector(0, 1, 0));
							}
							NodeModeUI::currentNode = nullptr;
						}
						else
						{
							if (Actor != ConnectionModeUI::currentConnection)
							{
								ConnectionModeUI::currentConnection = static_cast<AAINodeConnecter*>(Actor);
							}
							if (ConnectionModeUI::currentConnection->fromActor && ConnectionModeUI::currentConnection->toActor)
							{
								ConnectionModeUI::fromNodeName->SetText(ConnectionModeUI::currentConnection->getFromString());
								ConnectionModeUI::toNodeName->SetText(ConnectionModeUI::currentConnection->getToString());
								ConnectionModeUI::costName->SetText(ConnectionModeUI::currentConnection->getCostString());

							}
						}
						return EVisibility::Collapsed;

						//UniqueLevels.AddUnique(Actor->GetLevel());
					}
					else if (Actor->GetClass()->IsChildOf(AEventObject::StaticClass()))
					{
					if (EventNodeUI::currentEventNode != static_cast<AEventObject*>(Actor))
					{
						boardingLineNodeUI::UIReady = false;

					}
						EventNodeUI::currentEventNode = static_cast<AEventObject*>(Actor);

						boardingLineNodeUI::EventMG = static_cast<AEventObject*>(EventNodeUI::currentEventNode);

						NodeModeUI::currentAddNode = nullptr;
						if (NodeModeUI::currentNode)
						{
							NodeModeUI::currentNode->SetConnectionsColor(FVector(0, 1, 0));
						}
						ConnectionModeUI::currentConnection = nullptr;
						boardingLineNodeModeUI::currentBoardingLineNode = nullptr;
						NodeModeUI::currentNode = nullptr;
						NodeModeUI::isInAddMode = false;
						ManagerModeUI::currentMode = ManagerModeUI::eventNodeMode;
						EventNodeUI::EVRadiusSlider->SetValue((EventNodeUI::currentEventNode->Radius*0.1) / EventNodeUI::currentEventNode->MaxRadius);
						EventNodeUI::EVRadiusLabel->SetText(FText::FromString("Radius :               " + (FString::FromInt(EventNodeUI::currentEventNode->Radius))));
 
						EventNodeUI::EVflagData->SetText(EventNodeUI::currentEventNode->FlagID);
						EventNodeUI::EVNodeIDData->SetText(EventNodeUI::currentEventNode->nodeID);
						int x =FCString::Atoi(*EventNodeUI::currentEventNode->FlagID);
						if (FlagIDList.Num() >= x)
						{
							EventNodeUI::EVSelectedNodeIDItem = FlagIDList[x];
							EventNodeUI::EVSelectedFlagItem = FlagIDList[x];
						}
						EventNodeUI::EVxPos->SetText(FText::FromString(FString::FromInt(EventNodeUI::currentEventNode->location.X)));
						EventNodeUI::EVyPos->SetText(FText::FromString(FString::FromInt(EventNodeUI::currentEventNode->location.Y)));
						EventNodeUI::EVzPos->SetText(FText::FromString(FString::FromInt(EventNodeUI::currentEventNode->location.Z)));
						return EVisibility::Collapsed;
					}
					else if (Actor->GetClass()->IsChildOf(AboardingLineNode::StaticClass()))
					{
						if (boardingLineNodeModeUI::currentBoardingLineNode != static_cast<AboardingLineNode*>(Actor))
						{
							boardingLineNodeUI::UIReady = false;

						}
						boardingLineNodeModeUI::currentBoardingLineNode = StaticCast<AboardingLineNode*>(Actor);
						ConnectionModeUI::currentConnection = nullptr;
 						NodeModeUI::currentNode = nullptr;
						NodeModeUI::isInAddMode = false;
						EventNodeUI::currentEventNode = nullptr;
						ManagerModeUI::currentMode = ManagerModeUI::boardingNodeMode;
						return EVisibility::Collapsed;
					}
					else
					{	
						{
							if (GEditor->GetSelectedActors()->Num() > 0)
							{
								for (FSelectionIterator Iter(*GEditor->GetSelectedActors()); Iter; ++Iter)
								{
									AActor* Actor = Cast<AActor>(*Iter);
									lastPos = Actor->GetActorLocation();

									if (Actor->GetClass()->IsChildOf(AEventObject::StaticClass()))
									{
										 

										ManagerModeUI::currentMode = ManagerModeUI::eventNodeMode;
										return EVisibility::Collapsed;
									}
								}
							}
							
						}
						if (NodeModeUI::currentNode)
						{
							NodeModeUI::currentNode->SetConnectionsColor(FVector(0, 1, 0));
							NodeModeUI::currentNode = nullptr;							
						}
						if (ConnectionModeUI::currentConnection)
						{
							ConnectionModeUI::currentConnection = nullptr;
							
						}
						if (boardingLineNodeModeUI::currentBoardingLineNode)
						{
							boardingLineNodeModeUI::currentBoardingLineNode = nullptr;
						}
						if (EventNodeUI::currentEventNode)
						{
							EventNodeUI::currentEventNode = nullptr;
							boardingLineNodeUI::UIReady = false;

							boardingLineNodeUI::EventMG = static_cast<AEventObject*>(EventNodeUI::currentEventNode);

							 
						}
						NodeModeUI::isInAddMode = false;
						ManagerModeUI::currentMode = ManagerModeUI::managerMode;
						return EVisibility::Visible;
					}
				}
			
				
			}
			else
			{
				if (NodeModeUI::currentNode)
				{
					NodeModeUI::currentNode->SetConnectionsColor(FVector(0, 1, 0));
					NodeModeUI::currentNode = nullptr;
				}
				if (ConnectionModeUI::currentConnection)
				{
					ConnectionModeUI::currentConnection = nullptr;
					boardingLineNodeModeUI::currentBoardingLineNode = nullptr;
				}
				ManagerModeUI::currentMode = ManagerModeUI::managerMode;
				return EVisibility::Visible;
			}
			ManagerModeUI::currentMode = ManagerModeUI::managerMode;
			return EVisibility::Visible;
		}
		static FReply createManager()
		{
		 
				// Let editor know that we're about to do something that we want to undo/redo
				GEditor->BeginTransaction(LOCTEXT("Create AIEMG", "Create manager"));

				FLevelEditorViewportClient* ViewportClient = (FLevelEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();
				FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
					ViewportClient->Viewport,
					ViewportClient->GetScene(),
					ViewportClient->EngineShowFlags));

				UWorld* myWorld = ViewportClient->GetWorld();
				TArray<AActor*> FoundActors;
				UGameplayStatics::GetAllActorsOfClass(myWorld, AIEditorManager::StaticClass(), FoundActors);

				if (FoundActors.Num() == 0)
				{

					AActor* actor = GEditor->AddActor(myWorld->GetCurrentLevel(),
						AIEditorManager::StaticClass(),
						FTransform(), true, RF_Public | RF_Transactional);

					ManagerModeUI::wayPointMGG = static_cast<AIEditorManager*>(actor);


				}

				else if (FoundActors.Num() == 1)
				{


					ManagerModeUI::wayPointMGG = static_cast<AIEditorManager*>(FoundActors[0]);
				}
				GEditor->EndTransaction();

				return FReply::Handled();
			 
		}
		
		static TSharedRef<SWidget> generateDirectionCBontent(TSharedPtr< FString > Value)
		{

			return SNew(STextBlock).Text(FText::FromString(*Value));
		}
		static void onDirectionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type)
		{
			/*if (selectedItem != NewValue && ManagerModeUI::wayPointMGG)
			{
				ManagerModeUI::wayPointMGG->Reset();
			}*/
			ManagerModeUI::currentDirection = NewValue;
			ManagerModeUI::directionComboBox->SetSelectedItem(NewValue);
			FString f = *ManagerModeUI::directionComboBox->GetSelectedItem().Get();
			ManagerModeUI::directionComboData->SetText(f);
		}
		static FReply resetAIEditor()
		{
			GEditor->BeginTransaction(LOCTEXT("resetLabel", "Reset"));
			if (ManagerModeUI::wayPointMGG)
			{
				NodeModeUI::firstNode = false;
				ManagerModeUI::wayPointMGG->Reset();
			}
			GEditor->EndTransaction();
			return FReply::Handled();
		}
		/*static FReply SetupStartPos()
		{
			GEditor->BeginTransaction(LOCTEXT("setupStartpos", "SetupStarPos"));

			FLevelEditorViewportClient* ViewportClient = (FLevelEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();
			FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
				ViewportClient->Viewport,
				ViewportClient->GetScene(),
				ViewportClient->EngineShowFlags));
			FSceneView* View = ViewportClient->CalcSceneView(&ViewFamily);
  			ManagerModeUI::xPos->SetText(FText::FromString(FString::FromInt(View->ViewLocation.X)));
			ManagerModeUI::yPos->SetText(FText::FromString(FString::FromInt(View->ViewLocation.Y)));
			ManagerModeUI::zPos->SetText(FText::FromString(FString::FromInt(View->ViewLocation.Z)));
			GEditor->EndTransaction();
			return FReply::Handled();
		}*/
		
		//Events
		static EVisibility EventModeToggle()
		{
			if (ManagerModeUI::currentMode == ManagerModeUI::eventMode)
			{			
				EventNodeUI::CreateEventNodesList();
				return EVisibility::Visible;
			}
			else
			{		
				if (ManagerModeUI::wayPointMGG)
				{
					ManagerModeUI::wayPointMGG->hasEventUI = false;
				}
				return EVisibility::Collapsed;
			}
		}
		static bool EventModeValidator()
		{
			return(ManagerModeUI::wayPointMGG && ManagerModeUI::wayPointMGG->nodes.Num() > 0);
		}
		static void EventObjectDragDrop()
		{
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
						createEventObject(OutHit.ImpactPoint, ViewportClient->GetWorld());




					}
				}

				//create

			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("out of range"));
			}



		}
		static void createEventObject(FVector WorldPosition, UWorld* world)
		{
			FVector objectPosition(WorldPosition + FVector(0, 0, 3));
			FRotator objectRotation(0, 0, 0); //in degrees
			FVector objectScale(1, 1, 1);
			FTransform objectTrasform(objectRotation, objectPosition, objectScale);
			UWorld* c = world;
			ULevel* level = c->GetCurrentLevel();


			AActor* newactor = GEditor->AddActor(level, AEventObject::StaticClass(), objectTrasform, true, RF_Public | RF_Transactional);




			newactor->SetActorLabel("Event Object" + FString::FromInt(numOfEventObject));

			auto newEvent = static_cast<AEventObject*>(newactor);
			if (newEvent != NULL)
			{
				newEvent->location = objectPosition;
				newEvent->FlagID = FString::FromInt(0);
			}
			numOfEventObject++;
			newactor->RerunConstructionScripts();

			EventNodeUI::EVxPos->SetText(FText::FromString(FString::FromInt(objectPosition.X)));
			EventNodeUI::EVyPos->SetText(FText::FromString(FString::FromInt(objectPosition.Y)));
			EventNodeUI::EVzPos->SetText(FText::FromString(FString::FromInt(objectPosition.Z)));


			if (ManagerModeUI::wayPointMGG)
			{
				newEvent->mg = ManagerModeUI::wayPointMGG;
				ManagerModeUI::wayPointMGG->Events.Add(newEvent);
				ManagerModeUI::wayPointMGG->RerunConstructionScripts();
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
					newEvent->mg = ManagerModeUI::wayPointMGG;
					ManagerModeUI::wayPointMGG->Events.Add(newEvent);
					ManagerModeUI::wayPointMGG->RerunConstructionScripts();
				}
			}
			GLevelEditorModeTools().MapChangeNotify();


		}
		static AEventObject* createEventObject(FVector worldPos,int flagID,int nodeID,float radius, UWorld* world)
		{
			 
			
			FRotator objectRotation(0, 0, 0); //in degrees
			FVector objectScale(1, 1, 1);
			FTransform objectTrasform(objectRotation, worldPos, objectScale);
			UWorld* c = world;
			ULevel* level = c->GetCurrentLevel();


			AActor* newactor = GEditor->AddActor(level, AEventObject::StaticClass(), objectTrasform, true, RF_Public | RF_Transactional);




			newactor->SetActorLabel("Event Object" + FString::FromInt(numOfEventObject));

			auto newEvent = static_cast<AEventObject*>(newactor);
			if (newEvent != NULL)
			{
				newEvent->location = worldPos;
				//newEvent->id = FString::FromInt(id);
				newEvent->FlagID = FString::FromInt( flagID);
				newEvent->updateRadius(radius);
				newEvent->nodeID = FString::FromInt(nodeID);
			}
			numOfEventObject++;
			newactor->RerunConstructionScripts();

			EventNodeUI::EVxPos->SetText(FText::FromString(FString::FromInt(worldPos.X)));
			EventNodeUI::EVyPos->SetText(FText::FromString(FString::FromInt(worldPos.Y)));
			EventNodeUI::EVzPos->SetText(FText::FromString(FString::FromInt(worldPos.Z)));
			EventNodeUI::EVRadiusSlider->SetValue(radius);
			EventNodeUI::EVRadiusLabel->SetText(FText::FromString(FString::FromInt(radius)));

			if (ManagerModeUI::wayPointMGG)
			{
				newEvent->mg = ManagerModeUI::wayPointMGG;
				ManagerModeUI::wayPointMGG->Events.Add(newEvent);
				ManagerModeUI::wayPointMGG->RerunConstructionScripts();
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
					newEvent->mg = ManagerModeUI::wayPointMGG;
					ManagerModeUI::wayPointMGG->Events.Add(newEvent);
					ManagerModeUI::wayPointMGG->RerunConstructionScripts();
				}
			}
			GLevelEditorModeTools().MapChangeNotify();
			return newEvent;

		}
		
		//TODO delete
		static void createEventObject(TArray<FString> lineData, UWorld* world)
		{
			float x = FCString::Atof(*lineData[2]);
			float y = FCString::Atof(*lineData[3]);
			float z = FCString::Atof(*lineData[4]);
			FVector WorldPosition = FVector(x, y, z);
			FVector objectPosition(WorldPosition);
			FRotator objectRotation(0, 0, 0); //in degrees
			FVector objectScale(1, 1, 1);
			FTransform objectTrasform(objectRotation, objectPosition, objectScale);
			UWorld* c = world;
			ULevel* level = c->GetCurrentLevel();


			AActor* newactor = GEditor->AddActor(level, AEventObject::StaticClass(), objectTrasform, true, RF_Public | RF_Transactional);




			newactor->SetActorLabel("Event Object" + FString::FromInt(numOfEventObject));

			auto newEvent = static_cast<AEventObject*>(newactor);
			if (newEvent != NULL)
			{
				newEvent->location = objectPosition;
				//newEvent->id = lineData[0];
				newEvent->FlagID = lineData[1];
				newEvent->updateRadius(FCString::Atof(*lineData[5]));
				newEvent->nodeID = lineData[6];
			}
			numOfEventObject++;
			newactor->RerunConstructionScripts();

			EventNodeUI::EVxPos->SetText(FText::FromString(FString::FromInt(objectPosition.X)));
			EventNodeUI::EVyPos->SetText(FText::FromString(FString::FromInt(objectPosition.Y)));
			EventNodeUI::EVzPos->SetText(FText::FromString(FString::FromInt(objectPosition.Z)));


			if (ManagerModeUI::wayPointMGG)
			{
				newEvent->mg = ManagerModeUI::wayPointMGG;
				ManagerModeUI::wayPointMGG->Events.Add(newEvent);
				ManagerModeUI::wayPointMGG->RerunConstructionScripts();
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
					newEvent->mg = ManagerModeUI::wayPointMGG;
					ManagerModeUI::wayPointMGG->Events.Add(newEvent);
					ManagerModeUI::wayPointMGG->RerunConstructionScripts();
				}
			}
			GLevelEditorModeTools().MapChangeNotify();


		}
		static void onNodeIDChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type)
		{
			/*if (selectedItem != NewValue && ManagerModeUI::wayPointMGG)
			{
				ManagerModeUI::wayPointMGG->Reset();
			}*/
			ManagerModeUI::currentDirection = NewValue;
			EventNodeUI:: EventNodeUI::EVNodeIDComboBox->SetSelectedItem(NewValue);
			FString f = *EventNodeUI::EVNodeIDComboBox->GetSelectedItem().Get();
			EventNodeUI::EVNodeIDData->SetText(f);
			if (EventNodeUI::currentEventNode)
			{
				EventNodeUI::currentEventNode->nodeID = f;
			}
		}
		static void onFlagChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type)
		{
			/*if (selectedItem != NewValue && ManagerModeUI::wayPointMGG)
			{
				ManagerModeUI::wayPointMGG->Reset();
			}*/
			ManagerModeUI::currentDirection = NewValue;
			EventNodeUI::EVflagComboBox->SetSelectedItem(NewValue);
			FString f = *EventNodeUI::EVflagComboBox->GetSelectedItem().Get();
			EventNodeUI::EVflagData->SetText(f);
			if (EventNodeUI::currentEventNode && f != "2")
			{
				if (EventNodeUI::currentEventNode->boardingLinePositions.Num() > 0)
				{
					EventNodeUI::currentEventNode->RemoveAllBoardingPositions();
					boardingLineNodeUI::UIReady = false;
					
					FlushPersistentDebugLines(getLocalWorld());

				}
			}
			if (EventNodeUI::currentEventNode)
			{
				EventNodeUI::currentEventNode->FlagID = f;
			}
		}
		static TSharedRef<SWidget> generateNodeIDCBontent(TSharedPtr< FString > Value)
		{

			return SNew(STextBlock).Text(FText::FromString(*Value));
		}
		static TSharedRef<SWidget> generateFlagCBontent(TSharedPtr< FString > Value)
		{

			return SNew(STextBlock).Text(FText::FromString(*Value));
		}
		static void updateRadiuus(float NewValue)
		{
			if (EventNodeUI::currentEventNode)
			{
				EventNodeUI::currentEventNode->updateRadius(NewValue*EventNodeUI::currentEventNode->MaxRadius);
				EventNodeUI::EVRadiusLabel->SetText(FText::FromString("Radius :               " + (FString::FromInt(NewValue*EventNodeUI::currentEventNode->MaxRadius))));

			}
		}
		static FReply toggleAddNodeIDMode()
		{
			GEditor->BeginTransaction(LOCTEXT("changemodeToSelectIDMode", "changemodeToSelectIDMode"));
			if (ManagerModeUI::currentMode == ManagerModeUI::selectNodeIdMode)
			{
				ManagerModeUI::currentMode = ManagerModeUI::eventNodeMode;
			}
			else
			{
				ManagerModeUI::currentMode = ManagerModeUI::selectNodeIdMode;
			}
			GEditor->EndTransaction();
			return FReply::Handled();
		}
		static FReply SetEventMode()
		{
			GEditor->BeginTransaction(LOCTEXT("setevmod", "Set in Event Mode"));
			ManagerModeUI::currentMode = ManagerModeUI::eventMode;
			NodeModeUI::isInAddMode = false;
			NodeModeUI::currentAddNode = nullptr;
			if (NodeModeUI::currentNode)
			{
				NodeModeUI::currentNode->SetConnectionsColor(FVector(0, 1, 0));
			}
			NodeModeUI::currentNode = nullptr;
			ConnectionModeUI::currentConnection = nullptr;
			boardingLineNodeModeUI::currentBoardingLineNode = nullptr;
			EventNodeUI::currentEventNode = nullptr;
			boardingLineNodeUI::UIReady = false;

			boardingLineNodeUI::EventMG = static_cast<AEventObject*>(EventNodeUI::currentEventNode);

			GEditor->EndTransaction();

			return FReply::Handled();

		}
		static FReply exitEventMode()
		{
			GEditor->BeginTransaction(LOCTEXT("exit ev mode", "exit event mode"));

			ManagerModeUI::currentMode = ManagerModeUI::managerMode;
			GEditor->EndTransaction();
			return FReply::Handled();
		}

		//Misc
		static bool saveValidator()
		{
			return(ManagerModeUI::wayPointMGG && ManagerModeUI::wayPointMGG->nodes.Num() > 0);

		}
		static bool loadValidator()
		{
			return(!ManagerModeUI::wayPointMGG || ManagerModeUI::wayPointMGG->nodes.Num() == 0);
		}
		static bool moveCameraValidator()
		{
			FLevelEditorViewportClient* ViewportClient = (FLevelEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();

			if (ViewportClient && !ViewportClient->IsCameraLocked())
				return true;
			else
				return false;
			
			 
		}
		static bool emptySceneValidator()
		{
			FLevelEditorViewportClient* ViewportClient = (FLevelEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();
			UWorld* myWorld = ViewportClient->GetWorld();
			if (myWorld->WorldType == EWorldType::Editor)
			{
				FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
					ViewportClient->Viewport,
					ViewportClient->GetScene(),
					ViewportClient->EngineShowFlags));


				TArray<AActor*> FoundActors;
				UGameplayStatics::GetAllActorsOfClass(myWorld, AIEditorManager::StaticClass(), FoundActors);

				return (FoundActors.Num() == 0);
			}
			else
			{
				return false;
			}
		}		
		static bool editorModeValidator()
		{
			FLevelEditorViewportClient* ViewportClient = (FLevelEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();
			UWorld* myWorld = ViewportClient->GetWorld();
			return myWorld->WorldType == EWorldType::Editor;
		}		
	
		static UWorld* getLocalWorld()
		{
			FLevelEditorViewportClient* ViewportClient = (FLevelEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();
			
			UWorld* myWorld = ViewportClient->GetWorld();
			return myWorld;
		}
		static FReply moveCameraToRegion()
		{
			
			{
				GEditor->BeginTransaction(LOCTEXT("set cam pos", "set camera position"));
				FLevelEditorViewportClient* ViewportClient = (FLevelEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();
				FVector pos = FVector(FCString::Atoi(*ManagerModeUI::xPos->GetText().ToString()),
					FCString::Atoi(*ManagerModeUI::yPos->GetText().ToString()),
					FCString::Atoi(*ManagerModeUI::zPos->GetText().ToString()));
				//pos.X += FCString::Atoi(*width->GetText().ToString()) / 2.0;
				//pos.Z += 350;
				if (ViewportClient)
				{

					ViewportClient->SetViewLocation(pos);
				}
				GEditor->EndTransaction();
				
			}
			return FReply::Handled();
		}
		static FReply moveCameraToPosition(FVector destination)
		{
			GEditor->BeginTransaction(LOCTEXT("set cam pos", "set camera position"));
			FLevelEditorViewportClient* ViewportClient = (FLevelEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();
			 
			if (ViewportClient)
			{

				ViewportClient->SetViewLocation(destination +FVector(0,-10,45));
			}
			GEditor->EndTransaction();
			return FReply::Handled();
		}
		static FReply loadData()
		{
			GEditor->BeginTransaction(LOCTEXT("load data", "load data"));
			NodeModeUI::firstNode = false;
			createManager();
			if (ManagerModeUI::wayPointMGG)
			{
				 
				//TArray<FString> data;
				ManagerModeUI::wayPointMGG->levelFileLocation = ManagerModeUI::levelLocation->GetText().ToString();
				ManagerModeUI::wayPointMGG->levelID = ManagerModeUI::levelID->GetText().ToString();
				ManagerModeUI::wayPointMGG->levelDirection = ManagerModeUI::directionComboData->GetText().ToString();

 				FString direction = FString("_TH_") + ManagerModeUI::directionComboData->GetText().ToString();
				if (direction == "_TH_NONE")
					direction = "";
 				FString AbsoluteFilePath = FPaths::GameDir() +
										   ManagerModeUI::levelLocation->GetText().ToString() +
											ManagerModeUI::levelID->GetText().ToString() + direction
										   + FString(".map");


 				//data = ManagerModeUI::wayPointMGG->loadFile();
				std::ifstream in(*AbsoluteFilePath);
				if (in)
				{
					UWorld* world = getLocalWorld();
					int numNodes;
					in >> numNodes;
					for (int i = 1; i <= numNodes; i++)
					{
						FVector worldpos = FVector();
						worldpos = FVector();
						char buffer[50];
						in >> buffer >> buffer >> buffer >> worldpos.X >> buffer >> worldpos.Y >> buffer >> worldpos.Z;
						NodeModeUI::createNode(worldpos, world);
						 
					}
					int numConnections;
					in >> numConnections;
					for (int i = 0; i < numConnections; i++)
					{
						char buffer[100];
						int fromNodeIndex;// = FCString::Atoi(*lineData[1]);
						int toNodeIndex; //= FCString::Atoi(*lineData[3]);
						in >> buffer >> fromNodeIndex >> buffer >> toNodeIndex >> buffer >> buffer >> buffer >> buffer>> buffer>> buffer;
							if (ManagerModeUI::wayPointMGG->nodes.Num() > fromNodeIndex && ManagerModeUI::wayPointMGG->nodes.Num() > toNodeIndex)
								ManagerModeUI::wayPointMGG->nodes[fromNodeIndex]->AddObjectToConnections(ManagerModeUI::wayPointMGG->nodes[toNodeIndex]);
							
							
						 

					}
					FVector AIModulePosition;
					FVector Area;
					FVector colsRowsLayers;
					in >> AIModulePosition.X >> AIModulePosition.Y >> AIModulePosition.Z>>Area.X>>Area.Y>>Area.Z
					   >> colsRowsLayers.X>>colsRowsLayers.Y>>colsRowsLayers.Z;
					ManagerModeUI::xPos->SetText(FText::FromString(FString::FromInt(AIModulePosition.X)));  
					ManagerModeUI::yPos->SetText(FText::FromString(FString::FromInt(AIModulePosition.Y)));  
					ManagerModeUI::zPos->SetText(FText::FromString(FString::FromInt(AIModulePosition.Z)));  
					ManagerModeUI::width->SetText(FText::FromString(FString::FromInt(Area.X)));
					ManagerModeUI::height->SetText(FText::FromString(FString::FromInt(Area.Y)));
					ManagerModeUI::depth->SetText(FText::FromString(FString::FromInt(Area.Z)));
					ManagerModeUI::columns->SetText(FText::FromString(FString::FromInt(colsRowsLayers.X)));
					ManagerModeUI::rows->SetText(FText::FromString(FString::FromInt(colsRowsLayers.Y)));
					ManagerModeUI::Layers->SetText(FText::FromString(FString::FromInt(colsRowsLayers.Z)));

					while (!in.eof())
					{
						 
						 
						int flagID;
						FVector worldPos;
						float radius;
						int nodeID;
						
						
						in >> flagID >> worldPos.X >> worldPos.Y >> worldPos.Z >> radius >> nodeID;
						AEventObject* lastEvent= createEventObject(worldPos, flagID, nodeID, radius / 10.0, world);

						if (flagID == 2)
						{
							int numOfBordingPositions, numofExitingPosition;
							FVector nbposition=FVector();
							FVector nbdirection = FVector();
							float angle;
							in >> numOfBordingPositions>> numofExitingPosition;
							for (int i = 0; i < numOfBordingPositions; i++)
							{
								in >> nbposition.X >> nbposition.Y >> nbposition.Z>>
									nbdirection.X >> nbdirection.Y >> nbdirection.Z>>angle;
								boardingLineNodeUI::createBoardingLineNodeObject(lastEvent, nbposition,nbdirection,angle, getLocalWorld());
							}
							FVector nbEposition = FVector();
							FVector nbEdirection = FVector();
							float Eangle;
							for (int i = 0; i < numofExitingPosition; i++)
							{
								in >> nbEposition.X >> nbEposition.Y >> nbEposition.Z >>
									nbEdirection.X >> nbEdirection.Y >> nbEdirection.Z >> Eangle;
								boardingLineNodeUI::createBoardingLineNodeObject(lastEvent, nbEposition, nbEdirection, Eangle, getLocalWorld(),true);
							}
						}

						 
						 
					}
					
					 
					 

				}
						}
			
			NodeModeUI::exitNodeMode();
			moveCameraToRegion();
			GEditor->EndTransaction();
			
			return FReply::Handled();
		}
		static FReply saveData()
		{
			GEditor->BeginTransaction(LOCTEXT("save data", "save data"));
			if (ManagerModeUI::wayPointMGG)
			{
				ManagerModeUI::wayPointMGG->levelFileLocation = ManagerModeUI::levelLocation->GetText().ToString();
				ManagerModeUI::wayPointMGG->levelID = ManagerModeUI::levelID->GetText().ToString();
				ManagerModeUI::wayPointMGG->levelDirection = ManagerModeUI::directionComboData->GetText().ToString();
				ManagerModeUI::wayPointMGG->posX = ManagerModeUI::xPos->GetText().ToString();
				ManagerModeUI::wayPointMGG->posY = ManagerModeUI::yPos->GetText().ToString();
				ManagerModeUI::wayPointMGG->posZ = ManagerModeUI::zPos->GetText().ToString();
				ManagerModeUI::wayPointMGG->width = ManagerModeUI::width->GetText().ToString();
				ManagerModeUI::wayPointMGG->height = ManagerModeUI::height->GetText().ToString();
				ManagerModeUI::wayPointMGG->depth = ManagerModeUI::depth->GetText().ToString();
				ManagerModeUI::wayPointMGG->columns = ManagerModeUI::columns->GetText().ToString();
				ManagerModeUI::wayPointMGG->rows = ManagerModeUI::rows->GetText().ToString();
				ManagerModeUI::wayPointMGG->layers = ManagerModeUI::Layers->GetText().ToString();
				//ManagerModeUI::wayPointMGG->SaveFile();

				/////////////
				FString SaveDirectory = FPaths::ProjectDir();
				FString FileName = ManagerModeUI::wayPointMGG->levelID + (ManagerModeUI::wayPointMGG->levelDirection == "NONE" ? "" : FString("_TH_") + ManagerModeUI::wayPointMGG->levelDirection);
				std::ofstream myfile;
				myfile.open(*(SaveDirectory + ManagerModeUI::wayPointMGG->levelFileLocation + FileName + FString(".map")), std::fstream::trunc);
				if (myfile.is_open())
				{
					myfile << TCHAR_TO_ANSI(*(FString::FromInt(ManagerModeUI::wayPointMGG->nodes.Num()) + FString("\n")));
					for (int i = 0; i <  ManagerModeUI::wayPointMGG->nodes.Num(); i++)
					{
						if ( ManagerModeUI::wayPointMGG->nodes[i])
						{
							myfile << TCHAR_TO_ANSI(*("index: " + FString::FromInt(i) +
								(" PosX: ") + FString::FromInt( ManagerModeUI::wayPointMGG->nodes[i]->location.X) +
								(" PosY: ") + FString::FromInt( ManagerModeUI::wayPointMGG->nodes[i]->location.Y) +
								(" PosZ: ") + FString::FromInt( ManagerModeUI::wayPointMGG->nodes[i]->location.Z) +
								("\n")));
						}
					}
					int numConnections = 0;
					for (int i = 0; i <  ManagerModeUI::wayPointMGG->nodes.Num(); i++)
					{
						if ( ManagerModeUI::wayPointMGG->nodes[i])
						{
							numConnections +=  ManagerModeUI::wayPointMGG->nodes[i]->nearNodes.Num();
						}
						else
							UE_LOG(LogTemp, Warning, TEXT("Error"));
					}

					myfile << TCHAR_TO_ANSI(*(FString::FromInt(numConnections) + FString("\n")));

					for (int i = 0; i <  ManagerModeUI::wayPointMGG->nodes.Num(); i++)
					{
						if ( ManagerModeUI::wayPointMGG->nodes[i])
						{
							for (int j = 0; j <  ManagerModeUI::wayPointMGG->nodes[i]->connections.Num(); j++)
							{
								if ( ManagerModeUI::wayPointMGG->nodes[i]->nearNodes[j])
								{
									//m_iFrom: 0 m_iTo : 1 m_dCost : 10.739807 m_iFlags : 0 ID : -1
									float distCost = FVector::Dist( ManagerModeUI::wayPointMGG->nodes[i]->location,  ManagerModeUI::wayPointMGG->nodes[i]->nearNodes[j]->location);
									int index =  ManagerModeUI::wayPointMGG->nodes.IndexOfByKey( ManagerModeUI::wayPointMGG->nodes[i]->nearNodes[j]);
									if (index != INDEX_NONE)
									{
										myfile << TCHAR_TO_ANSI(*("m_iFrom: " + FString::FromInt(i) + " m_iTo: " +
											FString::FromInt(index) + (" m_dCost: " +
												FString::SanitizeFloat(distCost) + " m_iFlags: 0 ID: -1") +
												("\n")));
									}
								}
							}
						}
					}

					myfile << TCHAR_TO_ANSI(*( ManagerModeUI::wayPointMGG->posX + (" ") + ManagerModeUI::wayPointMGG->posY + (" ") +
						ManagerModeUI::wayPointMGG->posZ + ("\n") + ManagerModeUI::wayPointMGG->width + (" ") + ManagerModeUI::wayPointMGG->height + (" ") +  ManagerModeUI::wayPointMGG->depth + ("\n") +
						 ManagerModeUI::wayPointMGG->columns + (" ") + ManagerModeUI::wayPointMGG->rows + (" ") + ManagerModeUI::wayPointMGG->layers));

					if (ManagerModeUI::wayPointMGG->Events.Num() > 0)
					{
						//myfile << TCHAR_TO_ANSI(*(FString::FromInt(Events.Num()) + FString("\n")));


						for (int i = 0; i < ManagerModeUI::wayPointMGG->Events.Num(); i++)
						{
							if (ManagerModeUI::wayPointMGG->Events[i])
							{
								myfile << TCHAR_TO_ANSI(*(
									//Events[i]->id							  + " " +
									("\n") +
									 ManagerModeUI::wayPointMGG->Events[i]->FlagID + " " +
									FString::FromInt( ManagerModeUI::wayPointMGG->Events[i]->location.X) + " " +
									FString::FromInt( ManagerModeUI::wayPointMGG->Events[i]->location.Y) + " " +
									FString::FromInt( ManagerModeUI::wayPointMGG->Events[i]->location.Z) + " " +
									FString::SanitizeFloat( ManagerModeUI::wayPointMGG->Events[i]->Radius) + " " +
									 ManagerModeUI::wayPointMGG->Events[i]->nodeID));

								if ( ManagerModeUI::wayPointMGG->Events[i]->FlagID == "2")
								{
									myfile << TCHAR_TO_ANSI(*(
										(" ") +
										FString::FromInt( ManagerModeUI::wayPointMGG->Events[i]->boardingLinePositions.Num())));
									myfile << TCHAR_TO_ANSI(*(
										(" ") +
										FString::FromInt( ManagerModeUI::wayPointMGG->Events[i]->unBoardingLinePositions.Num())));

									for (int j = 0; j <  ManagerModeUI::wayPointMGG->Events[i]->boardingLinePositions.Num(); j++)
									{
										FVector blpos =  ManagerModeUI::wayPointMGG->Events[i]->boardingLinePositions[j]->GetActorLocation();
										FVector blposDirection =  ManagerModeUI::wayPointMGG->Events[i]->boardingLinePositions[j]->direction;
										myfile << TCHAR_TO_ANSI(*(

											("\n") +

											FString::FromInt(blpos.X) + " " +
											FString::FromInt(blpos.Y) + " " +
											FString::FromInt(blpos.Z) + " " +
											FString::FromInt(blposDirection.X) + " " +
											FString::FromInt(blposDirection.Y) + " " +
											FString::FromInt(blposDirection.Z) + " " +
											FString::FromInt( ManagerModeUI::wayPointMGG->Events[i]->boardingLinePositions[j]->Angle)));

									}

									for (int j = 0; j <  ManagerModeUI::wayPointMGG->Events[i]->unBoardingLinePositions.Num(); j++)
									{
										FVector blpos =  ManagerModeUI::wayPointMGG->Events[i]->unBoardingLinePositions[j]->GetActorLocation();
										FVector blposDirection =  ManagerModeUI::wayPointMGG->Events[i]->unBoardingLinePositions[j]->direction;
										myfile << TCHAR_TO_ANSI(*(

											("\n") +

											FString::FromInt(blpos.X) + " " +
											FString::FromInt(blpos.Y) + " " +
											FString::FromInt(blpos.Z) + " " +
											FString::FromInt(blposDirection.X) + " " +
											FString::FromInt(blposDirection.Y) + " " +
											FString::FromInt(blposDirection.Z) + " " +
											FString::FromInt( ManagerModeUI::wayPointMGG->Events[i]->unBoardingLinePositions[j]->Angle)));

									}
								}
							}
						}
					}
					if (myfile.is_open())
					{
						myfile.close();
					}
				//	return true;
				}
				//return false;
				//////////////













			}
			GEditor->EndTransaction();
			return FReply::Handled();
		}
		static FString getLevelName()
		{
			if (GEditor)
				return (GEditor->GetWorld()->GetCurrentLevel()->GetName());
			else
				return ("ERROR");
		} 
		static FText getActorName(AActor* actor)
		{
			if (actor)
				return FText::FromString(actor->GetActorLabel());
			else
			{
				return FText::FromString("No Node Selected");
			}
		}


	
	
 	};
#pragma endregion

	const float Factor = 256.0f;
	/*if ( ManagerModeUI::wayPointMGG && ManagerModeUI::wayPointMGG->nodes.Num()>0 && !Locals::getLocalWorld()->GetFirstPlayerController())
	{
		Locals::resetAIEditor();
	}*/

#pragma region UISlate


			//NODE
 			SAssignNew(NodeManagerToolkit, SBorder)
			//.HAlign(HAlign_Left)
			[	
				SNew(SVerticalBox)
				//Connection
#pragma region ConnectionMode
				+ SVerticalBox::Slot()
				[
					SNew(SScrollBox).Visibility_Static(&ConnectionModeUI::connectionModeToggle)
					+ SScrollBox::Slot()
					.Padding(5.0f)
					[
						SNew(SGridPanel)
						.FillColumn(0, 1.0f)
 						+ SGridPanel::Slot(0, 0).Padding(150, 10)
						[
							SAssignNew(ConnectionModeUI::fromNodeName, STextBlock).OnDoubleClicked_Static(&NodeModeUI::SelectNode,0)

						]
						 
						+ SGridPanel::Slot(0, 1).Padding(150, 10)
							[
								SAssignNew(ConnectionModeUI::toNodeName, STextBlock).OnDoubleClicked_Static(&NodeModeUI::SelectNode, 1)

							]
					 
						+ SGridPanel::Slot(0, 2).Padding(150, 10)
							[
								SAssignNew(ConnectionModeUI::costName, STextBlock)
							]
						+ SGridPanel::Slot(0, 3).Padding(150, 10)
							[
								SAssignNew(ConnectionModeUI::deleteConnectionButton, SButton).OnClicked_Static(&ConnectionModeUI::removeConnection)
								[
									SNew(STextBlock).Text(LOCTEXT("deleteconnectionbtn","Delete"))
								]

							]
						+ SGridPanel::Slot(0, 4).Padding(150, 10)
							[
								SAssignNew(ConnectionModeUI::deleteConnectionButton, SButton).OnClicked_Static(&NodeModeUI::exitNodeMode)
								[
									SNew(STextBlock).Text(LOCTEXT("exit ConnectionMode", "Exit"))
								]

							]
						
					]
				]
#pragma endregion
				//Node
#pragma region NodeMode
				+ SVerticalBox::Slot()
				[
					SNew(SScrollBox).Visibility_Static(&NodeModeUI::NodeModeToggle)

					
				+ SScrollBox::Slot()
				.Padding(5.0f)
				[
				SNew(SGridPanel)
				.FillColumn(0, 1.0f)


				+ SGridPanel::Slot(1, 0).Padding(150, 10)
					[
						SAssignNew(NodeModeUI::nodeName,STextBlock)
						
					]

				 

				]
				
				+ SScrollBox::Slot().HAlign(EHorizontalAlignment::HAlign_Center)
				.Padding(5.0f)
				[

					SAssignNew(ConnectionModeUI::nodeConnectionsPanel, SGridPanel)


				]
				+ SScrollBox::Slot()
					.Padding(5.0f)
					[
						SNew(SGridPanel)
						.FillColumn(0, 1.0f)
					 
					+ SGridPanel::Slot(1, 0).Padding(150, 10)
					[
						SNew(SButton).Visibility_Static(&NodeModeUI::nodeDataModeToggle).OnClicked_Static(&NodeModeUI::addConectionModeToggle)[
							SAssignNew(NodeModeUI::addButtonLabel, STextBlock)
						]
					]
				
				+ SGridPanel::Slot(1, 1).Padding(150, 10)
					[
						SNew(SButton).Visibility_Static(&NodeModeUI::nodeADDDataModeToggle).OnClicked_Static(&NodeModeUI::addConectionModeToggle)[
							SNew(STextBlock).Text(LOCTEXT("ExittBtnaddmode", "Exit Mode"))]
					]
					 
				
				+ SGridPanel::Slot(1, 2).Padding(150, 10)
					[
						SNew(SButton).Visibility_Static(&NodeModeUI::nodeDataModeToggle).OnClicked_Static(&NodeModeUI::deleteCurrentNode)[
							SNew(STextBlock).Text(LOCTEXT("DeletBtn", "Delete"))]
					]
				
				+ SGridPanel::Slot(1, 3).Padding(150, 10)
					[
						SNew(SButton).Visibility_Static(&NodeModeUI::nodeDataModeToggle).OnClicked_Static(&NodeModeUI::exitNodeMode)[
							SNew(STextBlock).Text(LOCTEXT("ExittBtn", "Exit"))]
					]



					]
				]
#pragma endregion
				//Manager
#pragma region ManagerMode
				+ SVerticalBox::Slot()
				[
						SNew(SScrollBox).Visibility_Static(&Locals::managerModeToggle)

						+ SScrollBox::Slot()
					.Padding(5.0f)
					[
						SNew(SGridPanel)
						.FillColumn(0, 0.5f)
						.FillColumn(1, 0.5f)

					// SBorder
					+ SGridPanel::Slot(0, 0).Padding(0, 10)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("AIEMg", "AIEditorManager"))
					]
				+ SGridPanel::Slot(1, 0).HAlign(HAlign_Right)
					[
						SNew(SButton)
						.Text(LOCTEXT("HelperLabel", "CreateManager")).IsEnabled_Static(&Locals::emptySceneValidator).OnClicked_Static(&Locals::createManager)
					.Content()
					[
						SNew(SImage).Image(AIEditorStyle::Get()->GetBrush("Roboto.icon"))
					]

					]

				+ SGridPanel::Slot(0, 1).Padding(0, 10)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("WP", "WayPoint Obj"))
					]
				+ SGridPanel::Slot(1, 1).HAlign(HAlign_Right)
					[
						NodeModeUI::makeNodeButton(LOCTEXT("HelperLabel", "Waypoint"), "waypoint.icon")

					]
				+ SGridPanel::Slot(0, 2).Padding(0, 10)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("IDlevel", "Level ID Location"))
					]
				+ SGridPanel::Slot(1, 2).HAlign(HAlign_Right)
					[
						SAssignNew(ManagerModeUI::levelLocation, SEditableTextBox).Text(FText::FromString("/Content/"))

					]
				+ SGridPanel::Slot(0, 3).Padding(0, 10)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("IDlevel", "Level ID"))
					]
				+ SGridPanel::Slot(1, 3).HAlign(HAlign_Right)
					[
						SAssignNew(ManagerModeUI::levelID, SEditableTextBox).Text(FText::FromString(*levelName))

					]
				+ SGridPanel::Slot(0, 4).Padding(0, 10)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("IDlevel2", "Level Direction"))
					]
				+ SGridPanel::Slot(1, 4).HAlign(HAlign_Right)
					[
						SAssignNew(ManagerModeUI::directionComboBox, SComboBox<TSharedPtr<FString>>)
						.OptionsSource(&DirectionOptions)
					.InitiallySelectedItem(DirectionOptions[2])
					.OnGenerateWidget_Static(&Locals::generateDirectionCBontent)
					.OnSelectionChanged_Static(&Locals::onDirectionChanged)
					[
						//return SNew(STextBlock).Text(FText::FromString(*Value));
						SAssignNew(ManagerModeUI::directionComboData, STextBlock)
						.Text(FText::FromString(*ManagerModeUI::currentDirection))
					]

					]


					]
				+ SScrollBox::Slot()
					.Padding(5.0f)
					[


						SNew(SGridPanel)
						.FillColumn(0, 0.15f)
					.FillColumn(1, 0.10f)
					.FillColumn(2, 0.25f)
					.FillColumn(3, 0.25f)
					.FillColumn(4, 0.25f)
					+ SGridPanel::Slot(0, 0).Padding(0, 10)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("pos", "Position"))
					]
				+ SGridPanel::Slot(1, 0).Padding(0, 10)
					[
						SNew(SButton).OnClicked_Static(&ManagerModeUI::SetupStartPos).Content()
						[
							SNew(STextBlock).Text(LOCTEXT("SetupPosition", "Set"))
						]
					]
				+ SGridPanel::Slot(2, 0).Padding(5, 10)
					[
						SAssignNew(ManagerModeUI::xPos, SEditableTextBox).Text(LOCTEXT("plwop", "20461"))
					]
				+ SGridPanel::Slot(3, 0).Padding(5, 10)
					[
						SAssignNew(ManagerModeUI::yPos, SEditableTextBox).Text(LOCTEXT("plwwop", "1930"))
					]
				+ SGridPanel::Slot(4, 0).Padding(5, 10)
					[
						SAssignNew(ManagerModeUI::zPos, SEditableTextBox).Text(LOCTEXT("plwwwop", "168"))
					]



					]
				+ SScrollBox::Slot()
					.Padding(5.0f)
					[
						SNew(SGridPanel)
						.FillColumn(0, 0.5f)
					.FillColumn(1, 0.5f)
					+ SGridPanel::Slot(0, 0).Padding(0, 10)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("W", "Width"))
					]
				+ SGridPanel::Slot(1, 0).Padding(0, 10)
					[
						SAssignNew(ManagerModeUI::width, SEditableTextBox).Text(LOCTEXT("widthT", "6490"))
					]

				+ SGridPanel::Slot(0, 1).Padding(0, 10)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("H", "Height"))
					]
				+ SGridPanel::Slot(1, 1).Padding(0, 10)
					[
						SAssignNew(ManagerModeUI::height, SEditableTextBox).Text(LOCTEXT("heightT", "1230"))
					]
				+ SGridPanel::Slot(0, 2).Padding(0, 10)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("D", "Depth"))
					]
				+ SGridPanel::Slot(1, 2).Padding(0, 10)
					[
						SAssignNew(ManagerModeUI::depth, SEditableTextBox).Text(LOCTEXT("depthT", "1230"))
					]
					]
				+ SScrollBox::Slot()
					.Padding(5.0f)
					[


						SNew(SGridPanel)
					.FillColumn(0, 0.15f)
					.FillColumn(1, 0.15f)
					.FillColumn(2, 0.15f)
					.FillColumn(3, 0.15f)
					.FillColumn(4, 0.15f)
					.FillColumn(5, 0.15f)
					+ SGridPanel::Slot(0, 0).Padding(0, 10)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("col", "Columns"))
					]
				+ SGridPanel::Slot(1, 0).Padding(0, 10)
					[
						SAssignNew(ManagerModeUI::columns, SEditableTextBox).Text(LOCTEXT("clop", "100"))

					]
				+ SGridPanel::Slot(2, 0).Padding(0, 10)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("row", "Rows"))
					]
				+ SGridPanel::Slot(3, 0).Padding(0, 10)
					[
						SAssignNew(ManagerModeUI::rows, SEditableTextBox).Text(LOCTEXT("plorp", "10"))
					]
				+ SGridPanel::Slot(4, 0).Padding(0, 10)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("layers", "Layers"))
					]
				+ SGridPanel::Slot(5, 0).Padding(0, 10)
					[
						SAssignNew(ManagerModeUI::Layers, SEditableTextBox).Text(LOCTEXT("plorp", "10"))
					]
					]
				 
				+ SScrollBox::Slot()
					.Padding(5.0f)
					[
						SNew(SGridPanel)
						.FillColumn(0, 0.25f)
					.FillColumn(1, 0.25f)
					.FillColumn(2, 0.25f)
					.FillColumn(3, 0.25f)
					+ SGridPanel::Slot(0, 0).Padding(0, 10)
					[
						SNew(SButton)
						.Text(LOCTEXT("HelperLabel", "load")).IsEnabled_Static(&Locals::loadValidator).OnClicked_Static(&Locals::loadData)
					.Content()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("loadb", "Load"))
					]
					]
				+ SGridPanel::Slot(1, 0).Padding(0, 10)
					[
						SNew(SButton)
						.Text(LOCTEXT("HelperLabel", "save")).IsEnabled_Static(&Locals::saveValidator).OnClicked_Static(&Locals::saveData)
					.Content()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("Saveb", "Save"))
					]
					]
				+ SGridPanel::Slot(2, 0).Padding(0, 10)
					[
						SNew(SButton)
						.Text(LOCTEXT("HelperLabel", "gotopos")).IsEnabled_Static(&Locals::moveCameraValidator).OnClicked_Static(&Locals::moveCameraToRegion)
					.Content()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("go to", "位置に行く"))
					]
					]
				+ SGridPanel::Slot(3, 0).Padding(0, 10)
					[
						SNew(SButton)
						.Text(LOCTEXT("HelperLabel", "Reset")).IsEnabled_Static(&Locals::editorModeValidator).OnClicked_Static(&Locals::resetAIEditor)
					.Content()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("reset", "Reset"))
					]
					]
					]
				+ SScrollBox::Slot()
					.Padding(5.0f)
					[
						SNew(SButton)
						.Text(LOCTEXT("EvmodT", "evmode")).IsEnabled_Static(&Locals::EventModeValidator).OnClicked_Static(&Locals::SetEventMode)
					.Content()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("evmodelb", "Event Mode"))
					]

					]
				]
#pragma endregion
#pragma region EventMode
				+ SVerticalBox::Slot()
					[
						SNew(SScrollBox).Visibility_Static(&Locals::EventModeToggle)

						+ SScrollBox::Slot()
					.Padding(5.0f)
					[


						SNew(SGridPanel)
						.FillColumn(0, 0.5f)
					.FillColumn(1, 0.5f)

					// SBorder
					+ SGridPanel::Slot(0, 0).Padding(0, 10)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("EvM", "Event Mode"))
					]
				+ SGridPanel::Slot(1, 1).HAlign(HAlign_Center).Padding(-50, 10)
					[
						SNew(SButton)
						.Text(LOCTEXT("HelperLabel", "Create New Event")).OnReleased_Static(&Locals::EventObjectDragDrop)
					.Content()
					[
						SNew(SImage).Image(AIEditorStyle::Get()->GetBrush("EventIcon.icon"))
					]

					]
				+ SGridPanel::Slot(1, 2).HAlign(HAlign_Right)
					[
						SAssignNew(EventNodeUI::EventsnodePanel, SGridPanel)
						 

					]
				+ SGridPanel::Slot(1, 3).HAlign(HAlign_Right)
					[
						SNew(SButton)
						.Text(LOCTEXT("ExitEM", "Exit")).OnClicked_Static(&Locals::exitEventMode)
					.Content()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("EvMo", "Exit Mode"))
					]

					]

					
				
					]
					]
					

					
					
#pragma endregion
#pragma region EventNodeMode
				+ SVerticalBox::Slot()
					[
						SNew(SScrollBox).Visibility_Static(&EventNodeUI::EventNodeModeToggle)

						
				+ SScrollBox::Slot()
					.Padding(5.0f)
					[


						SNew(SGridPanel)
						.FillColumn(0, 0.25f)
					.FillColumn(1, 0.25f)
					.FillColumn(2, 0.25f)
					.FillColumn(3, 0.25f)
					+ SGridPanel::Slot(0, 0).Padding(0, 10)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("pos", "Position"))
					]
				+ SGridPanel::Slot(1, 0).Padding(5, 10)
					[
						SAssignNew(EventNodeUI::EVxPos, SEditableTextBox).Text(LOCTEXT("plwop", "4"))
					]
				+ SGridPanel::Slot(2, 0).Padding(5, 10)
					[
						SAssignNew(EventNodeUI::EVyPos, SEditableTextBox).Text(LOCTEXT("plwwop", "4"))
					]
				+ SGridPanel::Slot(3, 0).Padding(5, 10)
					[
						SAssignNew(EventNodeUI::EVzPos, SEditableTextBox).Text(LOCTEXT("plwwwop", "4"))
					]



					]
				+ SScrollBox::Slot()
					.Padding(5.0f)
					[
						SNew(SGridPanel)
						.FillColumn(0, 0.5f)
					.FillColumn(1, 0.5f)
					+ SGridPanel::Slot(0, 0).Padding(0, 10)
					[
						SAssignNew(EventNodeUI::EVRadiusLabel,STextBlock)
						.Text(LOCTEXT("rad", "Radius :               1" ))
						
					]
					+ SGridPanel::Slot(1, 0).Padding(0, 10)
					[
						SAssignNew(EventNodeUI::EVRadiusSlider, SSlider).OnValueChanged_Static(&Locals::updateRadiuus).Value(0.028)

					]
					]
					
					+ SScrollBox::Slot()
					[SNew(SGridPanel)
					.FillColumn(0, 0.0f)
					.FillColumn(1, 0.23f)
					.FillColumn(2,0.35f)

					// SBorder
					+ SGridPanel::Slot(0, 0).Padding(0, 10)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("EvNI", "Node ID"))
					]
				+ SGridPanel::Slot(2, 0).Padding(15, 10)
					[
						SAssignNew(EventNodeUI::EVNodeIDComboBox, SComboBox<TSharedPtr<FString>>)
						.OptionsSource(&NodeModeUI::nodesIDList)
					.InitiallySelectedItem(NodeModeUI::nodesIDList[0])
					.OnGenerateWidget_Static(&Locals::generateNodeIDCBontent)
					.OnSelectionChanged_Static(&Locals::onNodeIDChanged)
					[
						//return SNew(STextBlock).Text(FText::FromString(*Value));
						SAssignNew(EventNodeUI::EVNodeIDData, STextBlock)
						.Text(FText::FromString(* EventNodeUI::EVSelectedNodeIDItem))
					]
					] + SGridPanel::Slot(1, 0).Padding(30, 10)
						[
							
							SNew(SButton)
						.Text(LOCTEXT("selectNode", "Select")).OnClicked_Static(&Locals::toggleAddNodeIDMode)
						.Content()
						[
							SNew(STextBlock)
							.Text(LOCTEXT("selectNode", "Select"))
						]
						]

				+ SGridPanel::Slot(0, 1).Padding(0, 10)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("EvF", "Flag ID"))
					]
				+ SGridPanel::Slot(1, 1).Padding(0, 10)
					[
						SAssignNew(EventNodeUI::EVflagComboBox, SComboBox<TSharedPtr<FString>>)
						.OptionsSource(&FlagIDList)
					.InitiallySelectedItem(FlagIDList[0])
					.OnGenerateWidget_Static(&Locals::generateFlagCBontent)
					.OnSelectionChanged_Static(&Locals::onFlagChanged)
					[
						//return SNew(STextBlock).Text(FText::FromString(*Value));
						SAssignNew(EventNodeUI::EVflagData, STextBlock)
						.Text(FText::FromString(* EventNodeUI::EVSelectedFlagItem))
					]
					]

					]
					+ SScrollBox::Slot()
						.Padding(5.0f)
						[


							SNew(SGridPanel)
							.FillColumn(0, 0.5f)
						.FillColumn(1, 0.5f)

						// SBorder

						+SGridPanel::Slot(1, 0).HAlign(HAlign_Right)
						[
							SNew(SButton)
							.Text(LOCTEXT("ExitEM", "Exit")).OnClicked_Static(&EventNodeUI::exitEventNodeMode)
						.Content()
						[
							SNew(STextBlock)
							.Text(LOCTEXT("EvMo", "Exit Mode"))
						]

						]
						+ SGridPanel::Slot(1, 1).HAlign(HAlign_Right)
						[
							SNew(SButton)
							.Text(LOCTEXT("deleteEM", "Delete")).OnClicked_Static(&EventNodeUI::DeleteCurrentEventNode)
						.Content()
						[
							SNew(STextBlock)
							.Text(LOCTEXT("EvMasdo", "Delete Trigger"))
						]

						]
						+ SGridPanel::Slot(1, 2).HAlign(HAlign_Right)
							[

								SAssignNew(boardingLineNodeUI::boardingLinenodePanel, SGridPanel).Visibility_Static(boardingLineNodeUI::boardingLineNodeModeToggle)
								.FillColumn(0, 0.5f)
							.FillColumn(1, 0.5f)

							+ SGridPanel::Slot(1, 0).HAlign(HAlign_Right)
							[

								SNew(STextBlock)
								.Text(FText::FromString("boarding Line UI"))
							]





							]
						]

					]
#pragma endregion	
#pragma region boardingLineNodeMode
					+SVerticalBox::Slot()
						[
							boardingLineNodeModeUI::makeBoardingLineNodeModeUI()
						]
				
#pragma endregion
];

		NodeModeUI::addButtonLabel->SetText(FText::FromString("Add"));
#pragma endregion

		FModeToolkit::Init(InitToolkitHost);
}

FName FAIEditorEdModeToolkit::GetToolkitFName() const
{
	return FName("AIEditorEdMode");
}

FText FAIEditorEdModeToolkit::GetBaseToolkitName() const
{
	return NSLOCTEXT("AIEditorEdModeToolkit", "DisplayName", "AIEditorEdMode Tool");
}

class FEdMode* FAIEditorEdModeToolkit::GetEditorMode() const
{
	return GLevelEditorModeTools().GetActiveMode(FAIEditorEdMode::EM_AIEditorEdModeId);
}
#undef LOCTEXT_NAMESPACE
