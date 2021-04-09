// Fill out your copyright notice in the Description page of Project Settings.

#include "ImageSourceModel/VAReflectionWall.h"

#include "EngineUtils.h"
#include "Components/BoxComponent.h"


// Sets default values
AVAReflectionWall::AVAReflectionWall()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlaneComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("PlaneComp"));
	PlaneComponent->InitBoxExtent(FVector(1, 100, 100));
	PlaneComponent->SetVisibility(true);
}

// Called when the game starts or when spawned
void AVAReflectionWall::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AVAReflectionWall::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}


FVector AVAReflectionWall::GetNormalVector() const
{
	return PlaneComponent->GetComponentRotation().Vector();
}

FVector AVAReflectionWall::GetSupportVector() const
{
	return PlaneComponent->GetComponentLocation();
}

float AVAReflectionWall::GetHessianD() const
{
	return FVector::DotProduct(GetNormalVector(), GetSupportVector());
}

float AVAReflectionWall::GetReflectionValueR() const
{
	return ReflectionValueR;
}



