// Fill out your copyright notice in the Description page of Project Settings.

#include "NodeModeUI.h"

bool NodeModeUI::firstNode;
AAIBaseNode* NodeModeUI::currentAddNode;
AAIBaseNode* NodeModeUI::currentNode;
bool NodeModeUI::isInAddMode = false;
int NodeModeUI::numOfNode = 0;
TSharedPtr<STextBlock> NodeModeUI::addButtonLabel;

TSharedPtr<STextBlock> NodeModeUI::nodeName; 
TArray<TSharedPtr <FString>> NodeModeUI::nodesIDList = { MakeShareable<FString>(new FString("0")) };

NodeModeUI::NodeModeUI()
{
}

NodeModeUI::~NodeModeUI()
{
}
EVisibility NodeModeUI::NodeModeToggle()
{
	if (currentNode)
	{
		if (!currentNode->hasConnectionUI)
		{
			ConnectionModeUI::connectionsUIRegionGenerator();
		}
		ManagerModeUI::currentMode = ManagerModeUI::nodeMode;
		if (isInAddMode)
		{
			addButtonLabel->SetText(FText::FromString("Exit Add mode"));
		}
		else
		{
			addButtonLabel->SetText(FText::FromString("Enter Add Mode"));
		}
		return EVisibility::Visible;
	}
	else
	{
		return EVisibility::Collapsed;
	}


}
FReply NodeModeUI::addConnectionToNode()
{
	GEditor->BeginTransaction(FText::FromString("addNode"));
	if (currentAddNode != currentNode && currentAddNode && currentNode && isInAddMode)
	{
		currentNode->AddObjectToConnections(currentAddNode);
	}
	GEditor->EndTransaction();
	return FReply::Handled();

	
}
FReply NodeModeUI::addConectionModeToggle()
{
	/*if (isInAddMode)
	{
		addButtonLabel->SetText(FText::FromString("Exit Add Mode"));
		ManagerModeUI::currentMode = ManagerModeUI::addMode;
		return EVisibility::Visible;
	}
	else
	{
		addButtonLabel->SetText(FText::FromString("Enter Add Mode"));

		return EVisibility::Collapsed;
	}*/
	GEditor->BeginTransaction(FText::FromString("Toggle add mode"));
	isInAddMode = !isInAddMode;
	if (isInAddMode)
	{
		addButtonLabel->SetText(FText::FromString("Exit Add mode"));
	}
	else
	{
		addButtonLabel->SetText(FText::FromString("Enter Add Mode"));
	}
	FText qpdo = addButtonLabel->GetText();
	GEditor->EndTransaction();

	return FReply::Handled();

}
void NodeModeUI::createNode(FVector WorldPosition, UWorld* world)
{
	FVector objectPosition(WorldPosition + FVector(0, 0, 3));
	FRotator objectRotation(0, 0, 0); //in degrees
	FVector objectScale(1, 1, 1);
	FTransform objectTrasform(objectRotation, objectPosition, objectScale);
	UWorld* c = world;
	ULevel* level = c->GetCurrentLevel();


	AActor* newactor = GEditor->AddActor(level, AAIBaseNode::StaticClass(), objectTrasform, true, RF_Public | RF_Transactional);

	if (NodeModeUI::firstNode)
	{
		ManagerModeUI::xPos->SetText(FText::FromString(FString::FromInt(newactor->GetActorLocation().X)));
		ManagerModeUI::yPos->SetText(FText::FromString(FString::FromInt(newactor->GetActorLocation().Y)));
		ManagerModeUI::zPos->SetText(FText::FromString(FString::FromInt(newactor->GetActorLocation().Z + 350)));
		NodeModeUI::firstNode = false;
	}


	newactor->SetActorLabel("AI Node" + FString::FromInt(numOfNode));
	if (numOfNode > 0)
	{
		FString x = FString::FromInt(numOfNode);
		nodesIDList.Add(MakeShareable<FString>(new FString(x)));
	}
	numOfNode++;
	auto newNode = static_cast<AAIBaseNode*>(newactor);
	if (newNode != NULL)
	{
		newNode->location = objectPosition;
	}
	newactor->RerunConstructionScripts();

	if (ManagerModeUI::wayPointMGG)
	{
		newNode->mg = ManagerModeUI::wayPointMGG;
		ManagerModeUI::wayPointMGG->nodes.Add(static_cast<AAIBaseNode*>(newactor));
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
			newNode->mg = ManagerModeUI::wayPointMGG;
			ManagerModeUI::wayPointMGG->nodes.Add(static_cast<AAIBaseNode*>(newactor));
			ManagerModeUI::wayPointMGG->RerunConstructionScripts();
		}
	}
	GLevelEditorModeTools().MapChangeNotify();


}
FReply NodeModeUI::SelectNode(int FromTo)
{
	GEditor->BeginTransaction(FText::FromString("set camera position"));
	if (ConnectionModeUI::currentConnection)
	{
		currentNode = (FromTo == 0) ? ConnectionModeUI::currentConnection->fromActor : ConnectionModeUI::currentConnection->toActor;
		ConnectionModeUI::currentConnection = nullptr;
		boardingLineNodeModeUI::currentBoardingLineNode = nullptr;
		GEditor->SelectNone(true, true);
		GEditor->SelectActor(currentNode, true, true);
	}


	ManagerModeUI::moveCameraToPosition(currentNode->GetActorLocation());
	GEditor->EndTransaction();
	return FReply::Handled();


}
FReply NodeModeUI::deleteCurrentNode()
{
	GEditor->BeginTransaction(FText::FromString("delete node"));
	GEditor->SelectNone(true, true);
	if (currentNode && ManagerModeUI::wayPointMGG)
	{
		currentNode->Destroy();
		currentNode = nullptr;

		//ManagerModeUI::wayPointMGG->Recalculate();
	}
	GEditor->EndTransaction();
	return FReply::Handled();
}
FReply NodeModeUI::exitNodeMode()
{
	GEditor->BeginTransaction(FText::FromString("unselected"));
	GEditor->SelectNone(true, true);
	addButtonLabel->SetText(FText::FromString("Enter Add mode"));
	NodeModeUI::isInAddMode = false;
	if (currentNode)
	{
		currentNode->SetConnectionsColor(FVector(0, 1, 0));
	}
	currentNode = nullptr;
	ConnectionModeUI::currentConnection = nullptr;
	boardingLineNodeModeUI::currentBoardingLineNode = nullptr;
	GEditor->EndTransaction();
	return FReply::Handled();
}
bool NodeModeUI::nodeManagerValidator()
{

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

			return (FoundActors.Num() == 1);
		}
		else
		{

			return false;
		}

	}
}
void NodeModeUI::endDragDrop()
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
				createNode(OutHit.ImpactPoint, ViewportClient->GetWorld());




			}
		}

		//create

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("out of range"));
	}

}
TSharedRef<SWidget> NodeModeUI::makeNodeButton(FText InLabel, const ANSICHAR* imageName)
{
	const FSlateBrush* brosh = AIEditorStyle::Get()->GetBrush(imageName);
	return SNew(SButton).IsEnabled_Static(&nodeManagerValidator)
		.Text(InLabel)

		.OnReleased_Static(&endDragDrop)
		.Content()
		[
			SNew(SImage).Image(brosh)
		];

}
EVisibility NodeModeUI::nodeDataModeToggle()
{
	if (NodeModeUI::isInAddMode)
	{
		return EVisibility::Collapsed;
	}
	else
	{

		return EVisibility::Visible;
	}


}
EVisibility NodeModeUI::nodeADDDataModeToggle()
{
	if (NodeModeUI::isInAddMode)
	{
		return EVisibility::Visible;
	}
	else
	{

		return EVisibility::Collapsed;
	}


}