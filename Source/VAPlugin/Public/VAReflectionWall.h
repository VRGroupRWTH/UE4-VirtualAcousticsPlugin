// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "VAReflectionWall.generated.h"

UCLASS()
class VAPLUGIN_API AVAReflectionWall : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category = ReflectionWall)
	class UBoxComponent* planeComp;

	// In- or decrease Gain Offset 
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Reflection Factor", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0"
		, UIMax = "1.0"))
	float vR = 1;

public:
	// Sets default values for this actor's properties
	AVAReflectionWall();

	// Return the Normal Vector of the Plane
	FVector getNormalVec();

	// returns support Vector of NormalForm (x-p)*n = 0
	FVector getSupportVec();

	// return d of Hesian normal form n*x = d (= p*n)
	float getD();

	// return R = reflection Value for Wall
	float getR();

	// Compute all Vecs
	void computePlaneData();

	// spawn Sphere to represent mirrored source
	void spawnSphere(FVector pos, FRotator rot);

protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

	// Normal Vector of Plane
	FVector normalVec;

	// Support Vector of Normalform
	FVector supportVec;

	// D of Hessian normal form
	float d;

	// is PlaneData already computed?
	bool alreadyComputed;

	// // sphere to show // 
	// UStaticMeshComponent* SphereMesh;


public:
	// Called every frame
	void Tick(float DeltaTime) override;
};
