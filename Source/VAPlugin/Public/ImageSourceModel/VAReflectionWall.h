// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "VAReflectionWall.generated.h"

UCLASS(ClassGroup = (VA))
class VAPLUGIN_API AVAReflectionWall : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category = ReflectionWall)
	class UBoxComponent* PlaneComponent;

	// In- or decrease Gain Offset 
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Reflection Factor, 1 = full reflection, 0 = no reflection", ClampMin = "0.0", ClampMax = "1.0", 
		UIMin = "0.0", UIMax = "1.0"))
	float ReflectionValueR = 1;

public:
	// Sets default values for this actors properties
	AVAReflectionWall();

	// Getter functions
	FVector GetNormalVector() const;
	FVector GetSupportVector() const;
	float GetHessianD() const;
	float GetReflectionValueR() const;



protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

	// Data of plane
	FVector NormalVector;
	FVector SupportVector;
	float HessianD;

public:
	// Called every frame
	void Tick(float DeltaTime) override;
};
