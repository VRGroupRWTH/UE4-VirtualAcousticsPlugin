// Fill out your copyright notice in the Description page of Project Settings.

#include "VAReflectionWall.h"
#include "VAUtils.h"
#include "VAPlugin.h"
#include "EngineUtils.h"
#include "Engine/StaticMeshActor.h"
#include "Components/BoxComponent.h"


// Sets default values
AVAReflectionWall::AVAReflectionWall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// normalVec = FVector(1, 0, 0);
	// supportVec = FVector(0, 0, 0);
	// d = 0;

	alreadyComputed = false;
	
	planeComp = CreateDefaultSubobject<UBoxComponent>(TEXT("PlaneComp"));
	planeComp->InitBoxExtent(FVector(1, 100, 100));
	planeComp->SetVisibility(true);
	// planeComp->SetRelativeLocation(FVector(0, 0, 0));

	// planeComp = CreateDefaultSubobject<UBoxComponent>(TEXT("PlaneComp"));q
	// planeComp->InitBoxExtent(FVector(100, 100, 100));
	// planeComp->SetVisibility(true);
	// planeComp->SetRelativeLocation(FVector(0, 0, 0));
	// 
	// 
	// // reflection Sphere
	// sphereComp = CreateDefaultSubobject<USphereComponent>(FName("SphereComp"));
	// sphereComp->bHiddenInGame = !FVAPluginModule::isInDebugMode();
	// sphereComp->Mobility = EComponentMobility::Movable;
	// sphereComp->SetRelativeScale3D(FVector(1, 1, 1));
	// RootComponent = sphereComp;
	// 
	// SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	// SphereMesh->AttachTo(RootComponent);
	// static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	// if (SphereMeshAsset.Succeeded()) {
	// 	SphereMesh->SetStaticMesh(SphereMeshAsset.Object);
	// 	SphereMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	// 	SphereMesh->SetWorldScale3D(FVector(0.8f));
	// 	SphereMesh->SetVisibility(FVAPluginModule::isInDebugMode());
	// }



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
	
	// normalVec  = GetTransform().GetRotation().Rotator().Vector();
	// supportVec = GetTransform().GetLocation();

	// normalVec  = GetActorRotation().Vector();
	// supportVec = GetActorLocation();

	FRotator normalRot = planeComp->GetComponentRotation();
	normalVec = normalRot.Vector();


	FString text = "AVAReflectionWall::computePlaneData() Info for wall: Normal vec: ";
	text.Append(FString::FromInt(normalVec.X)).Append("/").Append(FString::FromInt(normalVec.Y)).Append("/").Append(FString::FromInt(normalVec.Z));
	text.Append(" // normal rot: ");
	text.Append(FString::FromInt(normalRot.Roll)).Append("/").Append(FString::FromInt(normalRot.Pitch)).Append("/").Append(FString::FromInt(normalRot.Yaw));
	VAUtils::logStuff(text, false);


	supportVec = planeComp->GetComponentLocation();

	d = FVector::DotProduct(normalVec, supportVec);


	alreadyComputed = true;
}

void AVAReflectionWall::spawnSphere(FVector pos, FRotator rot)
{
	VAUtils::openMessageBox("in spawnSphere of Reflection Wall, thats wrong");
	// FVector offset(0, 0, 40);
	// SphereMesh->SetWorldLocation(pos - offset);
	// SphereMesh->SetVisibility(FVAPluginModule::isInDebugMode());
	//SphereMesh->SetRelativeLocation(pos);
	//SphereMesh->Set

	return;

	// AStaticMeshActor *Mesh = nullptr;
	// int counter = 0;
	// for (TActorIterator<AStaticMeshActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	// {
	// 	Mesh = *ActorItr;
	// 	if (Mesh->GetName() == "ReflectionSphere") {
	// 		Mesh->SetActorLocationAndRotation(pos, rot.Quaternion());
	// 		counter++;
	// 	}
	// }
	// FString text = "Counter: ";
	// text.Append(FString::FromInt(counter));
	// VAUtils::logStuff(text);

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

