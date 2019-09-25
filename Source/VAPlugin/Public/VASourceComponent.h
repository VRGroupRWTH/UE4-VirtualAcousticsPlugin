// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VAPlugin.h"
#include "CoreMinimal.h"
#include "Array.h"
#include "Components/ActorComponent.h"

#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"

#include "VASourceComponent.generated.h"


// struct used to get 
UENUM()
enum EMovement {
	ObjectSpawnPoint,
	MoveWithObject,
	OwnPosition,
	Manual,
	Human
};

// struct used to get 
UENUM()
enum EPlayAction {
	Play,
	Pause, 
	Stop
};




UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VAPLUGIN_API UVASourceComponent : public UActorComponent
{
	GENERATED_BODY()


	// In- or decrease Gain Offset 
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Gain Factor", ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "4.0"))
		float vGainFactor = 1;

	// In- or decrease Gain Offset
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Sound Name"))
		FString vSoundName = "Audiofiles/Bauer.wav";

	// Loop sound?
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Action"))
		TEnumAsByte<EPlayAction> vAction = EPlayAction::Play;

	// Loop sound?
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Loop"))
		bool vLoop = false;

	// Start at Second x
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Starting Sound at Second x"))
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

	// Offset
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Offset"))
		FVector vOffset = FVector(0, 0, 0);

	// Manual Rotation (if used)
	// UPROPERTY(EditAnywhere, meta = (DisplayName = "Offset Rotation"))
	// 	bool vOffsetRot = false;



public:	
	// Sets default values for this component's properties
	UVASourceComponent();

	bool sendSoundData();

	// initialize Sound Source // 
	UFUNCTION(BlueprintCallable)
		void initialize();

	// Plays Sound // 
	UFUNCTION(BlueprintCallable)
		void playSound();

	// Pauses Sound //
	UFUNCTION(BlueprintCallable)
		void stopSound();

	// Stops Sound //
	UFUNCTION(BlueprintCallable)
		void pauseSound();

	// Update Position // 
	UFUNCTION(BlueprintCallable)
		void updatePosition(FVector vec, FRotator rot);

	// Plays Sound With Reflections // 
	UFUNCTION(BlueprintCallable)
		void playSoundWithReflections();

	// Pauses Sound With Reflections //
	UFUNCTION(BlueprintCallable)
		void stopSoundWithReflections();

	// Stops Sound With Reflections //
	UFUNCTION(BlueprintCallable)
		void pauseSoundWithReflections();

	// Update Position With Reflections // 
	UFUNCTION(BlueprintCallable)
		void updatePositionWithReflections(FVector vec, FRotator rot);

	// returns the Position minding its setting for location //
	UFUNCTION(BlueprintCallable)
		FVector getPosition();

	// returns the Roatation minding its setting for rotation // 
	UFUNCTION(BlueprintCallable)
		FRotator getRotation();

	// set directivity of sound source with reflections // 
	UFUNCTION(BlueprintCallable)
		bool setDirectivityWithReflections_Phoneme(FString phoneme);

	// set directivity of sound source with reflections // 
	UFUNCTION(BlueprintCallable)
		bool setDirectivity_Phoneme(FString phoneme);



protected:
	// Called when the game starts
	virtual void BeginPlay() override;

    // Link to Owner Actor //
	AActor* ownerActor;

    // Sound ID, used to identify Sound at VA Server //
	int soundID;
    
    // Check if Sound is started, used to start at different time //
	bool started;

	// Check if it is the first tick //
	bool firstTick;

	// Check if Sound data is already sent // 
	bool alreadySent;
    
    // Check time // 
	float timer;

	// name of face_bone // 
	FName face_bone_name;

	// skeletal_mesh
	USkeletalMeshComponent* skeletal_mesh_component;

	// root component
	USphereComponent* root_component;
	

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
