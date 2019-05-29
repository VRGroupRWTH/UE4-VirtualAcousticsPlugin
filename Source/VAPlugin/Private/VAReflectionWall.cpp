// Fill out your copyright notice in the Description page of Project Settings.

#include "VAReflectionWall.h"
#include "VAUtils.h"
#include "EngineUtils.h"
#include "Engine/StaticMeshActor.h"

// Sets default values
AVAReflectionWall::AVAReflectionWall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	normalVec = FVector(1, 0, 0);
	supportVec = FVector(0, 0, 0);
	d = 0;

	alreadyComputed = false;

	// planeComp = CreateDefaultSubobject<UBoxComponent>(TEXT("PlaneComp"));
	// planeComp->InitBoxExtent(FVector(1, 100, 100));

}

FVector AVAReflectionWall::getNormalVec()
{
	if (!alreadyComputed) {
		computePlaneData();
	}
	return normalVec;
}

FVector AVAReflectionWall::getSupportVec()
{
	if (!alreadyComputed) {
		computePlaneData();
	}
	return supportVec;
}

float AVAReflectionWall::getD()
{
	if (!alreadyComputed) {
		computePlaneData();
	}
	return d;
}

float AVAReflectionWall::getR()
{
	return vR;
}

void AVAReflectionWall::computePlaneData()
{
	normalVec = GetTransform().GetRotation().Rotator().Vector();
	supportVec = GetTransform().GetLocation();

	d = FVector::DotProduct(normalVec, supportVec);


	alreadyComputed = true;
}

void AVAReflectionWall::spawnSphere(FVector pos, FRotator rot)
{
	AStaticMeshActor *Mesh = nullptr;
	int counter = 0;
	for (TActorIterator<AStaticMeshActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		Mesh = *ActorItr;
		if (Mesh->GetName() == "ReflectionSphere") {
			Mesh->SetActorLocationAndRotation(pos, rot.Quaternion());
			counter++;
		}
	}
	FString text = "Counter: ";
	text.Append(FString::FromInt(counter));
	VAUtils::openMessageBox(text);

	// SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereMeshz"));
	// //this->SphereMesh->AttachTo(GetRootComponent());
	// SetRootComponent(SphereMesh);
	// 
	// static ConstructorHelpers::FObjectFinder<UStaticMesh>SphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	// this->SphereMesh->SetStaticMesh(SphereMeshAsset.Object);


	// float size = 50.0f;
	// 
	// sphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	// sphereComp->SetAbsolute(true, true, true);
	// sphereComp->InitSphereRadius(size);
	// sphereComp->SetWorldLocation(pos);
	// //sphereComp->SetVisibility(true);
	// 
	// dirComp = CreateDefaultSubobject<USphereComponent>(TEXT("dirComp"));
	// dirComp->SetAbsolute(true, true, true);
	// dirComp->InitSphereRadius(size / 4.0f);
	// dirComp->SetWorldLocation(pos + size*rot.Vector());
	// // dirComp->SetVisibility(true);

	
}

// Called when the game starts or when spawned
void AVAReflectionWall::BeginPlay()
{
	Super::BeginPlay();
	
	if (!alreadyComputed) {
		computePlaneData();
	}

}

// Called every frame
void AVAReflectionWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

