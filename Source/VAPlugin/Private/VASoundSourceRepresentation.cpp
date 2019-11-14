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
	PrimaryActorTick.bCanEverTick = true;

	// reflection Sphere
	// sphereComp2 = CreateDefaultSubobject<USphereComponent>(FName("ShapeComponent"));
	// sphereComp2->bHiddenInGame = false;
	// sphereComp2->Mobility = EComponentMobility::Movable;
	// RootComponent = sphereComp2;
	// 
	// sphereMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	// sphereMesh2->AttachTo(RootComponent);
	// 
	// // static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/VAPlugin/Shape_Cone.Shape_Cone"));
	// 
	// static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Game/StarterContent/Shapes/Shape_Cone.Shape_Cone"));
	// 
	// // FString BaseDir = IPluginManager::Get().FindPlugin("VAPlugin")->GetBaseDir();
	// // FString pathFile = FPaths::Combine(*BaseDir, TEXT("Content/Shape_Cone.Shape_Cone"));
	// // BaseDir.Append("/Content/Shape_Cone.Shape_Cone");
	// // static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(*BaseDir);
	// 
	// if (SphereMeshAsset.Succeeded()) {
	// 	sphereMesh2->SetStaticMesh(SphereMeshAsset.Object);
	// 	sphereMesh2->SetRelativeLocation(FVector(80.0f, 0.0f, 0.0f));
	// 	sphereMesh2->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	// 	sphereMesh2->SetWorldScale3D(FVector(0.8f));
	// }
	// 
	// setVisibility(FVAPluginModule::isInDebugMode());

	sphereComp = CreateDefaultSubobject<USphereComponent>(FName("SphereComp"));
	sphereComp->bHiddenInGame = true;
	sphereComp->Mobility = EComponentMobility::Movable;
	RootComponent = sphereComp;

	sphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	sphereMesh->AttachTo(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Game/StarterContent/Shapes/Shape_Cone.Shape_Cone"));
	if (SphereMeshAsset.Succeeded()) {
		sphereMesh->SetStaticMesh(SphereMeshAsset.Object);
		sphereMesh->SetRelativeLocation(FVector(80.0f, 0.0f, 0.0f));
		sphereMesh->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
		sphereMesh->SetWorldScale3D(FVector(0.8f));
	}

}

bool AVASoundSourceRepresentation::setPos(FVector pos)
{
	sphereComp->SetWorldLocation(pos);
	return true;
}

bool AVASoundSourceRepresentation::setRot(FRotator rot)
{
	sphereComp->SetWorldRotation(rot);
	return true;
}

bool AVASoundSourceRepresentation::setVisibility(bool visibility)
{
	sphereComp->SetVisibility(true);
	// sphereMesh->SetVisibility(true);
	return true;
}

// Called when the game starts or when spawned
void AVASoundSourceRepresentation::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVASoundSourceRepresentation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

