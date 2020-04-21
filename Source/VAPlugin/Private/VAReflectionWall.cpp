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

	bAlreadyComputed = false;

	PlaneComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("PlaneComp"));
	PlaneComponent->InitBoxExtent(FVector(1, 100, 100));
	PlaneComponent->SetVisibility(true);
}

// Called when the game starts or when spawned
void AVAReflectionWall::BeginPlay()
{
	Super::BeginPlay();

	if (!bAlreadyComputed)
	{
		ComputePlaneData();
	}
}

// Called every frame
void AVAReflectionWall::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}


FVector AVAReflectionWall::GetNormalVector() const
{
	return NormalVector;
}

FVector AVAReflectionWall::GetSupportVector() const
{
	return SupportVector;
}

float AVAReflectionWall::GetHessianD() const
{
	return HessianD;
}

float AVAReflectionWall::GetReflectionValueR() const
{
	return ReflectionValueR;
}

void AVAReflectionWall::ComputePlaneData()
{
	NormalVector  = PlaneComponent->GetComponentRotation().Vector();
	SupportVector = PlaneComponent->GetComponentLocation();

	HessianD = FVector::DotProduct(NormalVector, SupportVector);

	bAlreadyComputed = true;
}



