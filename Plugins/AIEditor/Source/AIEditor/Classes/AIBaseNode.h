// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "AINodeConnecter.h"
#include "UObject/UObjectGlobals.h"
//Source/ProceduralMeshComponent/Private/ProceduralMeshComponentPluginPrivate.h"
#include "AIBaseNode.generated.h"


UCLASS()
class AIEDITOR_API AAIBaseNode : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAIBaseNode();
	~AAIBaseNode();	
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", ClampMax = "4"))
		float fovMultiplier = 3.0;
	UPROPERTY(EditAnywhere)
		bool recalculate = true;
	UPROPERTY(EditAnywhere)
		float maxDistance=5000;
	UPROPERTY(EditAnywhere)
		FVector location;
	UPROPERTY(EditAnywhere)
		int numSlices = 32;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UProceduralMeshComponent* CustomMesh;
	TArray<FVector> Vertices;
	TArray<int32>Triangles;
	UPROPERTY(VisibleAnywhere)
	TArray<AAIBaseNode*>nearNodes;

	bool hasConnectionUI;
	
	AActor* mg;
	void AddTriangle(int32 V1, int32 V2, int32 V3);
	void GenerateWaypointMesh();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Destroyed();

	//void getNearbyWaypoints(int mD=1100, int mN=5);
	void OnObjectSelected(UObject* Object);
	void AddObjectToConnections(AAIBaseNode* nodeToAdd);
	void ResetNode();
	void RemoveNode(AAIBaseNode* nodeToRemove);
	void RecalculateConnections();
	void SetConnectionsColor(FVector color);
	AAINodeConnecter* newBeam;
	UPROPERTY(VisibleAnywhere)
	TArray<AAINodeConnecter*> connections;
	
};
