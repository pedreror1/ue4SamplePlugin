// Fill out your copyright notice in the Description page of Project Settings.

#include "AIBaseNode.h"
#include "ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "DrawDebugHelpers.h"
#include "AIEditorManager.h"
#include "Engine/Selection.h"
#include "AIEditorEdMode.h"

// Sets default values
AAIBaseNode::AAIBaseNode()
{
	USelection::SelectObjectEvent.AddUObject(this, &AAIBaseNode::OnObjectSelected);

	 
		// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
		PrimaryActorTick.bCanEverTick = true;
		CustomMesh = CreateDefaultSubobject<UProceduralMeshComponent>("Custom Mesho");
		SetActorHiddenInGame(true);
		CustomMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		SetRootComponent(CustomMesh);
		CustomMesh->bUseAsyncCooking = true;
		GenerateWaypointMesh();
		
	 
}

AAIBaseNode::~AAIBaseNode()
{
	
}

void AAIBaseNode::AddTriangle(int32 V1, int32 V2, int32 V3)
{
	Triangles.Add(V1);
	Triangles.Add(V2);
	Triangles.Add(V3);
}
FVector getPointInCircle(float r, float ang, float height = 25)
{
	FVector pos = FVector();
	pos.X = r * FMath::Cos(ang);
	pos.Y = r * FMath::Sin(ang);
	pos.Z = height;
	return pos;
}
void AAIBaseNode::GenerateWaypointMesh()
{
	for (int i = 0; i < numSlices; i++)
	{
		Vertices.Add(getPointInCircle(15, FMath::DegreesToRadians((360/numSlices)*i),25)); //upper left - 1

	}
	for (int i = 0; i < numSlices; i++)
	{
		Vertices.Add(getPointInCircle(15, FMath::DegreesToRadians((360 / numSlices)*i), -25)); //upper left - 1

	}
	//Center
	Vertices.Add(location);
	
	for (int j = numSlices - 1; j >= 0; j--)
	{
		int y = j - 1;
		AddTriangle(j, (y >= 0) ? y : numSlices - 1, Vertices.Num() - 1);
	}
	for (int j = (numSlices * 2) - 1; j >= numSlices; j--)
	{
		int y = j - 1;
		AddTriangle(j, (y >= numSlices) ? y : (numSlices * 2) - 1, j-numSlices);
	}
	 
	for (int j = numSlices  - 1; j >= 0; j--)
	{
		int y = j - 1;
		int z = j + numSlices - 1;
		AddTriangle((z>=numSlices)?z:(numSlices*2)-1, (y >= 0) ? y : numSlices  - 1 ,j);
	}
	//Back face of cube
 
	 

	TArray<FLinearColor> VertexColors;
	VertexColors.Add(FLinearColor(0.f, 0.f, 1.f));
	VertexColors.Add(FLinearColor(1.f, 0.f, 0.f));
	VertexColors.Add(FLinearColor(1.f, 0.f, 0.f));
	VertexColors.Add(FLinearColor(0.f, 1.f, 0.f));
	VertexColors.Add(FLinearColor(0.5f, 1.f, 0.5f));
	VertexColors.Add(FLinearColor(0.f, 1.f, 0.f));
	VertexColors.Add(FLinearColor(1.f, 1.f, 0.f));
	VertexColors.Add(FLinearColor(0.f, 1.f, 1.f));

	CustomMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, TArray<FVector>(), TArray<FVector2D>(), VertexColors, TArray<FProcMeshTangent>(), true);
	//CustomMesh->SetMaterial(0,UMaterialInterface::get)
	static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/AIEditorAssets/wireframe_mat'"));

	if (Material.Object != NULL)
	{
		auto TheMaterial = (UMaterial*)Material.Object;
		 

		CustomMesh->SetMaterial(0, TheMaterial);
	}
	

}
//Called when the game starts or when spawned
void AAIBaseNode::BeginPlay()
{
	//UE_LOG(LogTemp,Warning,TEXT("loc %f %f %f"),this->ActorToWorld().GetLocation().X, this->ActorToWorld().GetLocation().Y, this->ActorToWorld().GetLocation().Z)
	 
	//getNearbyWaypoints();
	
}

void AAIBaseNode::OnObjectSelected(UObject * Object)
{
	if (Object == this)
	{
		hasConnectionUI = false;
	}
	else if (!IsSelected())
	{
	}
}
void AAIBaseNode::AddObjectToConnections(AAIBaseNode * nodeToAdd)
{
		if (!nearNodes.Contains(nodeToAdd))
		{
			hasConnectionUI = false;
			nearNodes.Add(nodeToAdd);
			RecalculateConnections();
		}
	
}
void AAIBaseNode::ResetNode()
{
	nearNodes.Empty();
	hasConnectionUI = false;
}
void AAIBaseNode::RemoveNode(AAIBaseNode * nodeToRemove)
{
	if (nearNodes.Contains(nodeToRemove))
	{
		hasConnectionUI = false;
		nearNodes.Remove(nodeToRemove);
		nodeToRemove->RemoveNode(this);
		for (int i = 0; i < connections.Num(); i++)
		{
			if (connections[i]->toActor == nodeToRemove)
			{
				connections[i]->Destroy();
				connections.RemoveAt(i);
			}
		}

	}
}
void AAIBaseNode::RecalculateConnections()
{
	if (connections.Num()>0 &&  connections.Num() == nearNodes.Num())
	{
		for (int i = 0; i < nearNodes.Num(); i++)
		{
			if (nearNodes[i] && connections[i])
			{
				connections[i]->UpdatePosition(this, nearNodes[i]);
			}
		}
	}
	else
	{
		for (int i = connections.Num(); i < nearNodes.Num(); i++)
		{
			if (nearNodes[i])
			{
				AActor* newactor = GEditor->AddActor(GetWorld()->GetCurrentLevel(), AAINodeConnecter::StaticClass(),
					GetActorTransform(), true, RF_Public | RF_Transactional);
				newactor->SetActorLabel("Connection From " + GetActorLabel() + FString(" TO ") + nearNodes[i]->GetActorLabel());
				newBeam = StaticCast<AAINodeConnecter*>(newactor);
				FVector Offset = (nearNodes[i]->nearNodes.Contains(this)) ? FVector(0, 0, 25) : FVector();
				newBeam->SetActorLocation(GetActorLocation() + Offset);
				if (newBeam)
				{
					if (!connections.Contains(newBeam))
					{
						newBeam->UpdatePosition(this, nearNodes[i]);
						connections.Add(newBeam);
					}
				}
			}
		}
	}
}
void AAIBaseNode::SetConnectionsColor(FVector color)
{
	
	for (int i = 0; i < connections.Num(); i++)
	{
		connections[i]->setupColor(color);
	}
}
// Called every frame
void AAIBaseNode::Tick(float DeltaTime)
{
	//Super::Tick(DeltaTime);
	//if (recalculate)
	//	getNearbyWaypoints();
}
void AAIBaseNode::OnConstruction(const FTransform & Transform)
{
	location = GetActorLocation();
	RecalculateConnections();
}
void AAIBaseNode::Destroyed()
{
	for (int i = 0; i < connections.Num(); i++)
	{
		connections[i]->Destroy();
	}
	if (mg)
	{
		AIEditorManager* m = static_cast<AIEditorManager*>(mg);
		m->removeNode(this);		
	}
}



