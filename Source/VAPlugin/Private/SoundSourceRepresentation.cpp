// Fill out your copyright notice in the Description page of Project Settings.

#include "SoundSourceRepresentation.h"


#include "Engine/StaticMeshActor.h"

// Sets default values
ASoundSourceRepresentation::ASoundSourceRepresentation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// reflection Sphere
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

bool ASoundSourceRepresentation::setPos(FVector pos)
{
	sphereComp->SetWorldLocation(pos);
	return false;
}

bool ASoundSourceRepresentation::setRot(FRotator rot)
{
	sphereComp->SetWorldRotation(rot);
	return false;
}

bool ASoundSourceRepresentation::setVis(bool visibility)
{
	sphereComp->SetVisibility(visibility);
	return false;
}

// Called when the game starts or when spawned
void ASoundSourceRepresentation::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASoundSourceRepresentation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

