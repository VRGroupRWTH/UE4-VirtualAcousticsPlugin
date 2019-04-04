// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VAPlugin.h"
#include "CoreMinimal.h"
#include "Array.h"
#include "Components/ActorComponent.h"
#include "VASourceComponent.generated.h"


// struct used to get 
UENUM()
enum EMovement {
	ObjectSpawnPoint,
	MoveWithObject,
	OwnPosition,
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VAPLUGIN_API UVASourceComponent : public UActorComponent
{
	GENERATED_BODY()


	// In- or decrease Gain Offset // TODO not working yet
	UPROPERTY(EditAnywhere, meta = (DisplayName = "(in work) Gain"))
		int vGainOffset = 0;

	// In- or decrease Gain Offset
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Sound Name"))
		FString vSoundName = "Audiofiles/Bauer.wav";

	// Loop sound?
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Loop"))
		bool vLoop = false;

	// Start in x Seconds
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Starting time (s)"))
		float vDelay = 0.0f;

	// Decide whether to use manual Transform (below) or use Transform / Movement of Actor
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Position Settings" ,CustomStructureParam = "Objects Spawn Point, Move With Object, Own Position (below)"))
		TEnumAsByte<EMovement> vMovement = EMovement::ObjectSpawnPoint;

	// Manual Position (if used)
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Position"))
		FVector vPos = FVector(0, 0, 1.7);

	// Manual Rotation (if used)
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Rotation"))
		FRotator vRot = FRotator(0, 0, 0);



public:	
	// Sets default values for this component's properties
	UVASourceComponent();

	bool sendSoundData();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	AActor* ownerActor;

	int soundID;
	bool started;
	float timer;
	

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
