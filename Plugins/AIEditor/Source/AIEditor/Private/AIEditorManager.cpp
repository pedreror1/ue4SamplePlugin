
#include "AIEditorManager.h"
#include "Misc/FileHelper.h"
#include "Core/Public/HAL/PlatformFilemanager.h"

using namespace std;
 
AIEditorManager::AIEditorManager(const FObjectInitializer& ObjectInitializer)
{
	isResseting = false;
	
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.SetTickFunctionEnable(true);
}

void AIEditorManager::BeginPlay()
{
}

void AIEditorManager::Tick(float DeltaTime)
{
	float c = DeltaTime;

}
 
TArray<FString> AIEditorManager::loadFile()
{
	Reset();
	TArray<FString>result;
	FString SaveDirectory = FPaths::ProjectDir();
	FString direction = FString("_TH_") + levelDirection;
	if (direction == "_TH_NONE")
		direction = "";
	FString FileName = levelID + direction;
	//FString FileName = levelID + FString("_TH_") + levelDirection;
	FString AbsoluteFilePath = SaveDirectory + levelFileLocation + FileName + FString(".map");
	ifstream myfile;
	myfile.open(*AbsoluteFilePath, fstream::in);
	if (myfile.is_open())
	{	
		while(myfile)
		{
			char* tmp = new char[512];
			myfile.getline(tmp, 512);
			result.Add(tmp);
		}

		myfile.close();
	}

	return result;
}
void AIEditorManager::removeNode(AAIBaseNode* node)
{
	if(!isResseting)
	nodes.Remove(node);
	RemoveNodeGlobal(node);
}
void AIEditorManager::Recalculate()
{
	for (int i = 0; i < nodes.Num(); i++)
	{
		if (nodes[i])
		{
			nodes[i]->RecalculateConnections();
		}
	}
}
void AIEditorManager::RemoveNodeGlobal(AAIBaseNode* node)
{
	FlushPersistentDebugLines(GetWorld());
	for (int i = 0; i < nodes.Num(); i++)
	{
		if (nodes[i])
		{ 			 
			nodes[i]->RemoveNode(node);
		}
	}
}

void AIEditorManager::Reset()
{
	isResseting = true;
	for (AAIBaseNode* nodo : nodes)
	{
		nodo->Destroy(true);
	}
	for (AEventObject* nod : Events)
	{
		
		if (nod->FlagID == "2")
		{
			for (int i = 0; i < nod->boardingLinePositions.Num(); i++)
			{
				nod->boardingLinePositions[i]->Destroy();
			}
			for (int i = 0; i < nod->unBoardingLinePositions.Num(); i++)
			{
				nod->unBoardingLinePositions[i]->Destroy();
			}
		}
		nod->Destroy(true);
	}
	 
	nodes.Empty();
	Events.Empty();
	FlushPersistentDebugLines(GetWorld());
	isResseting = false;
}

void AIEditorManager::OnConstruction(const FTransform & Transform)
{

}

 


