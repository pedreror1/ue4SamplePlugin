// Fill out your copyright notice in the Description page of Project Settings.

#include "Classes/AINodeConnecter.h"
#include "ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "../Classes/AIBaseNode.h"

// Sets default values
AAINodeConnecter::AAINodeConnecter()
{
	
	static ConstructorHelpers::FObjectFinder<UParticleSystem> UPS(TEXT("UParticleSystem'/Game/AIEditorAssets/nodeConnection'"));
	beam = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ArbitraryParticleName"));
	SetActorHiddenInGame(true);
 	if (UPS.Succeeded() && GetWorld())
	{
		beam->Template = UPS.Object;
		if (beam)
		{
			beam->bAutoActivate = true;
			beam->SetHiddenInGame( false);
			
			beam->RegisterComponentWithWorld(GetWorld());
			RootComponent = beam;

		}
	}
	FRotator objectRotation(0, 0, 0); //in degrees
	FVector objectScale(1, 1, 1);
 	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAINodeConnecter::BeginPlay()
{
	Super::BeginPlay();
	
}
// Called every frame
void AAINodeConnecter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AAINodeConnecter::RemoveConnection()
{
	if (fromActor && toActor)
	{
		fromActor->connections.Remove(this);
		fromActor->nearNodes.Remove(toActor);
		Destroy();
	}
}
void AAINodeConnecter::setupColor(FVector color)
{
	if (!dynamicMat)
	{
		dynamicMat = UMaterialInstanceDynamic::Create(beam->GetMaterial(0), this);
	}
	if (dynamicMat)
	{
		dynamicMat->SetVectorParameterValue("connectorColor", color);
		beam->SetMaterial(0, dynamicMat);
	}
}
void AAINodeConnecter::UpdatePosition(AActor* from, AActor* to)
{
	fromActor = StaticCast<AAIBaseNode*>( from);
	toActor = StaticCast<AAIBaseNode*>(to);
	cost = FVector::Dist(from->GetActorLocation(), to->GetActorLocation());
	beam->SetActorParameter("source", fromActor);
	beam->SetActorParameter("target", toActor);
}
FString AAINodeConnecter::getFromString()
{
	return("FROM:             " +fromActor->GetActorLabel());
}
FString AAINodeConnecter::getToString()
{
	return("TO:               "+toActor->GetActorLabel());
}
FString AAINodeConnecter::getCostString()
{
	return("Cost:             "+FString::FromInt((int)cost));
}

