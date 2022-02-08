#pragma once

#include "GameFramework/Actor.h"
#include "AIBaseNode.h"
#include "EventObject.h"
#include "DrawDebugHelpers.h"
#include <iostream>
#include <fstream>
#include "AIEditorManager.generated.h"

UCLASS()
class  AIEditorManager : public AActor
{
	GENERATED_UCLASS_BODY()
	AIEditorManager();

public:
	// Sets default values for this actor's properties
	UPROPERTY(VisibleAnywhere)
	TArray<AAIBaseNode*> nodes;
	UPROPERTY(VisibleAnywhere)
	TArray<AEventObject*> Events;
	bool hasEventUI;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	bool isResseting;
	

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	//bool SaveFile();
	TArray<FString> loadFile();
	void removeNode(AAIBaseNode* node);
	void RemoveNodeGlobal(AAIBaseNode* node);
	void Recalculate();
	void Reset();
	FString levelFileLocation = "/Plugins/AIEditor/AIEditorAssets/";
	FString levelID;
	FString levelDirection;
	FString posX;
	FString posY;
	FString posZ;
	FString width;
	FString height;
	FString depth;
	FString columns;
	FString rows;
	FString layers;

 
	

};