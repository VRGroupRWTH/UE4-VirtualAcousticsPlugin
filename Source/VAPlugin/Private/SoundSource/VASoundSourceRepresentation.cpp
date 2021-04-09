// Fill out your copyright notice in the Description page of Project Settings.

#include "SoundSource/VASoundSourceRepresentation.h"
#include "VAPlugin.h"

// For ConstructorHelpers and SphereMesh
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/CollisionProfile.h"

#include "Engine/StaticMeshActor.h"

// Sets default values
AVASoundSourceRepresentation::AVASoundSourceRepresentation()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(FName("SphereComp"));
	SphereComponent->bHiddenInGame = true;
	SphereComponent->Mobility = EComponentMobility::Movable;
	RootComponent = SphereComponent;


	SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/VAPlugin/Shape_Cone"));
	if (SphereMeshAsset.Succeeded())
	{
		SphereMesh->SetStaticMesh(SphereMeshAsset.Object);
		SphereMesh->SetupAttachment(RootComponent);
		SphereMesh->SetRelativeLocation(FVector(80.0f, 0.0f, 0.0f));
		SphereMesh->SetRelativeRotation(FRotator(0.0f, 90.0f, 90.0f));
		SphereMesh->SetWorldScale3D(FVector(0.8f));
    SphereMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	}
	SetVisibility(false);
}


void AVASoundSourceRepresentation::BeginPlay()
{
	Super::BeginPlay();
}


void AVASoundSourceRepresentation::SetPosition(const FVector Pos)
{
	SphereComponent->SetWorldLocation(Pos);
}

void AVASoundSourceRepresentation::SetRotation(const FRotator Rot)
{
	SphereComponent->SetWorldRotation(Rot);
}

void AVASoundSourceRepresentation::SetVisibility(const bool bVisibility)
{
	if (SphereMesh != nullptr)
	{
		SphereMesh->SetVisibility(bVisibility);
	}
}
