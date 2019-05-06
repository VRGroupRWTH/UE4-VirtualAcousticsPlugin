// Fill out your copyright notice in the Description page of Project Settings.

#include "VAReflectionWall.h"
#include "Components/BoxComponent.h"

// Sets default values
AVAReflectionWall::AVAReflectionWall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	normalVec = FVector(1, 0, 0);
	supportVec = FVector(0, 0, 0);
	d = 0;

	alreadyComputed = false;

	planeComp = CreateDefaultSubobject<UBoxComponent>(TEXT("PlaneComp"));
	planeComp->InitBoxExtent(FVector(1, 100, 100));

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

void AVAReflectionWall::computePlaneData()
{
	normalVec = GetTransform().GetRotation().Rotator().Vector();
	supportVec = GetTransform().GetLocation();

	d = FVector::DotProduct(normalVec, supportVec);


	alreadyComputed = true;
}

void AVAReflectionWall::spawnSphere(FVector pos, FRotator rot)
{
	float size = 50.0f;

	sphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	sphereComp->SetAbsolute(true, true, true);
	sphereComp->InitSphereRadius(size);
	sphereComp->SetWorldLocation(pos);
	//sphereComp->SetVisibility(true);

	dirComp = CreateDefaultSubobject<USphereComponent>(TEXT("dirComp"));
	dirComp->SetAbsolute(true, true, true);
	dirComp->InitSphereRadius(size / 4.0f);
	dirComp->SetWorldLocation(pos + size*rot.Vector());
	// dirComp->SetVisibility(true);

	
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

