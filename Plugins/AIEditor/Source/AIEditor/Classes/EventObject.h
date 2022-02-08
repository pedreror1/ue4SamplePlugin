// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
 #include "boardingLineNode.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "UObject/UObjectGlobals.h"
#include "EventObject.generated.h"
 
UCLASS()
class AIEDITOR_API AEventObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEventObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere)
		FVector location;
	UPROPERTY(EditAnywhere)
		float Radius=1;
	UPROPERTY(EditAnywhere)
		float MaxRadius=25;
	//UPROPERTY(EditAnywhere)
		//FString id ="0";
	UPROPERTY(EditAnywhere)
		FString FlagID = "0";
	UPROPERTY(EditAnywhere)
		FString nodeID = "0";


	UPROPERTY()
		UProceduralMeshComponent* CustomMesh;
	TArray<FVector> Vertices;
	TArray<int32>Triangles;
	TArray<AboardingLineNode*>boardingLinePositions;
	TArray<AboardingLineNode*>unBoardingLinePositions;

	AActor* mg;
	void AddTriangle(int32 V1, int32 V2, int32 V3);
	void GenerateEventObjMesh();
	void updateRadius(float newRadius);
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Destroyed();
	void RemoveAllBoardingPositions();



};
