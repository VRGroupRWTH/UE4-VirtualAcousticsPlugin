// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "SoundSourceRepresentation.generated.h"

UCLASS()
class VAPLUGIN_API ASoundSourceRepresentation : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASoundSourceRepresentation();

	UPROPERTY(VisibleDefaultsOnly)
		class USphereComponent* sphereComp;

	UPROPERTY(VisibleDefaultsOnly)
		class UStaticMeshComponent* sphereMesh;

	bool setPos(FVector pos);

	bool setRot(FRotator rot);

	bool setVis(bool visibility);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
