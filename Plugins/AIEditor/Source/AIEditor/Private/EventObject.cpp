// Fill out your copyright notice in the Description page of Project Settings.

#include "EventObject.h"
#include "ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "DrawDebugHelpers.h"
#include "AIEditorManager.h"
#include "Engine/Selection.h"
#include "AIEditorEdMode.h"
// Sets default values
AEventObject::AEventObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need i t.
	PrimaryActorTick.bCanEverTick = true;
	CustomMesh = CreateDefaultSubobject<UProceduralMeshComponent>("Custom EV Mesho");
	SetActorHiddenInGame(true);
	CustomMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SetRootComponent(CustomMesh);
	CustomMesh->bUseAsyncCooking = true;
	GenerateEventObjMesh();
}

// Called when the game starts or when spawned
void AEventObject::BeginPlay()
{
	//Super::BeginPlay();
	
}

// Called every frame
void AEventObject::Tick(float DeltaTime)
{
	 

}

void AEventObject::AddTriangle(int32 V1, int32 V2, int32 V3)
{
	Triangles.Add(V1);
	Triangles.Add(V2);
	Triangles.Add(V3);
}
FVector getPointInCirclee(float r, float ang, float height = 25)
{
	FVector pos = FVector();
	pos.X = r * FMath::Cos(ang);
	pos.Y = r * FMath::Sin(ang);
	pos.Z = height;
	return pos;
}

void AEventObject::GenerateEventObjMesh()
{
	for (int i = 0; i < 5; i++)
	{
		Vertices.Add(getPointInCirclee(15, FMath::DegreesToRadians((360 / 5)*i), 25)); //upper left - 1

	}
	for (int i = 0; i < 5; i++)
	{
		Vertices.Add(getPointInCirclee(15, FMath::DegreesToRadians((360 / 5)*i), -25)); //upper left - 1

	}
	//Center
	Vertices.Add(location);

	for (int j = 5 - 1; j >= 0; j--)
	{
		int y = j - 1;
		AddTriangle(j, (y >= 0) ? y : 5 - 1, Vertices.Num() - 1);
	}
	for (int j = (5 * 2) - 1; j >= 5; j--)
	{
		int y = j - 1;
		AddTriangle(j, (y >= 5) ? y : (5 * 2) - 1, j - 5);
	}

	for (int j = 5 - 1; j >= 0; j--)
	{
		int y = j - 1;
		int z = j + 5 - 1;
		AddTriangle((z >= 5) ? z : (5 * 2) - 1, (y >= 0) ? y : 5 - 1, j);
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
	static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/AIEditorAssets/eventObjMat'"));

	if (Material.Object != NULL)
	{
		auto TheMaterial = (UMaterial*)Material.Object;


		CustomMesh->SetMaterial(0, TheMaterial);
	}


}

void AEventObject::updateRadius(float newRadius)
{
	Radius = newRadius*10;
	this->SetActorScale3D(FVector(newRadius, newRadius, 1.0));
}

void AEventObject::OnConstruction(const FTransform & Transform)
{
	location = GetActorLocation();
}
void AEventObject::RemoveAllBoardingPositions()
{
	if (boardingLinePositions.Num() > 0)
	{
		for (int i = 0; i < boardingLinePositions.Num(); i++)
		{
		
				AboardingLineNode* BLM = boardingLinePositions[i];
				if (BLM)
				{
					 
					BLM->Destroy();

				}
			
		}
		boardingLinePositions.Empty();

	}
	if (unBoardingLinePositions.Num() > 0)
	{
		for (int i = 0; i < unBoardingLinePositions.Num(); i++)
		{

			AboardingLineNode* BLM = unBoardingLinePositions[i];
			if (BLM)
			{

				BLM->Destroy();

			}

		}
		unBoardingLinePositions.Empty();

	}
}
void AEventObject::Destroyed()
{
	RemoveAllBoardingPositions();

}

