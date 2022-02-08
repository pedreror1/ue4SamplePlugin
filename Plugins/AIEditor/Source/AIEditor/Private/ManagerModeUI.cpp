// Fill out your copyright notice in the Description page of Project Settings.

#include "ManagerModeUI.h"
TSharedPtr<FString>						  ManagerModeUI::selectedItem;
TSharedPtr<STextBlock>					   ManagerModeUI::comboData;
TSharedPtr<SComboBox<TSharedPtr<FString>>>  ManagerModeUI::selectorComboBox;
TSharedPtr<SEditableTextBox>				   ManagerModeUI::xPos;
TSharedPtr<SEditableTextBox>				   ManagerModeUI::yPos;
TSharedPtr<SEditableTextBox>				   ManagerModeUI::zPos;
TSharedPtr<STextBlock>  ManagerModeUI::toNodeName;
AAIBaseNode*  ManagerModeUI::currentNode;
FVector  ManagerModeUI::lastPos;
bool  ManagerModeUI::isInAddMode;
AAINodeConnecter*  ManagerModeUI::currentConnection;
TSharedPtr<SEditableTextBox>				   ManagerModeUI::levelID;
TSharedPtr<SEditableTextBox>				   ManagerModeUI::levelLocation;

 
ManagerModeUI::Mode currentMode;
TSharedPtr<SEditableTextBox>				  ManagerModeUI::width;
TSharedPtr<SEditableTextBox>				  ManagerModeUI::height;
TSharedPtr<SEditableTextBox>				  ManagerModeUI::depth;
TSharedPtr<SEditableTextBox>				  ManagerModeUI::columns;
TSharedPtr<SEditableTextBox>				  ManagerModeUI::rows;
TSharedPtr<SEditableTextBox>				  ManagerModeUI::Layers;
TSharedPtr<FString>						      ManagerModeUI::currentDirection;
TSharedPtr<STextBlock>					      ManagerModeUI::directionComboData;
TSharedPtr<SComboBox<TSharedPtr<FString>>>    ManagerModeUI::directionComboBox;
ManagerModeUI::Mode  ManagerModeUI::currentMode;
AIEditorManager* ManagerModeUI::wayPointMGG = nullptr;

ManagerModeUI::ManagerModeUI()
{
}

ManagerModeUI::~ManagerModeUI()
{
}

FReply ManagerModeUI::SetupStartPos()
{
	GEditor->BeginTransaction(FText::FromString("SetupStarPos"));

	FLevelEditorViewportClient* ViewportClient = (FLevelEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();
	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
		ViewportClient->Viewport,
		ViewportClient->GetScene(),
		ViewportClient->EngineShowFlags));
	FSceneView* View = ViewportClient->CalcSceneView(&ViewFamily);
	xPos->SetText(FText::FromString(FString::FromInt(View->ViewLocation.X)));
	yPos->SetText(FText::FromString(FString::FromInt(View->ViewLocation.Y)));
	zPos->SetText(FText::FromString(FString::FromInt(View->ViewLocation.Z)));
	GEditor->EndTransaction();
	return FReply::Handled();
}

UWorld * ManagerModeUI::getLocalWorld()
{
	FLevelEditorViewportClient* ViewportClient = (FLevelEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();

	UWorld* myWorld = ViewportClient->GetWorld();
	if (myWorld)
	{
		return myWorld;
	}
	else
	{
		return nullptr;
	}
}

FText ManagerModeUI::getActorName(AActor * actor)
{
	if (actor)
	{
		return FText::FromString(actor->GetActorLabel());
	}
	else
	{
		return FText::FromString("No Node Selected");
	}
}

FReply ManagerModeUI::moveCameraToPosition(FVector destination)
{
	GEditor->BeginTransaction(FText::FromString("set camera position"));
	FLevelEditorViewportClient* ViewportClient = (FLevelEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();

	if (ViewportClient)
	{

		ViewportClient->SetViewLocation(destination + FVector(0, -10, 45));
	}
	GEditor->EndTransaction();
	return FReply::Handled();
} 


