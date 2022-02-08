// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "UObject/UObjectGlobals.h"
#include "ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"
 #include "boardingLineNode.generated.h"
class AEventObject;

/**
 * 
 */
UCLASS()
class AIEDITOR_API AboardingLineNode : public AActor
{
	GENERATED_BODY()

public:
	AboardingLineNode();
	~AboardingLineNode();
 
	 
	UPROPERTY(EditAnywhere)
		FVector location;
	 UPROPERTY(EditAnywhere)
		FString FlagID = "0";
	 


	UPROPERTY()

	UProceduralMeshComponent* CustomMesh;
	TArray<FVector> Vertices;
	TArray<int32>Triangles;
	 
	float Angle;
	void AddTriangle(int32 V1, int32 V2, int32 V3);
	void GenerateEventObjMesh();
 	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Destroyed();
	FVector direction;
	bool Moved = false;
	bool isUnboardingNode;
	AEventObject* eventParent;
	UMaterial* LineUpMaterial;
		UMaterial* getDownMaterial;
	void UpdateMat();
	 
};
