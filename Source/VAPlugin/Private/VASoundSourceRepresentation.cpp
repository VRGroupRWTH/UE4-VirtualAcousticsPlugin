// Fill out your copyright notice in the Description page of Project Settings.

#include "VASoundSourceRepresentation.h"
#include "VAPlugin.h"

// For path
#include "Interfaces/IPluginManager.h"

#include "Engine/StaticMeshActor.h"

// Sets default values
AVASoundSourceRepresentation::AVASoundSourceRepresentation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	sphereComp = CreateDefaultSubobject<USphereComponent>(FName("SphereComp"));
	sphereComp->bHiddenInGame = true;
	sphereComp->Mobility = EComponentMobility::Movable;
	RootComponent = sphereComp;


	sphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/VAPlugin/Shape_Cone"));
	if (SphereMeshAsset.Succeeded()) {
		sphereMesh->SetStaticMesh(SphereMeshAsset.Object);
		sphereMesh->SetupAttachment(RootComponent);
		sphereMesh->SetRelativeLocation(FVector(80.0f, 0.0f, 0.0f));
		sphereMesh->SetRelativeRotation(FRotator(0.0f, 90.0f, 90.0f));
		sphereMesh->SetWorldScale3D(FVector(0.8f));
	}
	setVisibility(false);
}


void AVASoundSourceRepresentation::BeginPlay()
{
	Super::BeginPlay();
	
}

// TURNED OFF IN CONSTRUCTOR!
void AVASoundSourceRepresentation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AVASoundSourceRepresentation::setPos(FVector pos)
{
	sphereComp->SetWorldLocation(pos);
	return true;
}

bool AVASoundSourceRepresentation::setRot(FRotator rot)
{
	FQuat quat(rot);
	quat = quat * FQuat(FRotator(0,0,0));
	rot = FRotator(quat);
	sphereComp->SetWorldRotation(rot);
	return true;
}

bool AVASoundSourceRepresentation::setVisibility(bool visibility)
{
	if (sphereMesh != nullptr) {
		sphereMesh->SetVisibility(visibility);
	}
	return true;
}

