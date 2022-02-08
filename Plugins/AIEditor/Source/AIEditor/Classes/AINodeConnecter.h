// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "AINodeConnecter.generated.h"

class AAIBaseNode;
UCLASS()
class AIEDITOR_API AAINodeConnecter : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAINodeConnecter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere)
		UParticleSystem* connectionPS;
	UPROPERTY(EditAnywhere)
		UParticleSystemComponent* beam;

	AAIBaseNode* fromActor;
	AAIBaseNode* toActor;
	UMaterialInstanceDynamic* dynamicMat;
	void setupColor(FVector color);
	float cost;
	void UpdatePosition(AActor* from, AActor* to);
	void RemoveConnection();
	FString getCostString();

	FString getFromString();
	FString getToString();

};
