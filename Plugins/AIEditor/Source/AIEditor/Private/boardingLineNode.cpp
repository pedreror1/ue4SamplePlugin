// Fill out your copyright notice in the Description page of Project Settings.

#include "boardingLineNode.h"
 AboardingLineNode::AboardingLineNode()
{
	PrimaryActorTick.bCanEverTick = true;
	CustomMesh = CreateDefaultSubobject<UProceduralMeshComponent>("Custom EV Mesho");
	SetActorHiddenInGame(true);
	CustomMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SetRootComponent(CustomMesh);
	CustomMesh->bUseAsyncCooking = true;
	GenerateEventObjMesh();
}

AboardingLineNode::~AboardingLineNode()
{
}

void AboardingLineNode::AddTriangle(int32 V1, int32 V2, int32 V3)
{
	Triangles.Add(V1);
	Triangles.Add(V2);
	Triangles.Add(V3);
}

void AboardingLineNode::GenerateEventObjMesh()
{
	Vertices.Add(FVector(0, -10, 0));  
	Vertices.Add(FVector(0, -10, 10));  
	Vertices.Add(FVector(0, 10, 0));  
	Vertices.Add(FVector(0, 10, 10));  

	Vertices.Add(FVector(10, -10, 0));  
	Vertices.Add(FVector(10, -10, 10));  

	Vertices.Add(FVector(10, 10, 10)); 
	Vertices.Add(FVector(10, 10, 0)); 

 	AddTriangle(0, 2, 3);
	AddTriangle(3, 1, 0);

 	AddTriangle(0, 1, 4);
	AddTriangle(4, 1, 5);

 	AddTriangle(4, 5, 7);
	AddTriangle(7, 5, 6);

 	AddTriangle(7, 6, 3);
	AddTriangle(3, 2, 7);

 	AddTriangle(1, 3, 5);
	AddTriangle(6, 5, 3);

 	AddTriangle(2, 0, 4);
	AddTriangle(4, 7, 2);

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
	static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/AIEditorAssets/eventObjMat'"));

	if (Material.Object != NULL)
	{
		LineUpMaterial = Material.Object;
		
		
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> Material1(TEXT("Material'/Game/AIEditorAssets/getOffPosMat'"));
	if (Material1.Object != NULL)
	{
		
		getDownMaterial = Material1.Object;


	}
}

 

 

void AboardingLineNode::OnConstruction(const FTransform & Transform)
{
	location = GetActorLocation();
	Moved = true;

}

void AboardingLineNode::Destroyed()
{
}

void AboardingLineNode::UpdateMat()
{
	if (isUnboardingNode)
	{
		CustomMesh->SetMaterial(0, getDownMaterial);
	}
	else
	{
		CustomMesh->SetMaterial(0, LineUpMaterial);
	}
	
}
